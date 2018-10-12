#ifndef MOUSEINPUTMANAGER_H
#define MOUSEINPUTMANAGER_H
#include <memory>
#include "graphics/Camera.h"
#include <GLFW/glfw3.h>
#include "game/Options.h"
#include "game/ScreenResolution.h"
#include "generator/LandGenerator.h"
#include "generator/HillsGenerator.h"
#include "generator/BuildableGenerator.h"

class MouseInputManager
{
public:
  static MouseInputManager& getInstance();
  static void setCallbacks(GLFWwindow* window);
  void updateCursorMappingCoordinates(Camera& camera,
                         const std::shared_ptr<LandGenerator> baseMapGenerator,
                         const std::shared_ptr<HillsGenerator> hillMapGenerator,
                         const std::shared_ptr<BuildableGenerator> buildableMapGenerator);
  int getCursorMapX() const;
  int getCursorMapZ() const;
  const glm::vec3& getCursorToViewportDirection() const;
  const std::string& getCursorTileName() const;
private:
  MouseInputManager() = default;
  static void cursorMoveCallback(GLFWwindow*, double x, double y);
  static void cursorClickCallback(GLFWwindow*, int, int, int);
  glm::vec3 cursorToViewportDirection;
  float lastX, lastY;
  float cursorAbsX = 0.0f;
  float cursorAbsZ = 0.0f;
  int cursorMapX = 0;
  int cursorMapZ = 0;
  std::string cursorTileName = "Flat";
};

#endif // MOUSEINPUTMANAGER_H
