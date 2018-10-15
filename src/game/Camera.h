#ifndef CAMERA_H
#define CAMERA_H
#include <vector>
#include <cmath>
#include <fstream>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/detail/func_geometric.hpp>
#include <glm/detail/func_trigonometric.hpp>
#include <glm/detail/func_common.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "util/Settings.h"

enum MOVE_DIRECTION
{
  FORWARD,
  BACKWARD,
  RIGHT,
  LEFT,
  UP,
  DOWN
};

class Camera
{
public:

  Camera(glm::vec3 position);
  glm::mat4 getViewMatrix() const;
  void processMouseCursor(float xOffset, float yOffset);
  void processMouseScroll(float yOffset);
  void processKeyboardInput(float delta, MOVE_DIRECTION dir, std::vector<std::vector<float>>& hillsMap);
  void switchFPSmode();
  bool getFPSmode() const;
  float getZoom() const;
  glm::vec3 getPosition() const;
  glm::vec3 getDirection() const;
  glm::vec3 getRight() const;
  glm::vec3 getUp() const;
  void updateVectors();
  int getMapCoordX() const;
  int getMapCoordZ() const;
  void serialize(std::ofstream& output);
  void deserialize(std::ifstream& input);
private:
  //options
  float zoom, moveSpeed, mouseSensitivity;
  bool FPSmode;
  const float CAMERA_WORLD_BORDER_OFFSET = 8.0f;
  const float CAMERA_WORLD_MIN_HEIGHT = 2.0f;
  const float CAMERA_WORLD_MIN_HEIGHT_ON_HILLS = CAMERA_WORLD_MIN_HEIGHT - HILLS_OFFSET_Y;
  //Euler angles
  float yaw, pitch;
  //direction vectors
  glm::vec3 position, front, right, up, worldUp;
  void moveCameraFrontAxial(bool forward, float velocity);
};

#endif // CAMERA_H
