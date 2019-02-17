/*
 * Copyright 2019 Ilya Malgin
 * LensFlareRenderer.h
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
 * Purpose: contains declaration for LensFlareRenderer class
 * @version 0.1.0
 */

#ifndef LENSFLARERENDERER_H
#define LENSFLARERENDERER_H
#include <GL/glew.h>
#include "graphics/openglObjects/BufferCollection.h"

/**
 * @brief Renderer for lens flare effect which is responsible for sending appropriate draw call to the GPU
 * and setting OpenGL state before and after draw call
 */
class LensFlareRenderer
{
public:
  LensFlareRenderer(BufferCollection& basicGLBuffer);
  void draw(unsigned int numFlares);

private:
  BufferCollection& basicGLBuffer;
};

#endif // LENSFLARERENDERER_H
