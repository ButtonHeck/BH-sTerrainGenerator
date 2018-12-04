#include "game/Game.h"

Game::Game(GLFWwindow *window, Camera& camera, Camera &shadowCamera, Options& options, ScreenResolution &screenResolution)
  :
    screenResolution(screenResolution),
    window(window),
    camera(camera),
    shadowCamera(shadowCamera),
    shadowNearFrustumRenderer(shadowNearFrustum),
    shadowMiddleFrustumRenderer(shadowMiddleFrustum),
    projection(glm::perspective(glm::radians(camera.getZoom()), screenResolution.getAspectRatio(), NEAR_PLANE, FAR_PLANE)),
    shadowNearProjection(glm::perspective(glm::radians(camera.getZoom()), screenResolution.getAspectRatio(), NEAR_PLANE, SHADOW_NEAR_DISTANCE)),
    shadowMiddleProjection(glm::perspective(glm::radians(camera.getZoom()), screenResolution.getAspectRatio(), SHADOW_NEAR_DISTANCE, SHADOW_FAR_DISTANCE)),
    shadowFarProjection(glm::perspective(glm::radians(camera.getZoom()), screenResolution.getAspectRatio(), SHADOW_FAR_DISTANCE, FAR_PLANE)),
    options(options),
    shaderManager(),
    textureLoader(TextureLoader(screenResolution)),
    textureManager(TextureManager(textureLoader)),
    csRenderer(CoordinateSystemRenderer(&shaderManager.get(SHADER_COORDINATE_SYSTEM))),
    screenBuffer(screenResolution, textureManager, shaderManager),
    depthmapBuffer(),
    scene(shaderManager, options, textureManager),
    shadowVolume(scene.getSunFacade()),
    shadowVolumeRenderer(shadowVolume),
    saveLoadManager(scene, camera),
    keyboard(KeyboardManager(window, camera, shadowCamera, options, scene.getSunFacade())),
    mouseInput(MouseInputManager::getInstance()),
    textManager(FONT_DIR + "font.fnt", FONT_DIR + "font.png", shaderManager.get(SHADER_FONT))
{
  srand(time(NULL));
  Model::bindTextureLoader(textureLoader);
}

Game::~Game()
{
  waterAnimator->join();
  meshIndirectBufferUpdater->join();
  BenchmarkTimer::finish(updateCount);
}

void Game::setup()
{
  BENCHMARK("Game: setup", false);
  Shader::cacheUniformsMode(UNIFORMS_NO_CACHE);
  RendererStateManager::setInitialRenderingState(options[OPT_USE_MULTISAMPLING]);
  MouseInputManager::setCallbacks(window);
  scene.setup();
  setupThreads();
  shaderManager.setupConstantUniforms(glm::ortho(0.0f, (float)screenResolution.getWidth(), 0.0f, (float)screenResolution.getHeight()));
  screenBuffer.setup();
  depthmapBuffer.setup(textureManager.get(TEX_DEPTH_MAP_SUN));
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

    if (!options[OPT_SHADOW_CAMERA_FIXED])
      {
        shadowCamera.processMouseCursor();
        shadowCamera.move(timerDelta, scene.getHillsFacade().getMap());
      }
  }

  scene.getSunFacade().move(timerDelta);

  glm::vec4 currentColor = glm::mix(NIGHT_SKY_COLOR, DAY_SKY_COLOR, glm::clamp(-shadowVolume.getLightDir().y * 5, 0.0f, 1.0f));
  glClearColor(currentColor.r, currentColor.g, currentColor.b, currentColor.a);

  {
    BENCHMARK("Game loop: wait mesh buffer ready", true);
    while(!meshBufferReady && !updateCount == 0)
      std::this_thread::yield();
    meshBufferReady = false;
  }

  if (options[OPT_RECREATE_TERRAIN_REQUEST])
    recreate();

  if (options[OPT_USE_SHADOWS])
    updateDepthmap();

  //render our world onto separate FBO as usual
  bool multisamplingEnabled = options[OPT_USE_MULTISAMPLING];
  screenBuffer.bindAppropriateFBO(multisamplingEnabled);
  drawFrame(projectionView);
  screenBuffer.draw(multisamplingEnabled);

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

void Game::drawFrame(glm::mat4& projectionView)
{
  BENCHMARK("Game loop: draw frame", true);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPolygonMode(GL_FRONT_AND_BACK, options[OPT_POLYGON_LINE] ? GL_LINE : GL_FILL);

  //EXPERIMENTAL
  if (options[OPT_CSM_VISUALIZATION])
    {
      shaderManager.get(SHADER_FRUSTUM).use();
      shaderManager.get(SHADER_FRUSTUM).setMat4("u_projectionView", projectionView);
      shaderManager.get(SHADER_FRUSTUM).setBool("u_isExpectedVolume", false);
      shaderManager.get(SHADER_FRUSTUM).setBool("u_isActualVolume", false);
      shaderManager.get(SHADER_FRUSTUM).setBool("u_isLightSource", false);
      if (options[OPT_FRUSTUM_VISUALIZATION])
        {
          shaderManager.get(SHADER_FRUSTUM).setBool("u_isNear", true);
          shadowNearFrustumRenderer.render();
          shaderManager.get(SHADER_FRUSTUM).setBool("u_isNear", false);
          shadowMiddleFrustumRenderer.render();
        }
      if (options[OPT_EXPECTED_VOLUME_VISUALIZATION])
        {
          shaderManager.get(SHADER_FRUSTUM).setBool("u_isExpectedVolume", true);
          shaderManager.get(SHADER_FRUSTUM).setBool("u_isNear", true);
          shadowVolumeRenderer.renderTerrainSquare(true);
          shaderManager.get(SHADER_FRUSTUM).setBool("u_isNear", false);
          shadowVolumeRenderer.renderTerrainSquare(false);
        }
      if (options[OPT_ACTUAL_VOLUME_VISUALIZATION])
        {
          shaderManager.get(SHADER_FRUSTUM).setBool("u_isActualVolume", true);
          shaderManager.get(SHADER_FRUSTUM).setBool("u_isNear", true);
          shadowVolumeRenderer.renderVolume(true, false);
          shaderManager.get(SHADER_FRUSTUM).setBool("u_isNear", false);
          shadowVolumeRenderer.renderVolume(false, false);
          shaderManager.get(SHADER_FRUSTUM).setBool("u_isLightSource", true);
          shaderManager.get(SHADER_FRUSTUM).setBool("u_isNear", true);
          shadowVolumeRenderer.renderVolume(true, true);
          shaderManager.get(SHADER_FRUSTUM).setBool("u_isNear", false);
          shadowVolumeRenderer.renderVolume(false, true);
        }
    }

  if (options[OPT_ANIMATE_WATER])
    {
      BENCHMARK("Water: buffer animation frame", true);
      scene.getWaterFacade().bufferNewData();
      waterNeedNewKeyFrame = true;
    }
  scene.getPlantsFacade().updateIndirectBufferData();
  glm::mat4 skyboxProjectionView(projection * glm::mat4(glm::mat3(camera.getViewMatrix())));

  scene.drawWorld(shadowVolume.getLightDir(),
                  shadowVolume.getLightSpaceMatrixNear(),
                  shadowVolume.getLightSpaceMatrixMiddle(),
                  shadowVolume.getLightSpaceMatrixFar(),
                  projectionView,
                  skyboxProjectionView,
                  viewFrustum,
                  camera,
                  mouseInput);

  //after all mesh related draw calls we could start updating meshes indirect data buffers
  //start updating right after we've used it and before we need that data to be updated and buffered again
  meshBufferNeedUpdate = true;

  if (options[OPT_DRAW_DEBUG_TEXT])
    {
      textManager.addText(screenResolution,
                          camera,
                          options,
                          mouseInput,
                          scene.getSunFacade().getCurrentPosition(),
                          CPU_timer.getFPS());
      textManager.drawText();
      csRenderer.draw(glm::mat3(camera.getViewMatrix()), screenResolution.getAspectRatio());
    }

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

  glm::mat4 shadowNearProjectionView = shadowNearProjection * shadowView;
  shadowNearFrustum.updateFrustum(shadowNearProjectionView);
  shadowNearFrustum.calculateIntersectionPoints();

  glm::mat4 shadowMiddleProjectionView = shadowMiddleProjection * shadowView;
  shadowMiddleFrustum.updateFrustum(shadowMiddleProjectionView);
  shadowMiddleFrustum.calculateIntersectionPoints();

  glm::mat4 shadowFarProjectionView = shadowFarProjection * shadowView;
  shadowFarFrustum.updateFrustum(shadowFarProjectionView);
  shadowFarFrustum.calculateIntersectionPoints();

  {
    BENCHMARK("Shadow volume: update", true);
    shadowVolume.update(shadowNearFrustum, shadowMiddleFrustum, shadowFarFrustum);
  }

  depthmapBuffer.bindToViewport(DEPTH_MAP_TEXTURE_WIDTH, DEPTH_MAP_TEXTURE_HEIGHT);
  scene.drawWorldDepthmap(shadowVolume.getLightSpaceMatrixNear(),
                          shadowVolume.getLightSpaceMatrixMiddle(),
                          shadowVolume.getLightSpaceMatrixFar());
  depthmapBuffer.unbindToViewport(screenResolution.getWidth(), screenResolution.getHeight());
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
          if (meshBufferNeedUpdate)
            {
              BENCHMARK("(ST)Model: update meshes DIBs data", true);
              float cameraOnMapX = glm::clamp(camera.getPosition().x, -HALF_WORLD_WIDTH_F, HALF_WORLD_WIDTH_F);
              float cameraOnMapZ = glm::clamp(camera.getPosition().z, -HALF_WORLD_HEIGHT_F, HALF_WORLD_HEIGHT_F);
              glm::vec2 cameraPositionXZ = glm::vec2(cameraOnMapX, cameraOnMapZ);
              scene.getPlantsFacade().prepareMeshesIndirectData(cameraPositionXZ, viewFrustum);
              meshBufferReady = true;
              meshBufferNeedUpdate = false;
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
