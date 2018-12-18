#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H
#include <string>
#include <memory>
#include <GL/glew.h>
#include <IL/il.h>
#include "graphics/ScreenResolution.h"
#include "util/Settings.h"
#include "util/Logger.h"
#include "util/typeAliases.h"

enum class TEXTURE_TYPE
{
  TEXTURE_TYPE_DIFFUSE,
  TEXTURE_TYPE_SPECULAR
};

class TextureLoader
{
public:
  TextureLoader(ScreenResolution& screenResolution);
  GLuint loadTexture(const std::string& path,
                     GLuint textureUnit,
                     GLenum wrapType,
                     GLint magFilter,
                     GLint minFilter,
                     bool useAnisotropy,
                     bool includeCWD = true);
  GLuint loadTextureBindless(const std::string& path,
                             GLenum wrapType,
                             GLint magFilter,
                             GLint minFilter,
                             bool useAnisotropy,
                             bool includeCWD = true);
  GLuint createFrameMSTexture(int multisample, GLuint textureUnit);
  GLuint createFrameTexture(GLuint textureUnit);
  GLuint createDepthMapTexture(int width, int height, GLuint textureUnit);
  GLuint loadCubemap(const std::string& directory, GLuint textureUnit);
  GLuint createUnderwaterReliefTexture(const map2D_f& waterMap, GLuint textureUnit, GLint magFilter, GLint minFilter);
  unsigned int getMaxMip(unsigned int width, unsigned int height);
private:
  GLuint createAndBindTextureObject(GLenum target, GLuint textureUnit);
  void setTex2DParameters(GLint magFilter, GLint minFilter, GLenum wrapType);
  void setTexture2DParameters(GLuint texture, GLint magFilter, GLint minFilter, GLenum wrapType);
  void setTex2DArrayParameters(GLint magFilter, GLint minFilter, GLenum wrapType);
  ScreenResolution& screenResolution;
};

#endif // TEXTURELOADER_H
