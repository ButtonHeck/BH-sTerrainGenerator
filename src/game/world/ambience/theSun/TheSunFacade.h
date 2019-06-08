/*
 * Copyright 2019 Ilya Malgin
 * TheSunFacade.h
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
 * Purpose: contains declaration for TheSunFacade class
 * @version 0.1.0
 */

#ifndef THESUNFACADE_H
#define THESUNFACADE_H
#include "TheSun"
#include "TheSunShader"
#include "TheSunRenderer"
#include "ScreenResolution"

/**
 * @brief Represents facade to the Sun related code module. Contains the Sun entity itself, personal shader and renderer,
 * and pair of numbers of maximum possible samples passed occlusion test.
 * Responsible mostly for delegating tasks among its member objects and keeping track of the flare visibility of the Sun
 */
class TheSunFacade
{
public:
  TheSunFacade(Shader& renderShader, const ScreenResolution& screenResolution);
  void move(float angleDegrees);
  void moveAbsolutePosition(float angleDegrees);
  void draw(const glm::mat4 &skyProjectionView, bool doOcclusionTest, bool useReflectionPointSize);
  const glm::vec3 &getPosition() const;
  const glm::vec3 &getLightDir() const;
  const glm::mat4 &getRotationTransform() const;
  GLfloat getSunFlareBrightness(bool multisampled, float viewPositionY) const;

private:
  TheSun theSun;
  TheSunShader shader;
  TheSunRenderer renderer;
  GLfloat maxSamplesPassedMultisampling, maxSamplesPassed;
};

#endif // THESUNFACADE_H
