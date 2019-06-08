/*
 * Copyright 2019 Ilya Malgin
 * LandPlantsGenerator.h
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
 * Purpose: contains declaration for LandPlantsGenerator class
 * @version 0.1.0
 */

#pragma once

#include "PlantGenerator"

/**
 * @brief Generator for plants located on the land.
 * Responsible for accomodating all land plants models on the world map
 */
class LandPlantsGenerator : public PlantGenerator
{
public:
  LandPlantsGenerator();
  void setup(const map2D_f &landMap, const map2D_f &hillMap, const map2D_i &distributionMap);

private:
  const float MIN_SCALE = 0.27f;
  const float MAX_SCALE = 0.32f;
  const float MIN_POSITION_OFFSET = -0.25f;
  const float MAX_POSITION_OFFSET = 0.25f;

  void setupMatrices(const map2D_f &landMap, const map2D_f &hillMap, const map2D_i &distributionMap);
};
