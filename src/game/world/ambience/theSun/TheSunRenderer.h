/*
 * Copyright 2019 Ilya Malgin
 * TheSunRenderer.h
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
 * Purpose: contains declaration for TheSunRenderer class
 * @version 0.1.0
 */

#pragma once

#include "Query"

class TheSun;

/**
 * @brief Represents personal renderer for the Sun related code module.
 * Responsible for setting up/switching OpenGL state during rendering, sending draw calls to the GPU
 * and managing occlusion query during rendering
 */
class TheSunRenderer
{
public:
	constexpr static float DEFAULT_SUN_POINT_SIZE = 64.0f;

	TheSunRenderer( TheSun & theSun ) noexcept;
	void render( bool doOcclusionTest, 
				 bool useReflectionPointSize );
	GLuint getSamplesPassedQueryResult() const noexcept;
	void setPointSize( float pointSize ) noexcept;
	void setReflectionPointSize( float pointSize ) noexcept;
	float getPointSize() const noexcept;

private:
	TheSun & theSun;
	Query samplesPassedQuery;
	float pointSize;
	float reflectionPointSize;
};
