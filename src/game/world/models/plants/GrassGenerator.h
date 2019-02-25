/*
 * Copyright 2019 Ilya Malgin
 * GrassGenerator.h
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
 * Purpose: contains declaration for GrassGenerator class
 * @version 0.1.0
 */

#ifndef GRASSGENERATOR_H
#define GRASSGENERATOR_H
#include "game/world/models/plants/PlantGenerator.h"

/**
 * @brief Generator for grass. Responsible for accomodating all grass models on the world map
 */
class GrassGenerator : public PlantGenerator
{
public:
  GrassGenerator();
  void setup(const map2D_f &landMap, const map2D_f &hillMap, const map2D_i &distributionMap);

private:
  const float MIN_SCALE = 0.189f;
  const float MAX_SCALE = 0.224f;

  void setupMatrices(const map2D_f &landMap, const map2D_f &hillMap, const map2D_i &distributionMap);
};

#endif // GRASSGENERATOR_H
