/*
 * Copyright 2019 Ilya Malgin
 * PlantGenerator.h
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
 * Purpose: contains declaration for PlantGenerator class
 * @version 0.1.0
 */

#ifndef PLANTGENERATOR_H
#define PLANTGENERATOR_H
#include <vector>
#include <fstream>
#include <memory>
#include <random>
#include <glm/gtx/norm.hpp>
#include "game/world/models/Model.h"
#include "game/world/models/ModelChunk.h"

/**
 * @brief Boilerplate generator for all the plants.
 * Responsible for defining distances of models' LOD (global), storing models, managing their chunks and their instance matrices,
 * including (de)serialization and indirect buffer updates
 */
class PlantGenerator
{
public:
  PlantGenerator();
  void serialize(std::ofstream& output);
  void deserialize(std::ifstream& input);
  void initializeModelRenderChunks(const map2D_f &map);
  void prepareIndirectBufferData(const glm::vec2 &cameraPositionXZ, const Frustum &viewFrustum);
  void updateIndirectBufferData();
  std::vector<Model>& getModels(bool isLowPoly);
  std::vector<ModelChunk>& getChunks();

protected:
  void initializeModelChunks(const map2D_f &map);
  void loadMatrices(const map2D_mat4& newMatrices);
  map2D_mat4 substituteMatricesStorage();

  std::vector<Model> models;
  std::vector<Model> lowPolyModels;
  map2D_mat4 matrices;
  std::unique_ptr<unsigned int[]> numPlants;
  std::vector<ModelChunk> chunks;
  decltype(chunks) renderChunks;
  std::default_random_engine randomizer;

private:
  /**
   * @brief LOADING_DISTANCE_CHUNKS define distance of how far would plain models be seen
   */
  constexpr static unsigned int LOADING_DISTANCE_CHUNKS = 8;
  constexpr static unsigned int LOADING_DISTANCE_UNITS = CHUNK_SIZE * LOADING_DISTANCE_CHUNKS;
  constexpr static unsigned int LOADING_DISTANCE_UNITS_SQUARE = LOADING_DISTANCE_UNITS * LOADING_DISTANCE_UNITS;
  /**
   * @brief LOADING_DISTANCE_CHUNKS_LOWPOLY define distance of how far would low-poly models be seen
   */
  constexpr static unsigned int LOADING_DISTANCE_CHUNKS_LOWPOLY = 32;
  constexpr static unsigned int LOADING_DISTANCE_UNITS_LOWPOLY = CHUNK_SIZE * LOADING_DISTANCE_CHUNKS_LOWPOLY;
  constexpr static unsigned int LOADING_DISTANCE_UNITS_LOWPOLY_SQUARE = LOADING_DISTANCE_UNITS_LOWPOLY * LOADING_DISTANCE_UNITS_LOWPOLY;
  /**
   * @brief LOADING_DISTANCE_CHUNKS_SHADOW define distance of how far would models be casting shadows
   */
  constexpr static unsigned int LOADING_DISTANCE_CHUNKS_SHADOW = 16;
  constexpr static unsigned int LOADING_DISTANCE_UNITS_SHADOW = CHUNK_SIZE * LOADING_DISTANCE_CHUNKS_SHADOW;
  constexpr static unsigned int LOADING_DISTANCE_UNITS_SHADOW_SQUARE = LOADING_DISTANCE_UNITS_SHADOW * LOADING_DISTANCE_UNITS_SHADOW;
};

#endif // PLANTGENERATOR_H
