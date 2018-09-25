#include "Game.h"

#ifdef _DEBUG
int ram_available, ram_size;
float ram_size_float_percentage;
#endif

Game::Game(GLFWwindow *window, glm::vec3 &cursorDir, Camera& camera, Options& options, ScreenResolution &screenResolution)
  :
    screenResolution(screenResolution),
    window(window),
    camera(camera),
    cursorToViewportDirection(cursorDir),
    projection(glm::perspective(glm::radians(camera.getZoom()), screenResolution.getAspectRatio(), NEAR_PLANE, FAR_PLANE)),
    options(options),
    keyboard(KeyboardManager(window, camera, options)),
    shaderManager(),
    renderer(Renderer(camera)),
    textureLoader(TextureLoader(screenResolution)),
    csRenderer(CoordinateSystemRenderer(&shaderManager.get(SHADER_CS)))
{
  srand(time(NULL));
  textureManager = new TextureManager(textureLoader);
  waterMapGenerator = new WaterMapGenerator(shaderManager.get(SHADER_WATER_CULLING));
  hillMapGenerator = new HillsMapGenerator(shaderManager.get(SHADER_HILLS_CULLING), waterMapGenerator->getMap());
  baseMapGenerator = new BaseMapGenerator(waterMapGenerator->getMap(), hillMapGenerator->getMap());
  buildableMapGenerator = new BuildableMapGenerator(baseMapGenerator->getMap(), hillMapGenerator->getMap());
  saveLoadManager = new SaveLoadManager(*baseMapGenerator, *hillMapGenerator, *waterMapGenerator, buildableMapGenerator, camera);
  fontManager = new FontManager(FONT_DIR + "font.fnt", FONT_DIR + "font.png", glm::ortho(0.0f, (float)screenResolution.getWidth(), 0.0f, (float)screenResolution.getHeight()), shaderManager.get(SHADER_FONT));
  Model::bindTextureLoader(textureLoader);
  plantGenerator = new PlantGenerator(NUM_GRASS_MODELS);
  saveLoadManager->setTreeGenerator(*plantGenerator);
#ifdef _DEBUG
  glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &ram_size);
  ram_size_float_percentage = (float)ram_size / 100;
#endif
}

Game::~Game()
{
  waterAnimationThread->join();
  delete waterAnimationThread;
  meshIndirectUpdateThread->join();
  delete meshIndirectUpdateThread;
  textureManager->deleteTextures();
  shaderManager.deleteShaders();
  delete fontManager;
  delete textureManager;
  delete baseMapGenerator;
  delete hillMapGenerator;
  delete waterMapGenerator;
  delete saveLoadManager;
  delete buildableMapGenerator;
  delete plantGenerator;
  BenchmarkTimer::finish(updateCount);
}

void Game::setupVariables()
{
  Shader::cacheUniformsMode(UNIFORMS_NO_CACHE);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_DITHER);
  if (options.get(OPT_USE_MULTISAMPLING))
    glEnable(GL_MULTISAMPLE);
  else
    glDisable(GL_MULTISAMPLE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glfwSetCursorPosCallback(window, MouseInputManager::cursorCallback);
  glfwSetMouseButtonCallback(window, MouseInputManager::cursorClickCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  {
    BENCHMARK("Game: Prepare Terrain", false);
    prepareTerrain();
  }
  meshIndirectUpdateThread = new std::thread([this]()
  {
      auto& plainPlants = plantGenerator->getPlainPlants();
      auto& hillTrees = plantGenerator->getHillTrees();
      auto& plainChunks = plantGenerator->getPlainPlantsModelChunks();
      auto& hillChunks = plantGenerator->getHillTreeModelChunks();
      while(!glfwWindowShouldClose(window))
        {
          if (meshesIndirectDataNeed)
            {
              BENCHMARK("(ST)Model: update meshes DIBs data", true);
              float cameraOnMapX = glm::clamp(camera.getPosition().x, -(float)HALF_WORLD_WIDTH, (float)HALF_WORLD_WIDTH);
              float cameraOnMapZ = glm::clamp(camera.getPosition().z, -(float)HALF_WORLD_HEIGHT, (float)HALF_WORLD_HEIGHT);
              glm::vec2 cameraPositionXZ = glm::vec2(cameraOnMapX, cameraOnMapZ);
              for (unsigned int i = 0; i < plainPlants.size(); i++)
                {
                  Model& model = plainPlants[i];
                  model.prepareMeshesIndirectData(plainChunks, i, cameraPositionXZ, viewFrustum);
                }
              for (unsigned int i = 0; i < hillTrees.size(); i++)
                {
                  Model& model = hillTrees[i];
                  model.prepareMeshesIndirectData(hillChunks, i, cameraPositionXZ, viewFrustum);
                }
              meshesIndirectDataReady = true;
              meshesIndirectDataNeed = false;
            }
          std::this_thread::yield();
        }
    });
  waterAnimationThread = new std::thread([this]()
  {
      while(!glfwWindowShouldClose(window))
            {
              if (waterThreadUpdatePermitted &&
                  options.get(OPT_ANIMATE_WATER) &&
                  options.get(OPT_DRAW_WATER))
                {
                  waterThreadHasUpdated = false;
                  waterMapGenerator->updateAnimationFrame(options);
                  waterThreadHasUpdated = true;
#ifdef _DEBUG
                  waterThreadAnimationIsWorking = true;
#endif
                }
              else
                {
#ifdef _DEBUG
                  waterThreadAnimationIsWorking = false;
#endif
                  std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
              std::this_thread::yield();
            }
        });

  textureManager->createUnderwaterReliefTexture(waterMapGenerator);
  shaderManager.setupConstantUniforms();
  prepareScreenVAO();
  prepareMS_FBO();
  prepareDepthMapFBO(&depthMapFBO, TEX_DEPTH_MAP_SUN);
}

void Game::prepareTerrain()
{
  waterMapGenerator->prepareMap(); //prepare water map
  hillMapGenerator->prepareMap(); //generating hill height map
  hillMapGenerator->fillBufferData(); //fill hills buffer
  baseMapGenerator->prepareMap(); //generating base terrain data
  baseMapGenerator->fillShoreBufferData(); //fill base terrain vertex data
  baseMapGenerator->fillSquareBufferData(); //generating data for chunk instance rendering
  baseMapGenerator->fillCellBufferData(); //generating data for 1x1 tile instance rendering
  waterMapGenerator->postPrepareMap();
  waterMapGenerator->fillBufferData(); //fill water buffer
  buildableMapGenerator->prepareMap();
  buildableMapGenerator->fillBufferData();
  plantGenerator->setupPlainModels(baseMapGenerator->getMap(), hillMapGenerator->getMap());
  plantGenerator->setupHillModels(hillMapGenerator->getMap());
}

void Game::prepareMS_FBO()
{
  //multisample
  glGenFramebuffers(1, &multisampleFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, multisampleFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureManager->get(TEX_FRAME_MULTISAMPLED), 0);
  GLuint msDepthRbo;
  glGenRenderbuffers(1, &msDepthRbo);
  glBindRenderbuffer(GL_RENDERBUFFER, msDepthRbo);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, MULTISAMPLES, GL_DEPTH24_STENCIL8, screenResolution.getWidth(), screenResolution.getHeight());
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msDepthRbo);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE)
    std::cout << "MS Framebuffer is not complete\n";
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //intermediate FBO (or direct off-screen FBO without multisampling)
  glGenFramebuffers(1, &screenFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         HDR_ENABLED ? textureManager->get(TEX_FRAME_HDR) : textureManager->get(TEX_FRAME), 0);
  //we don't need depth data if we use this FBO as intermediate, but we DO need it if theres no multisampling
  GLuint screenDepthRbo;
  glGenRenderbuffers(1, &screenDepthRbo);
  glBindRenderbuffer(GL_RENDERBUFFER, screenDepthRbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenResolution.getWidth(), screenResolution.getHeight());
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, screenDepthRbo);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Intermediate Framebuffer is not complete\n";
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::prepareDepthMapFBO(GLuint* fbo, GLuint depthTextureUnit)
{
  glGenFramebuffers(1, fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureManager->get(depthTextureUnit), 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Depthmap framebuffer is not complete\n";
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::prepareScreenVAO()
{
  glGenVertexArrays(1, &screenVAO);
  glGenBuffers(1, &screenVBO);
  glBindVertexArray(screenVAO);
  glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(SCREEN_VERTICES), &SCREEN_VERTICES, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Game::drawFrameToScreenRectangle(bool enableMS)
{
  if (enableMS)
    {
      glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampleFBO);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, screenFBO);
      glBlitFramebuffer(0, 0, screenResolution.getWidth(), screenResolution.getHeight(), 0, 0, screenResolution.getWidth(), screenResolution.getHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
  else
    {
      glBindFramebuffer(GL_READ_FRAMEBUFFER, screenFBO);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
  shaderManager.get(SHADER_MS_TO_DEFAULT).use();
  glBindVertexArray(screenVAO);
  glDisable(GL_DEPTH_TEST);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glEnable(GL_DEPTH_TEST);
}

void Game::drawFrameObjects(glm::mat4& projectionView)
{
  if (options.get(OPT_POLYGON_LINE))
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glm::vec3 viewPosition = camera.getPosition();

  if (options.get(OPT_ANIMATE_WATER))
    {
      BENCHMARK("Water: buffer animation frame", true);
      waterMapGenerator->bufferVertices();
    }

  //hills rendering
  shaderManager.updateHillsShaders(options.get(OPT_HILLS_CULLING), options.get(OPT_USE_SHADOWS), projectionView, viewPosition, viewFrustum, hillMapGenerator->getMaxHeight());
  {
    BENCHMARK("Renderer: draw hills", true);
    renderer.drawHills(options.get(OPT_HILLS_CULLING), hillMapGenerator, shaderManager.get(SHADER_HILLS_CULLING), shaderManager.get(SHADER_HILLS));
  }

  //flat terrain chunks drawing
  if (options.get(OPT_DRAW_FLAT_TERRAIN))
    {
      shaderManager.updateFlatShader(projectionView, options.get(OPT_USE_SHADOWS));
      {
        BENCHMARK("Renderer: draw flat", true);
        renderer.drawFlatTerrain(baseMapGenerator, viewFrustum);
      }
    }

  //underwater tile
  shaderManager.updateUnderwaterShader(projectionView);
  renderer.drawUnderwaterQuad(&underwaterQuadGenerator);

  //shore terrain chunks drawing
  shaderManager.updateShoreShader(projectionView, options.get(OPT_USE_SHADOWS));
  {
    BENCHMARK("Renderer: draw shore", true);
    renderer.drawShore(baseMapGenerator);
  }

  //trees chunks rendering
  if (options.get(OPT_DRAW_TREES))
    {
      shaderManager.updateModelShader(projectionView, viewPosition,
                                      options.get(OPT_TREES_SHADOW_EMPHASIZE),
                                      options.get(OPT_USE_SHADOWS),
                                      options.get(OPT_MODELS_FLAT_BLENDING));
      {
        BENCHMARK("Renderer: draw models", true);
        renderer.drawPlants(plantGenerator,
                           options.get(OPT_MODELS_PHONG_SHADING) ? shaderManager.get(SHADER_MODELS_PHONG) : shaderManager.get(SHADER_MODELS),
                           options.get(OPT_MODELS_CULLING),
                           true,
                           updateCount % MESH_INDIRECT_BUFFER_UPDATE_FREQ == 0,
                           true,
                           options.get(OPT_MODELS_FLAT_BLENDING));
      }
    }
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
  //reset texture units to terrain textures after we done with models
  glBindTextureUnit(TEX_FLAT, textureManager->get(TEX_FLAT));
  glBindTextureUnit(TEX_FLAT_2, textureManager->get(TEX_FLAT_2));

  //buildable tiles
  if (options.get(OPT_DRAW_BUILDABLE))
    {
      shaderManager.updateBuildableShader(projectionView);
      {
        BENCHMARK("Renderer: draw buildable", true);
        renderer.drawBuildableTiles(buildableMapGenerator);
      }
    }

  //cursor selected tile
  if (options.get(OPT_SHOW_CURSOR))
    {
      mouseInput.updateCursorMappingCoordinates(camera, baseMapGenerator, hillMapGenerator, buildableMapGenerator);
      if (buildableMapGenerator->getMap()[mouseInput.getCursorMapZ()][mouseInput.getCursorMapX()] != 0)
        {
          glm::mat4 selectedModel;
          selectedModel = glm::translate(selectedModel, glm::vec3(-HALF_WORLD_WIDTH + mouseInput.getCursorMapX(), 0.0f, -HALF_WORLD_HEIGHT + mouseInput.getCursorMapZ()));
          shaderManager.updateSelectedShader(projectionView, selectedModel);
          renderer.drawSelectedTile(buildableMapGenerator);
        }
    }

  //water rendering
  if (options.get(OPT_DRAW_WATER))
    {
      shaderManager.updateWaterShaders(options.get(OPT_WATER_CULLING), projectionView, viewPosition, viewFrustum);
      {
        BENCHMARK("Renderer: draw water (full func)", true);
        renderer.drawWater(options.get(OPT_WATER_CULLING), waterMapGenerator, shaderManager.get(SHADER_WATER_CULLING), shaderManager.get(SHADER_WATER));
      }
    }

  //Skybox rendering
  glm::mat4 skyProjectionView = projection * glm::mat4(glm::mat3(camera.getViewMatrix()));
  shaderManager.updateSkyShader(skyProjectionView);
  renderer.drawSkybox(&skybox);

  //font rendering
  if (options.get(OPT_DRAW_DEBUG_TEXT))
    {
      glEnable(GL_BLEND);
      glDisable(GL_CULL_FACE);
      {
        BENCHMARK("Renderer: add and draw text", true);
        fontManager->resetBufferOffset();
        float scrHeight = (float)screenResolution.getHeight();
        fontManager->addText("CPU UPS: " + std::to_string(CPU_timer.getFPS()), 10.0f, scrHeight - 15.0f, 0.18f);
        fontManager->addText("Camera pos: " + std::to_string(viewPosition.x).substr(0,6) + ": "
                               + std::to_string(viewPosition.y).substr(0,6) + ": "
                               + std::to_string(viewPosition.z).substr(0,6), 10.0f, scrHeight - 35.0f, 0.18f);
        fontManager->addText("Camera on map: " + std::to_string(camera.getMapCoordX()) + ": " + std::to_string(camera.getMapCoordZ()),
                               10.0f, scrHeight - 55.0f, 0.18f);
        fontManager->addText("View dir: " + std::to_string(camera.getDirection().x).substr(0,6) + ": "
                               + std::to_string(camera.getDirection().y).substr(0,6) + ": "
                               + std::to_string(camera.getDirection().z).substr(0,6), 10.0f, scrHeight - 75.0f, 0.18f);
        fontManager->addText("Cursor at: " + (!options.get(OPT_SHOW_CURSOR) ? "inactive" : (std::to_string(cursorToViewportDirection.x).substr(0,6) + ": "
                               + std::to_string(cursorToViewportDirection.y).substr(0,6) + ": "
                               + std::to_string(cursorToViewportDirection.z).substr(0,6))), 10.0f, scrHeight - 95.0f, 0.18f);
        fontManager->addText("Cursor on map: " + (!options.get(OPT_SHOW_CURSOR) ? "inactive" : (std::to_string(mouseInput.getCursorMapX()) + ": "
                               + std::to_string(mouseInput.getCursorMapZ()-1) + ", " + mouseInput.getCursorTileName())),
                               10.0f, scrHeight - 115.0f, 0.18f);
        fontManager->addText("Water culling: " + (options.get(OPT_WATER_CULLING) ? std::string("On") : std::string("Off")), 10.0f, 20.0f, 0.18f);
        fontManager->addText("Hills culling: " + (options.get(OPT_HILLS_CULLING) ? std::string("On") : std::string("Off")), 10.0f, 40.0f, 0.18f);
        fontManager->addText("Trees culling: " + (options.get(OPT_MODELS_CULLING) ? std::string("On") : std::string("Off")), 10.0f, 60.0f, 0.18f);
#ifdef _DEBUG
        fontManager->addText("Water anim thread works: " + (waterThreadAnimationIsWorking ? std::string("On") : std::string("Off")), 10.0f, 80.0f, 0.18f);
        glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &ram_available);
        fontManager->addText("RAM available: " + (std::to_string(ram_available)
                                                     .append(", ")
                                                     .append(std::to_string(ram_available / ram_size_float_percentage))
                                                     .append("%")), 10.0f, 100.0f, 0.18f);
        fontManager->addText("Models Phong: " + (options.get(OPT_MODELS_PHONG_SHADING) ? std::string("On") : std::string("Off")), 10.0f, 120.0f, 0.18f);
#endif
        fontManager->drawText();
      }
      glDisable(GL_BLEND);
      glEnable(GL_CULL_FACE);
      {
        BENCHMARK("Renderer: draw cs", true);
        csRenderer.draw(glm::mat3(camera.getViewMatrix()), screenResolution.getAspectRatio());
      }
    }

  if (options.get(OPT_POLYGON_LINE))
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Game::drawFrameObjectsDepthmap()
{
  glDisable(GL_CULL_FACE); //or set front face culling

  shaderManager.get(SHADER_SHADOW_TERRAIN).use();
  renderer.drawHillsDepthmap(hillMapGenerator);
  renderer.drawShore(baseMapGenerator);

  if (options.get(OPT_DRAW_TREES))
    {
      shaderManager.get(SHADER_SHADOW_MODELS).use();
      {
        BENCHMARK("Renderer: draw models depthmap", true);
        renderer.drawPlants(plantGenerator, shaderManager.get(SHADER_SHADOW_MODELS),
                           options.get(OPT_MODELS_CULLING),
                           false,
                           updateCount % MESH_INDIRECT_BUFFER_UPDATE_FREQ == 0,
                           false,
                           false);
      }
    }

  glEnable(GL_CULL_FACE); //or set back face culling

  //reset texture units to terrain textures after we done with models
  glBindTextureUnit(TEX_FLAT, textureManager->get(TEX_FLAT));
  glBindTextureUnit(TEX_FLAT_2, textureManager->get(TEX_FLAT_2));
}

void Game::loop()
{
  //even if we don't need to render models make sure we update indirect buffer data for meshes
  {
    BENCHMARK("Game: wait for mesh indirect ready", true);
    while(!meshesIndirectDataReady && !updateCount == 0 && meshesIndirectDataNeed) {}
  }
  meshesIndirectDataReady = false;

  keyboard.processKeyboard();
  keyboard.processKeyboardCamera(CPU_timer.tick(), hillMapGenerator->getMap());

  //recreate routine
  if (options.get(OPT_RECREATE_TERRAIN_REQUEST))
    {
      while(!waterThreadHasUpdated)
        {
          std::this_thread::yield();//busy wait until water thread has done its business...and business is good
        }
      waterThreadUpdatePermitted = false; //explicitly bypass water animation frame update routine
      delete baseMapGenerator;
      delete buildableMapGenerator;
      baseMapGenerator = new BaseMapGenerator(waterMapGenerator->getMap(), hillMapGenerator->getMap());
      buildableMapGenerator = new BuildableMapGenerator(baseMapGenerator->getMap(), hillMapGenerator->getMap());
      waterMapGenerator->initializeMap(waterMapGenerator->getMap());
      hillMapGenerator->initializeMap(hillMapGenerator->getMap());

      prepareTerrain();
      delete saveLoadManager;
      saveLoadManager = new SaveLoadManager(*baseMapGenerator, *hillMapGenerator, *waterMapGenerator, buildableMapGenerator, camera);
      saveLoadManager->setTreeGenerator(*plantGenerator);
      options.set(OPT_RECREATE_TERRAIN_REQUEST, false);
      textureManager->createUnderwaterReliefTexture(waterMapGenerator);
      waterThreadUpdatePermitted = true; //it's okay now to begin animating water
    }

  if ((options.get(OPT_CREATE_SHADOW_MAP_REQUEST) || updateCount % 16 == 0) && options.get(OPT_USE_SHADOWS))
    {
      glViewport(0, 0, DEPTH_MAP_TEXTURE_WIDTH, DEPTH_MAP_TEXTURE_HEIGHT);
      glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
      glClear(GL_DEPTH_BUFFER_BIT);
      drawFrameObjectsDepthmap();
      glViewport(0, 0, screenResolution.getWidth(), screenResolution.getHeight());
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      options.set(OPT_CREATE_SHADOW_MAP_REQUEST, false);
    }

  //update view and projection matrices
  glm::mat4 view = camera.getViewMatrix();
  glm::mat4 projectionView = projection * view;
  viewFrustum.updateFrustum(projectionView);

  /*
   * If the MULTISAMPLE_ENABLE option is true we only need to rebind fbo to use one with MS
   * because the msFBO is already configured to use multisample generated texture
   * with a MULTISAMPLES number of sampling (check Settings.h). If the MULTISAMPLE_ENABLE set to false
   * we just bind another one fbo without MS (that is also used as read buffer and then used for buffer blitting
   * to the default fbo IF the ms enabled), moreover, we do not need to do blitting,
   * because the fbo itself already contains all the data drawn into it
   * and it could be used by default fbo immediately
   */
  bool multisamplingEnabled = options.get(OPT_USE_MULTISAMPLING);
  glBindFramebuffer(GL_FRAMEBUFFER, multisamplingEnabled ? multisampleFBO : screenFBO);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //render our world onto separate FBO as usual
  drawFrameObjects(projectionView);

  //after all mesh related draw calls we could start updating meshes indirect data buffers
  //start updating right after we've used it and before we need that data to be updated and buffered again
  meshesIndirectDataNeed = updateCount % MESH_INDIRECT_BUFFER_UPDATE_FREQ == 1;

  //render result onto the default FBO and apply HDR/MS if the flag are set
  {
    BENCHMARK("Game: draw frame to screen", true);
    drawFrameToScreenRectangle(multisamplingEnabled);
  }

  //save/load routine
  if (options.get(OPT_SAVE_REQUEST))
    {
      saveLoadManager->saveToFile(SAVES_DIR + "testSave.txt");
      options.set(OPT_SAVE_REQUEST, false);
    }
  if (options.get(OPT_LOAD_REQUEST))
    {
      while(!waterThreadHasUpdated)
        {
          std::this_thread::yield(); //busy wait
        }
      waterThreadUpdatePermitted = false;
      saveLoadManager->loadFromFile(SAVES_DIR + "testSave.txt");
      options.set(OPT_LOAD_REQUEST, false);
      textureManager->createUnderwaterReliefTexture(waterMapGenerator);
      waterThreadUpdatePermitted = true;
    }

  {
    BENCHMARK("Game: glfwSwapBuffers", true);
    glfwSwapBuffers(window);
  }
  ++updateCount;
}
