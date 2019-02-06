#include "game/Game.h"

Game::Game(GLFWwindow *window, Camera& camera, Camera &shadowCamera, Options& options, ScreenResolution &screenResolution)
  :
    screenResolution(screenResolution),
    window(window),
    camera(camera),
    shadowCamera(shadowCamera),
    shadowFrustumRenderers({{shadowCameraFrustums[0], shadowCameraFrustums[1]}}),
    projection(glm::perspective(glm::radians(camera.getZoom()), screenResolution.getAspectRatio(), NEAR_PLANE, FAR_PLANE)),
    cullingProjection(glm::perspective(glm::radians(camera.getZoom() + 10.0f), screenResolution.getAspectRatio(), NEAR_PLANE, FAR_PLANE)),
    options(options),
    shaderManager(),
    textureLoader(TextureLoader(screenResolution)),
    textureManager(TextureManager(textureLoader)),
    csRenderer(CoordinateSystemRenderer(&shaderManager.get(SHADER_COORDINATE_SYSTEM))),
    screenFramebuffer(textureManager, screenResolution, shaderManager),
    depthmapFramebuffer(textureManager),
    reflectionFramebuffer(textureManager),
    refractionFramebuffer(textureManager),
    scene(shaderManager, options, textureManager, screenResolution),
    shadowVolume(scene.getSunFacade()),
    shadowVolumeRenderer(shadowVolume),
    saveLoadManager(scene, camera, shadowCamera),
    keyboard(KeyboardManager(window, camera, shadowCamera, options, scene.getSunFacade())),
    mouseInput(MouseInputManager::getInstance()),
    textManager(FONT_DIR + "font.fnt", FONT_DIR + "font.png", shaderManager.get(SHADER_FONT), screenResolution)
{
  srand(time(NULL));
  Model::bindTextureLoader(textureLoader);
  shadowProjections[0] = glm::perspective(glm::radians(camera.getZoom()), screenResolution.getAspectRatio(), NEAR_PLANE, SHADOW_DISTANCE_LAYER1);
  shadowProjections[1] = glm::perspective(glm::radians(camera.getZoom()), screenResolution.getAspectRatio(), SHADOW_DISTANCE_LAYER1, SHADOW_DISTANCE_LAYER2);
  shadowProjections[2] = glm::perspective(glm::radians(camera.getZoom()), screenResolution.getAspectRatio(), SHADOW_DISTANCE_LAYER2, FAR_PLANE);
}

Game::~Game()
{
  waterAnimator->join();
  meshIndirectBufferUpdater->join();
  BenchmarkTimer::finish(updateCount);
  BindlessTextureManager::makeAllNonResident();
}

void Game::setup()
{
  BENCHMARK("Game: setup", false);
  Shader::cacheUniformsMode(false);
  RendererStateManager::setInitialRenderingState(options[OPT_USE_MULTISAMPLING]);
  MouseInputManager::setCallbacks(window);
  scene.setup();
  BindlessTextureManager::makeAllResident();
  BindlessTextureManager::loadToShader(shaderManager.get(SHADER_MODELS_GOURAUD), BINDLESS_TEXTURE_MODEL);
  BindlessTextureManager::loadToShader(shaderManager.get(SHADER_MODELS_PHONG), BINDLESS_TEXTURE_MODEL);
  BindlessTextureManager::loadToShader(shaderManager.get(SHADER_LENS_FLARE), BINDLESS_TEXTURE_LENS_FLARE);
  setupThreads();
  shaderManager.setupConstantUniforms(screenResolution);
  screenFramebuffer.setup();
  depthmapFramebuffer.setup();
  reflectionFramebuffer.setup();
  refractionFramebuffer.setup();
}

void Game::loop()
{
  glm::mat4 view, projectionView;
  float timerDelta = CPU_timer.tick();
  {
    BENCHMARK("Game loop: process input and camera", true);
    keyboard.processInput();
    camera.processMouseCursor();
    camera.move(timerDelta, scene.getHillsFacade().getMap());
    if (!options[OPT_USE_SHADOW_CAMERA_MATRIX])
      view = camera.getViewMatrix();
    else
      view = shadowCamera.getViewMatrix();
    projectionView = projection * view;
    viewFrustum.updateFrustum(projectionView);
    cullingViewFrustum.updateFrustum(cullingProjection * view);

    if (!options[OPT_SHADOW_CAMERA_FIXED])
      {
        shadowCamera.processMouseCursor();
        shadowCamera.move(timerDelta, scene.getHillsFacade().getMap());
      }
  }

  scene.getSunFacade().move(timerDelta);
  scene.getSkysphereFacade().moveStarsSkysphere(timerDelta * PLANET_MOVE_SPEED);

  glm::vec4 currentColor = glm::mix(NIGHT_SKY_COLOR, DAY_SKY_COLOR, glm::clamp(-shadowVolume.getLightDir().y * 5, 0.0f, 1.0f));
  glClearColor(currentColor.r, currentColor.g, currentColor.b, currentColor.a);

  landIndirectBufferHasUpdated = false;
  {
    BENCHMARK("Game loop: wait models DIBs ready", true);
    while(!modelsIndirectBufferPrepared && updateCount != 0)
      {
        if (!landIndirectBufferHasUpdated)
          {
            scene.getLandFacade().updateCellsIndirectBuffer(viewFrustum);
            landIndirectBufferHasUpdated = true;
          }
        else
          std::this_thread::yield();
      }
    modelsIndirectBufferPrepared = false;
  }
  if (!landIndirectBufferHasUpdated)
    scene.getLandFacade().updateCellsIndirectBuffer(viewFrustum);

  if (options[OPT_RECREATE_TERRAIN_REQUEST])
    recreate();

  scene.getPlantsFacade().updateIndirectBufferData();
  if (options[OPT_USE_SHADOWS] && updateCount % 2)
    updateDepthmap();

  if(scene.getWaterFacade().hasWaterInFrame())
    {
      reflectionFramebuffer.bindToViewport(FRAME_WATER_REFLECTION_WIDTH, FRAME_WATER_REFLECTION_HEIGHT);
      drawFrameReflection();
      refractionFramebuffer.bindToViewport(FRAME_WATER_REFRACTION_WIDTH, FRAME_WATER_REFRACTION_HEIGHT);
      drawFrameRefraction(projectionView);
      refractionFramebuffer.unbindToViewport(screenResolution.getWidth(), screenResolution.getHeight());
    }

  //render our world onto separate FBO as usual
  bool multisamplingEnabled = options[OPT_USE_MULTISAMPLING];
  screenFramebuffer.bindAppropriateFBO(multisamplingEnabled);
  drawFrame(projectionView);
  screenFramebuffer.draw(multisamplingEnabled, options[OPT_USE_DOF]);

  if (options[OPT_SAVE_REQUEST])
    saveState();
  if (options[OPT_LOAD_REQUEST])
    loadState();

  {
    BENCHMARK("GLFW: swapBuffers", true);
    glfwSwapBuffers(window);
  }
  ++updateCount;
}

void Game::drawFrame(const glm::mat4 &projectionView)
{
  BENCHMARK("Game loop: draw frame", true);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPolygonMode(GL_FRONT_AND_BACK, options[OPT_POLYGON_LINE] ? GL_LINE : GL_FILL);

  if (options[OPT_CSM_VISUALIZATION])
    drawFrustumVisualizations(projectionView);

  if (options[OPT_ANIMATE_WATER])
    {
      BENCHMARK("Water: buffer animation frame", true);
      scene.getWaterFacade().bufferNewData();
      waterNeedNewKeyFrame = true;
    }

  scene.drawWorld(shadowVolume.getLightDir(),
                  shadowVolume.getLightSpaceMatrices(),
                  projectionView,
                  projection * glm::mat4(camera.getViewMatrixMat3()),
                  viewFrustum,
                  cullingViewFrustum,
                  camera,
                  mouseInput);

  //after all mesh related draw calls we could start updating meshes indirect data buffers
  //start updating right after we've used it and before we need that data to be updated and buffered again
  modelsIndirectBufferNeedUpdate = true;

  if (options[OPT_DRAW_DEBUG_TEXT])
    {
      textManager.addText(camera, options, mouseInput, scene.getSunFacade().getPosition(), CPU_timer.getFPS());
      textManager.drawText();
      csRenderer.draw(camera.getViewMatrixMat3(), screenResolution.getAspectRatio());
    }

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Game::drawFrustumVisualizations(const glm::mat4& projectionView)
{
  Shader& frustumShader = shaderManager.get(SHADER_FRUSTUM);
  frustumShader.use();
  frustumShader.setMat4("u_projectionView", projectionView);
  if (options[OPT_FRUSTUM_VISUALIZATION])
    {
      frustumShader.setInt("u_colorIndex", 3);
      shadowFrustumRenderers[0].render();
      frustumShader.setInt("u_colorIndex", 4);
      shadowFrustumRenderers[1].render();
    }
  if (options[OPT_EXPECTED_VOLUME_VISUALIZATION])
    {
      shadowVolumeRenderer.bufferExpectedVolumes();
      frustumShader.setInt("u_colorIndex", 6);
      shadowVolumeRenderer.renderExpectedVolume(0);
      frustumShader.setInt("u_colorIndex", 7);
      shadowVolumeRenderer.renderExpectedVolume(1);
      frustumShader.setInt("u_colorIndex", 8);
      shadowVolumeRenderer.renderExpectedVolume(2);
    }
  if (options[OPT_ACTUAL_VOLUME_VISUALIZATION])
    {
      shadowVolumeRenderer.bufferActualVolumes();
      frustumShader.setInt("u_colorIndex", 9);
      shadowVolumeRenderer.renderActualVolume(0);
      frustumShader.setInt("u_colorIndex", 10);
      shadowVolumeRenderer.renderActualVolume(1);
      frustumShader.setInt("u_colorIndex", 11);
      shadowVolumeRenderer.renderActualVolume(2);
      frustumShader.setInt("u_colorIndex", 0);
      shadowVolumeRenderer.renderLightSource(0);
      frustumShader.setInt("u_colorIndex", 1);
      shadowVolumeRenderer.renderLightSource(1);
      frustumShader.setInt("u_colorIndex", 2);
      shadowVolumeRenderer.renderLightSource(2);
    }
}

void Game::drawFrameReflection()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (options[OPT_USE_MULTISAMPLING])
    glDisable(GL_MULTISAMPLE);

  glm::mat4 viewReflected = camera.getReflectionViewMatrix();
  scene.drawWorldReflection(shadowVolume.getLightDir(),
                            shadowVolume.getLightSpaceMatrices(),
                            projection * viewReflected,
                            projection * glm::mat4(glm::mat3(viewReflected)),
                            cullingViewFrustum,
                            camera);

  if (options[OPT_USE_MULTISAMPLING])
    glEnable(GL_MULTISAMPLE);
}

void Game::drawFrameRefraction(const glm::mat4 &projectionView)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (options[OPT_USE_MULTISAMPLING])
    glDisable(GL_MULTISAMPLE);

  scene.drawWorldRefraction(shadowVolume.getLightDir(),
                            shadowVolume.getLightSpaceMatrices(),
                            projectionView);

  if (options[OPT_USE_MULTISAMPLING])
    glEnable(GL_MULTISAMPLE);
}

void Game::recreate()
{
  BENCHMARK("Game loop: recreate", true);
  while(!waterKeyFrameReady)
    std::this_thread::yield();//busy wait until water thread has done its business...and business is good
  waterNeedNewKeyFrame = false; //explicitly bypass water animation frame update routine
  scene.recreate();
  options[OPT_RECREATE_TERRAIN_REQUEST] = false;
  waterNeedNewKeyFrame = true; //it's okay now to begin animating water
}

void Game::updateDepthmap()
{
  glm::mat4 shadowView = shadowCamera.getViewMatrix();
  for (unsigned int layer = 0; layer < NUM_SHADOW_LAYERS; layer++)
    {
      shadowCameraFrustums[layer].updateFrustum(shadowProjections[layer] * shadowView);
      shadowCameraFrustums[layer].calculateIntersectionPoints();
    }
  {
    BENCHMARK("Shadow volume: update", true);
    shadowVolume.update(shadowCameraFrustums);
  }

  depthmapFramebuffer.bindToViewport(DEPTH_MAP_TEXTURE_WIDTH, DEPTH_MAP_TEXTURE_HEIGHT);
  scene.drawWorldDepthmap(shadowVolume.getLightSpaceMatrices(), options[OPT_GRASS_SHADOW]);
  depthmapFramebuffer.unbindToViewport(screenResolution.getWidth(), screenResolution.getHeight());
}

void Game::saveState()
{
  saveLoadManager.saveToFile(SAVES_DIR + "testSave.txt");
  options[OPT_SAVE_REQUEST] = false;
}

void Game::loadState()
{
  while(!waterKeyFrameReady)
    std::this_thread::yield(); //busy wait
  waterNeedNewKeyFrame = false;
  saveLoadManager.loadFromFile(SAVES_DIR + "testSave.txt");
  scene.load();
  options[OPT_LOAD_REQUEST] = false;
  waterNeedNewKeyFrame = true;
}

void Game::setupThreads()
{
  meshIndirectBufferUpdater = std::make_unique<std::thread>([this]()
  {
      while(!glfwWindowShouldClose(window))
        {
          if (modelsIndirectBufferNeedUpdate)
            {
              BENCHMARK("(ST)Model: update meshes DIBs data", true);
              float cameraOnMapX = glm::clamp(camera.getPosition().x, -HALF_WORLD_WIDTH_F, HALF_WORLD_WIDTH_F);
              float cameraOnMapZ = glm::clamp(camera.getPosition().z, -HALF_WORLD_HEIGHT_F, HALF_WORLD_HEIGHT_F);
              glm::vec2 cameraPositionXZ = glm::vec2(cameraOnMapX, cameraOnMapZ);
              scene.getPlantsFacade().prepareIndirectBufferData(cameraPositionXZ, viewFrustum);
              modelsIndirectBufferPrepared = true;
              modelsIndirectBufferNeedUpdate = false;
            }
          std::this_thread::yield();
        }
    });
  waterAnimator = std::make_unique<std::thread>([this]()
  {
      while(!glfwWindowShouldClose(window))
            {
              if (waterNeedNewKeyFrame &&
                  options[OPT_ANIMATE_WATER] &&
                  options[OPT_DRAW_WATER])
                {
                  waterKeyFrameReady = false;
                  scene.getWaterFacade().updateAnimationFrame(glfwGetTime(), options);
                  waterKeyFrameReady = true;
                  waterNeedNewKeyFrame = false;
                }
              std::this_thread::yield();
            }
        });
}
