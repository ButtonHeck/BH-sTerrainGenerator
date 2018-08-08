#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H
#include <IL/il.h>
#include <GL/glew.h>
#include <string>
#include <vector>
#include <glm/common.hpp>
#include "game/Settings.h"
#include "generator/WaterMapGenerator.h"

class TextureLoader
{
public:
  TextureLoader();
  GLuint createAndBindTextureObject(GLenum target, GLuint textureUnit);
  GLuint loadTexture(const std::string& path, GLuint textureUnit, GLenum wrapType, GLint magFilter, GLint minFilter, bool includeCWD = true);
  GLuint createFrameMSTexture(int width, int height, int multisample, GLuint textureUnit);
  GLuint createFrameTexture(int width, int height, GLuint textureUnit);
  GLuint createDepthMapTexture(int width, int height, GLuint textureUnit);
  GLuint loadCubemap(const std::string& directory, GLuint textureUnit);
  GLuint createUnderwaterReliefTexture(WaterMapGenerator* waterMapGenerator, GLuint textureUnit, GLint magFilter, GLint minFilter);
  unsigned int getMaxMip(unsigned int width, unsigned int height);
};

#endif // TEXTURELOADER_H
