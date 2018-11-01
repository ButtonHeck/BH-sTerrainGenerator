#ifndef FRUSTUM_H
#define FRUSTUM_H
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "util/BenchmarkTimer.h"

enum FRUSTUM_SIDE
{
  FRUSTUM_RIGHT = 0,
  FRUSTUM_LEFT = 1,
  FRUSTUM_BOTTOM = 2,
  FRUSTUM_TOP = 3,
  FRUSTUM_BACK = 4
};

class Frustum
{
public:
  Frustum();
  void updateFrustum(glm::mat4& projectionView);
  bool isInsideXZ(float x, float z, float radius) const;
  bool isInside(float x, float y, float z, float radius) const;
  glm::vec4 getPlane(FRUSTUM_SIDE side);
private:
  void normalizePlane(FRUSTUM_SIDE side);
  std::vector<glm::vec4> frustumPlanes;
};

#endif // FRUSTUM_H
