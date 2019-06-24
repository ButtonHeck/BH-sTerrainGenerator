/*
 * Copyright 2019 Ilya Malgin
 * WaterRenderer.h
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
 * Purpose: contains declaration for WaterRenderer class
 * @version 0.1.0
 */

#pragma once

#include "Query"

class WaterGenerator;
class WaterShader;

/**
* @brief renderer for water. 
* Responsible for delegating draw calls to OpenGL and managing query operations during rendering
*/
class WaterRenderer
{
public:
  WaterRenderer(WaterShader& shaders, WaterGenerator& generator) noexcept;
  void render(bool useFrustumCulling);
  void debugRender(GLenum primitiveType);
  bool anySamplesPassed() const noexcept;

private:
  WaterShader& shaders;
  WaterGenerator& generator;
  Query anySamplesPassedQuery;
};
