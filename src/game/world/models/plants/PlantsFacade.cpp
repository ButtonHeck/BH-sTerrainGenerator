/*
 * Copyright 2019 Ilya Malgin
 * PlantsFacade.cpp
 * This file is part of The Sugarpunky Chocolate Disaster project
 *
 * The Sugarpunky Chocolate Disaster project is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The Sugarpunky Chocolate Disaster project is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * See <http://www.gnu.org/licenses/>
 *
 * Purpose: contains definitions for PlantsFacade class
 * @version 0.1.0
 */

#include "game/world/models/plants/PlantsFacade.h"

/**
 * @param renderPhongShader compiled Phong shader program provided to a personal shader manager
 * @param renderGouraudShader compiled Gouraud shader program provided to a personal shader manager
 */
PlantsFacade::PlantsFacade(Shader &renderPhongShader, Shader &renderGouraudShader)
  :
    shaders(renderPhongShader, renderGouraudShader)
{}

/**
 * @brief initializes distribution map and delegates initialization command for generators
 * @param landMap map of the land
 * @param hillMap map of the hills
 * @param hillsNormalMap map of the hills normals
 */
void PlantsFacade::setup(const map2D_f &landMap, const map2D_f &hillMap, const map2D_vec3 &hillsNormalMap)
{
  prepareDistributionMap();
  landPlantsGenerator.setup(landMap, hillMap, distributionMap);
  grassGenerator.setup(landMap, hillMap, distributionMap);
  hillTreesGenerator.setup(hillMap, distributionMap, hillsNormalMap);
}

/**
 * @brief delegates chunk update call to the hill generator (necessary when loading terrain from file)
 * @param hillMap new map of the hills
 */
void PlantsFacade::updateHillModelChunks(const map2D_f &hillMap)
{
  hillTreesGenerator.updateModelChunks(hillMap);
}

/**
 * @brief for each generator's models delegates command to prepare its indirect buffer data on CPU side
 * @param cameraPositionXZ X and Z coordinates of a current view position
 * @param viewFrustum frustum to perform CPU culling
 */
void PlantsFacade::prepareIndirectBufferData(const glm::vec2 &cameraPositionXZ, const Frustum &viewFrustum)
{
  landPlantsGenerator.prepareIndirectBufferData(cameraPositionXZ, viewFrustum);
  hillTreesGenerator.prepareIndirectBufferData(cameraPositionXZ, viewFrustum);
  grassGenerator.prepareIndirectBufferData(cameraPositionXZ, viewFrustum);
}

/**
 * @brief for each generator's models delegates command to upload indirect buffer data from CPU to GPU
 */
void PlantsFacade::updateIndirectBufferData()
{
  BENCHMARK("PlantsFacade: updateIndirectBuffer", true);
  landPlantsGenerator.updateIndirectBufferData();
  hillTreesGenerator.updateIndirectBufferData();
  grassGenerator.updateIndirectBufferData();
}

/**
 * @brief updates the shader program state, switches GL_BLEND mode if necessary and delegates draw calls to renderers
 * @param lightDir direction of the sunlight (directional lighting)
 * @param lightSpaceMatrices matrices for shadow sampling
 * @param projectionView Projection*View matrix
 * @param viewPosition current position of the camera
 * @param usePhongShading define what shading model to use
 * @param useShadows define whether to use shadows
 * @param useLandBlending define whether to use blending
 */
void PlantsFacade::draw(const glm::vec3 &lightDir,
                        const std::array<glm::mat4, NUM_SHADOW_LAYERS> &lightSpaceMatrices,
                        const glm::mat4 &projectionView,
                        const glm::vec3 &viewPosition,
                        bool usePhongShading,
                        bool useShadows,
                        bool useLandBlending)
{
  if (useLandBlending)
    glEnable(GL_BLEND);
  else
    glDisable(GL_BLEND);

  shaders.updateAllPlants(usePhongShading,
                          lightDir,
                          lightSpaceMatrices,
                          projectionView,
                          viewPosition,
                          useShadows,
                          useLandBlending);

  //draw trees and hill models first (plain and low-poly)
  shaders.setType(PLANT_STATIC);
  shaders.setLowPolyMode(false);
  treesRenderer.render(landPlantsGenerator.getModels(false), hillTreesGenerator.getModels(false), false);
  shaders.setLowPolyMode(true);
  treesRenderer.render(landPlantsGenerator.getModels(true), hillTreesGenerator.getModels(true), false);

  //draw grass (plain and low-poly)
  shaders.setType(PLANT_ANIMATED);
  shaders.updateGrassKeyframe();
  shaders.setLowPolyMode(false);
  grassRenderer.render(grassGenerator.getModels(false), false);
  shaders.setLowPolyMode(true);
  grassRenderer.render(grassGenerator.getModels(true), false);

  if (useLandBlending)
    glDisable(GL_BLEND);
}

/**
 * @brief performs shadow mode rendering process
 * @param grassCastShadow define whether grass would be drawn in shadow maps
 */
void PlantsFacade::drawDepthmap(bool grassCastShadow)
{
  treesRenderer.render(landPlantsGenerator.getModels(false), hillTreesGenerator.getModels(false), true);
  treesRenderer.render(landPlantsGenerator.getModels(true), hillTreesGenerator.getModels(true), true);
  if (grassCastShadow)
    grassRenderer.render(grassGenerator.getModels(false), true);
}

/**
 * @brief delegates serialization command to generators
 * @param output file stream to write data to
 */
void PlantsFacade::serialize(std::ofstream &output)
{
  landPlantsGenerator.serialize(output);
  grassGenerator.serialize(output);
  hillTreesGenerator.serialize(output);
}

/**
 * @brief delegates deserialization command to generators
 * @param input file stream to read data from
 */
void PlantsFacade::deserialize(std::ifstream &input)
{
  landPlantsGenerator.deserialize(input);
  grassGenerator.deserialize(input);
  hillTreesGenerator.deserialize(input);
}

/**
 * @brief prepares distribution map used by generators during plants allocation
 */
void PlantsFacade::prepareDistributionMap()
{
  Generator::initializeMap(distributionMap);

  //for each subsequent cycle distribution kernel would be one unit wider in radius
  for (int cycle = 1; cycle <= PLANTS_DISTRIBUTION_FREQUENCY; cycle++)
    {
      for (unsigned int startY = 0; startY < distributionMap.size(); startY++)
        {
          for (unsigned int startX = 0; startX < distributionMap[0].size(); startX++)
            {
              if (rand() % (PLANTS_DISTRIBUTION_FREQUENCY * 5) == 0) //check for randomizer "hit"
                {
                  unsigned int yBorder = startY + cycle - 1;
                  //calculate borders for kernel
                  if (yBorder >= distributionMap.size())
                    yBorder = distributionMap.size() - 1;
                  unsigned int xBorder = startX + cycle - 1;
                  if (xBorder >= distributionMap[0].size())
                    xBorder = distributionMap.size() - 1;

                  //fatten kernel if necessary
                  for (unsigned int y = startY; y <= yBorder; y++)
                    {
                      for (unsigned int x = startX; x <= xBorder; x++)
                        {
                          if (distributionMap[y][x] < PLANTS_DISTRIBUTION_FREQUENCY)
                            distributionMap[y][x]++;
                        }
                    }
                }
            }
        }
    }
}
