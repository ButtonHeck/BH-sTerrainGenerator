#ifndef MOUSEINPUTMANAGER_H
#define MOUSEINPUTMANAGER_H
#include <glm/gtc/matrix_transform.hpp>
#include "../../src/graphics/Camera.h"
#include "../../src/game/Options.h"
#include "../../src/graphics/ScreenResolution.h"
#include "../../src/graphics/GraphicsSettings.h"

class MouseInputManager
{
public:
  static MouseInputManager& getInstance();
  static void setCallbacks(GLFWwindow* window);
  void updateCursorMappingCoordinates(const Camera& camera, const map2D_f &landMap, const map2D_f &hillMap, const map2D_f &buildableMap);
  int getCursorWorldX() const;
  int getCursorWorldZ() const;
  const glm::vec3& getCursorToNearPlaneWorldSpace() const;
  const std::string& getCursorTileName() const;

private:
  MouseInputManager() = default;
  static void cursorMoveCallback(GLFWwindow*, double x, double y);
  static void scrollCallback(GLFWwindow*, double, double y);
  static void cursorClickCallback(GLFWwindow*, int, int, int);

  glm::vec3 cursorToNearPlaneWorldSpace;
  float lastX, lastY;
  float cursorAbsX = 0.0f;
  float cursorAbsZ = 0.0f;
  int cursorWorldX = 0;
  int cursorWorldZ = 0;
  std::string cursorTileName = "Land";
};

#endif // MOUSEINPUTMANAGER_H
