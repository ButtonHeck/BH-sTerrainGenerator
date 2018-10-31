#ifndef PLANTSFACADE_H
#define PLANTSFACADE_H
#include <memory>
#include "util/typeAliases.h"
#include "game/world/plants/LandPlantsGenerator.h"
#include "game/world/plants/GrassGenerator.h"
#include "game/world/plants/HillTreesGenerator.h"
#include "game/world/terrain/Generator.h"
#include "game/world/plants/PlantsShader.h"
#include "game/world/plants/PlantsRenderer.h"

class Frustum;

class PlantsFacade
{
public:
  PlantsFacade(Shader& renderPhongShader, Shader& renderGouraudShader, Shader& shadowShader);
  virtual ~PlantsFacade() = default;
  void setup(map2D_f& baseMap, map2D_f& hillMap);
  void prepareMeshesIndirectData(const glm::vec2& cameraPositionXZ, const Frustum& viewFrustum);
  void updateIndirectBufferData();
  void draw(glm::mat4 &projectionView, glm::vec3 &viewPosition, bool usePhongShading, bool useShadowEmphasize, bool useShadows, bool useFlatBlending);
  void drawDepthmap();

  //save/load routine
  void serialize(std::ofstream& output);
  void deserialize(std::ifstream& input);

private:
  void prepareDistributionMap(int cycles);
  map2D_i distributionMap;
  PlantsShader shaders;
  std::unique_ptr<LandPlantsGenerator> landPlantsGenerator;
  std::unique_ptr<GrassGenerator> grassGenerator;
  std::unique_ptr<HillTreesGenerator> hillTreesGenerator;
  PlantsRenderer renderer;
};

#endif // PLANTSFACADE_H
