/*
 * Copyright 2019 Ilya Malgin
 * CoordinateSystemRenderer.h
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
 * Purpose: contains declaration for CoordinateSystemRenderer class
 * @version 0.1.0
 */

#pragma once

#include "BufferCollection"

#include <glm/mat3x3.hpp>

class Shader;

/**
* @brief utility class for rendering coordinate system (requires debug text output option enabled)
*/
class CoordinateSystemRenderer
{
public:
	CoordinateSystemRenderer( Shader * shader );
	virtual ~CoordinateSystemRenderer() = default;
	void draw( const glm::mat3 & view, 
			   float aspectRatio );

private:
	BufferCollection basicGLBuffers;
	Shader * shader;
};
