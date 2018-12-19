#include "game/world/plants/PlantsFacade.h"

PlantsFacade::PlantsFacade(Shader &renderPhongShader, Shader &renderGouraudShader)
  :
    shaders(renderPhongShader, renderGouraudShader),
    landPlantsGenerator(),
    grassGenerator(),
    hillTreesGenerator(),
    treesRenderer(),
    grassRenderer()
{}

void PlantsFacade::setup(const map2D_f &baseMap, const map2D_f &hillMap)
{
  prepareDistributionMap(MODELS_DISTRIBUTION_FREQ);
  landPlantsGenerator.setup(baseMap, hillMap, distributionMap);
  grassGenerator.setup(baseMap, hillMap, distributionMap);
  hillTreesGenerator.setup(hillMap, distributionMap);
}

void PlantsFacade::prepareMeshesIndirectData(const glm::vec2 &cameraPositionXZ, const Frustum &viewFrustum)
{
  prepareMeshesIndirectData(landPlantsGenerator, cameraPositionXZ, viewFrustum);
  prepareMeshesIndirectData(hillTreesGenerator, cameraPositionXZ, viewFrustum);
  prepareMeshesIndirectData(grassGenerator, cameraPositionXZ, viewFrustum);
}

void PlantsFacade::updateIndirectBufferData()
{
  BENCHMARK("PlantsFacade: updateIndirectBuffer", true);
  updateIndirectBufferData(landPlantsGenerator);
  updateIndirectBufferData(hillTreesGenerator);
  updateIndirectBufferData(grassGenerator);
}

void PlantsFacade::draw(glm::vec3 &lightDir,
                        const std::array<glm::mat4, NUM_SHADOW_LAYERS> &lightSpaceMatrices,
                        glm::mat4 &projectionView,
                        glm::vec3 &viewPosition,
                        bool usePhongShading,
                        bool useShadows,
                        bool useLandBlending)
{
  shaders.switchToGrass(usePhongShading, false);
  shaders.updateAllPlants(lightDir,
                          lightSpaceMatrices,
                          projectionView,
                          viewPosition,
                          usePhongShading,
                          useShadows,
                          useLandBlending);
  treesRenderer.render(landPlantsGenerator.models,
                       hillTreesGenerator.models,
                       useLandBlending,
                       false);
  treesRenderer.render(landPlantsGenerator.lowPolyModels,
                       hillTreesGenerator.lowPolyModels,
                       false,
                       false);

  shaders.switchToGrass(usePhongShading, true);
  shaders.updateGrass(usePhongShading);
  grassRenderer.render(grassGenerator.models,
                       useLandBlending,
                       false);
  grassRenderer.render(grassGenerator.lowPolyModels,
                       false,
                       false);
}

void PlantsFacade::drawDepthmap()
{
  treesRenderer.render(landPlantsGenerator.models,
                       hillTreesGenerator.models,
                       false,
                       true);
  grassRenderer.render(grassGenerator.models,
                       false,
                       true);
}

void PlantsFacade::serialize(std::ofstream &output)
{
  landPlantsGenerator.serialize(output);
  grassGenerator.serialize(output);
  hillTreesGenerator.serialize(output);
}

void PlantsFacade::deserialize(std::ifstream &input)
{
  landPlantsGenerator.deserialize(input);
  grassGenerator.deserialize(input);
  hillTreesGenerator.deserialize(input);
}

void PlantsFacade::prepareDistributionMap(int cycles)
{
  Generator::initializeMap(distributionMap);
  for (int cycle = 1; cycle <= cycles; cycle++)
    {
      for (unsigned int y = 0; y < distributionMap.size(); y++)
        {
          for (unsigned int x = 0; x < distributionMap[0].size(); x++)
            {
              if (rand() % (cycles * 5) == 0)
                {
                  unsigned int yBorder = y + cycle - 1;
                  if (yBorder >= distributionMap.size())
                    yBorder = distributionMap.size() - 1;
                  unsigned int xBorder = x + cycle - 1;
                  if (xBorder >= distributionMap[0].size())
                    xBorder = distributionMap.size() - 1;
                  for (unsigned int y2 = y; y2 <= yBorder; y2++)
                    {
                      for (unsigned int x2 = x; x2 <= xBorder; x2++)
                        {
                          if (distributionMap[y2][x2] < cycles)
                            distributionMap[y2][x2]++;
                        }
                    }
                }
            }
        }
    }
}

void PlantsFacade::prepareMeshesIndirectData(PlantGenerator &generator, const glm::vec2 &cameraPositionXZ, const Frustum &viewFrustum)
{
  auto& models = generator.models;
  auto& lowPolyModels = generator.lowPolyModels;
  auto& chunks = generator.chunks;
  for (unsigned int i = 0; i < models.size(); i++)
    {
      Model& model = models[i];
      model.prepareMeshesIndirectData(chunks, i, cameraPositionXZ, viewFrustum,
                                      LOADING_DISTANCE_UNITS_SHADOW_SQUARE,
                                      LOADING_DISTANCE_UNITS_SHADOW_SQUARE,
                                      LOADING_DISTANCE_UNITS_SQUARE);
      Model& lowPolyModel = lowPolyModels[i];
      lowPolyModel.prepareMeshesIndirectData(chunks, i, cameraPositionXZ, viewFrustum,
                                             LOADING_DISTANCE_UNITS_SHADOW_SQUARE,
                                             LOADING_DISTANCE_UNITS_SHADOW_SQUARE,
                                             LOADING_DISTANCE_UNITS_SQUARE);
    }
}

void PlantsFacade::updateIndirectBufferData(PlantGenerator &generator)
{
  auto& models = generator.models;
  auto& lowPolyModels = generator.lowPolyModels;
  for (unsigned int i = 0; i < models.size(); i++)
    {
      Model& model = models[i];
      model.updateIndirectBufferData();
      Model& lowPolyModel = lowPolyModels[i];
      lowPolyModel.updateIndirectBufferData();
    }
}
