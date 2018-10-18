#include "game/Game.h"

Game::Game(GLFWwindow *window, Camera& camera, Options& options, ScreenResolution &screenResolution)
  :
    screenResolution(screenResolution),
    window(window),
    camera(camera),
    projection(glm::perspective(glm::radians(camera.getZoom()), screenResolution.getAspectRatio(), NEAR_PLANE, FAR_PLANE)),
    options(options),
    keyboard(KeyboardManager(window, camera, options)),
    mouseInput(MouseInputManager::getInstance()),
    shaderManager(),
    renderer(Renderer(camera)),
    textureLoader(TextureLoader(screenResolution)),
    textureManager(TextureManager(textureLoader)),
    csRenderer(CoordinateSystemRenderer(&shaderManager.get(SHADER_COORDINATE_SYSTEM))),
    screenBuffer(screenResolution, textureManager, shaderManager),
    depthmapBuffer(),
    textManager(FONT_DIR + "font.fnt", FONT_DIR + "font.png", shaderManager.get(SHADER_FONT))
{
  srand(time(NULL));
  Model::bindTextureLoader(textureLoader);
  worldFacade = std::make_shared<WorldGeneratorFacade>(shaderManager, renderer, options, textureManager);
  saveLoadManager = std::make_unique<SaveLoadManager>(worldFacade, camera);
}

Game::~Game()
{
  waterAnimator->join();
  meshIndirectBufferUpdater->join();
  BenchmarkTimer::finish(updateCount);
}

void Game::setup()
{
  Shader::cacheUniformsMode(UNIFORMS_NO_CACHE);
  Renderer::setInitialGLState(options.get(OPT_USE_MULTISAMPLING));
  MouseInputManager::setCallbacks(window);
  worldFacade->setup();
  setupThreads();
  shaderManager.setupConstantUniforms(glm::ortho(0.0f, (float)screenResolution.getWidth(), 0.0f, (float)screenResolution.getHeight()));
  screenBuffer.setup();
  depthmapBuffer.setup(textureManager.get(TEX_DEPTH_MAP_SUN));
}

void Game::loop()
{
  keyboard.processInput(CPU_timer.tick(), worldFacade->getHillsGenerator()->getMap());
  glm::mat4 view = camera.getViewMatrix();
  glm::mat4 projectionView = projection * view;
  viewFrustum.updateFrustum(projectionView);

  while(!meshBufferReady && !updateCount == 0 && meshBufferNeedUpdate)
    std::this_thread::yield();
  meshBufferReady = false;

  if (options.get(OPT_RECREATE_TERRAIN_REQUEST))
    recreate();

  if ((options.get(OPT_CREATE_SHADOW_MAP_REQUEST) || updateCount % 16 == 0) && options.get(OPT_USE_SHADOWS))
    updateDepthmap();

  //render our world onto separate FBO as usual
  bool multisamplingEnabled = options.get(OPT_USE_MULTISAMPLING);
  screenBuffer.bindAppropriateFBO(multisamplingEnabled);
  drawFrameObjects(projectionView);

  {
    BENCHMARK("Game: draw frame to screen", true);
    screenBuffer.draw(multisamplingEnabled);
  }

  if (options.get(OPT_SAVE_REQUEST))
    saveState();
  if (options.get(OPT_LOAD_REQUEST))
    loadState();

  {
    BENCHMARK("Game: glfwSwapBuffers", true);
    glfwSwapBuffers(window);
  }
  ++updateCount;
}

void Game::drawFrameObjects(glm::mat4& projectionView)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPolygonMode(GL_FRONT_AND_BACK, options.get(OPT_POLYGON_LINE) ? GL_LINE : GL_FILL);

  if (options.get(OPT_ANIMATE_WATER))
    {
      BENCHMARK("Water: buffer animation frame", true);
      worldFacade->getWaterGenerator()->bufferNewData();
      waterNeedNewKeyFrame = true;
    }
  glm::mat4 skyboxProjectionView(projection * glm::mat4(glm::mat3(camera.getViewMatrix())));
  worldFacade->drawWorld(projectionView,
                         skyboxProjectionView,
                         viewFrustum,
                         camera,
                         mouseInput,
                         updateCount);

  //after all mesh related draw calls we could start updating meshes indirect data buffers
  //start updating right after we've used it and before we need that data to be updated and buffered again
  meshBufferNeedUpdate = updateCount % MESH_INDIRECT_BUFFER_UPDATE_FREQ == 1;

  if (options.get(OPT_DRAW_DEBUG_TEXT))
    {
      textManager.addText(screenResolution,
                          camera,
                          options,
                          mouseInput,
                          CPU_timer.getFPS(),
                          waterAnimatorIsWorking);
      textManager.drawText();
      csRenderer.draw(glm::mat3(camera.getViewMatrix()), screenResolution.getAspectRatio());
    }

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Game::recreate()
{
  while(!waterKeyFrameReady)
    std::this_thread::yield();//busy wait until water thread has done its business...and business is good
  waterNeedNewKeyFrame = false; //explicitly bypass water animation frame update routine
  worldFacade->recreate();
  options.set(OPT_RECREATE_TERRAIN_REQUEST, false);
  waterNeedNewKeyFrame = true; //it's okay now to begin animating water
}

void Game::updateDepthmap()
{
  depthmapBuffer.bindToViewport(DEPTH_MAP_TEXTURE_WIDTH, DEPTH_MAP_TEXTURE_HEIGHT);
  worldFacade->drawWorldDepthmap(updateCount);
  depthmapBuffer.unbindToViewport(screenResolution.getWidth(), screenResolution.getHeight());
  options.set(OPT_CREATE_SHADOW_MAP_REQUEST, false);
}

void Game::saveState()
{
  saveLoadManager->saveToFile(SAVES_DIR + "testSave.txt");
  options.set(OPT_SAVE_REQUEST, false);
}

void Game::loadState()
{
  while(!waterKeyFrameReady)
    std::this_thread::yield(); //busy wait
  waterNeedNewKeyFrame = false;
  saveLoadManager->loadFromFile(SAVES_DIR + "testSave.txt");
  worldFacade->load();
  options.set(OPT_LOAD_REQUEST, false);
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
              worldFacade->getPlantsGeneratorFacade()->prepareMeshesIndirectData(cameraPositionXZ, viewFrustum);
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
                  options.get(OPT_ANIMATE_WATER) &&
                  options.get(OPT_DRAW_WATER))
                {
                  waterKeyFrameReady = false;
                  worldFacade->getWaterGenerator()->updateAnimationFrame(options);
                  waterKeyFrameReady = true;
                  waterNeedNewKeyFrame = false;
                  waterAnimatorIsWorking = true;
                }
              else
                waterAnimatorIsWorking = false;
              std::this_thread::yield();
            }
        });
}
