#ifndef GAME_H
#define GAME_H
#include <thread>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include "game/Settings.h"
#include "game/TextureUnits.h"
#include "game/Options.h"
#include "game/ScreenResolution.h"
#include "input/KeyboardManager.h"
#include "input/MouseInputManager.h"
#include "game/SaveLoadManager.h"
#include "graphics/Camera.h"
#include "graphics/Renderer.h"
#include "graphics/Frustum.h"
#include "graphics/ShaderManager.h"
#include "graphics/FontManager.h"
#include "graphics/CoordinateSystemRenderer.h"
#include "graphics/Skybox.h"
#include "graphics/TextureManager.h"
#include "generator/UnderwaterQuadMapGenerator.h"
#include "model/Model.h"
#include "timer/Timer.h"
#include "timer/BenchmarkTimer.h"

class Game
{
public:
  Game(GLFWwindow* window, glm::vec3& cursorDir, Camera& camera, Options& options, ScreenResolution& screenResolution);
  ~Game();
  void setupVariables();
  void prepareTerrain();
  void prepareMS_FBO();
  void prepareDepthMapFBO(GLuint* fbo, GLuint depthTextureUnit);
  void prepareScreenVAO();
  void drawFrameToScreenRectangle(bool enableMS);
  void drawFrameObjects(glm::mat4& projectionView);
  void drawFrameObjectsDepthmap();
  void loop();
private:
  unsigned long updateCount = 0;
  ScreenResolution& screenResolution;
  GLFWwindow* window;
  glm::vec3& cursorToViewportDirection;
  Timer CPU_timer;
  Camera& camera;
  Renderer renderer = Renderer(camera);
  Frustum viewFrustum;
  Options& options;
  KeyboardManager keyboard = KeyboardManager(window, camera, options);
  MouseInputManager mouseInput;
  TextureLoader textureLoader = TextureLoader(screenResolution);
  ShaderManager shaderManager;
  WaterMapGenerator* waterMapGenerator = new WaterMapGenerator(shaderManager.get(SHADER_WATER_FC));
  HillsMapGenerator* hillMapGenerator = new HillsMapGenerator(shaderManager.get(SHADER_HILLS_FC), waterMapGenerator->getMap());
  BaseMapGenerator* baseMapGenerator = new BaseMapGenerator(waterMapGenerator->getMap(), hillMapGenerator->getMap());
  BuildableMapGenerator* buildableMapGenerator = new BuildableMapGenerator(baseMapGenerator->getMap(), hillMapGenerator->getMap());
  SaveLoadManager* saveLoadManager = new SaveLoadManager(*baseMapGenerator, *hillMapGenerator, *waterMapGenerator, buildableMapGenerator, camera);
  TreeGenerator* treeGenerator;
  FontManager* fontManager;
  CoordinateSystemRenderer csRenderer = CoordinateSystemRenderer(&shaderManager.get(SHADER_CS));
  UnderwaterQuadMapGenerator underwaterQuadGenerator;
  Skybox skybox;
  TextureManager* textureManager;
  glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()), screenResolution.getAspectRatio(), NEAR_PLANE, FAR_PLANE);
  GLuint screenVAO, screenVBO, multisampleFBO, screenFBO, depthMapFBO;
  std::thread* waterAnimationThread;
  std::thread* meshIndirectUpdateThread;
  volatile bool meshesIndirectDataReady = false, meshesIndirectDataNeed = false;
  volatile bool waterThreadHasUpdated = false, waterThreadUpdatePermitted = true;
#ifdef _DEBUG
  bool waterThreadAnimationIsWorking = true;
#endif
};

#endif // GAME_H
