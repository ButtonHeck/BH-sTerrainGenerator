#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H
#include <GL/glew.h>
#include <IL/il.h>
#include <glm/gtc/matrix_transform.hpp>
#include "graphics/ScreenResolution.h"
#include "util/DirectoriesSettings.h"
#include "graphics/GraphicsSettings.h"
#include "game/world/SceneSettings.h"
#include "util/Logger.h"
#include "util/typeAliases.h"

class TextureLoader
{
public:
  TextureLoader(const ScreenResolution &screenResolution);
  GLuint loadTexture(const std::string& path,
                     GLuint textureUnit,
                     GLenum wrapType,
                     GLint magFilter,
                     GLint minFilter,
                     bool useAnisotropy,
                     bool includeCWD = true,
                     bool isBindless = false,
                     bool explicitNoSRGB = false);
  GLuint createFrameMSTexture(GLuint textureUnit, int multisamples);
  GLuint createFrameTexture(GLuint textureUnit, bool isDepthTexture, bool useAnisotropy);
  GLuint createFrameTextureSized(GLuint textureUnit, bool isDepthTexture, int width, int height, bool useAnisotropy);
  GLuint createDepthMapTexture(GLuint textureUnit, int width, int height);
  GLuint loadCubemap(const std::string& directory, GLuint textureUnit, bool explicitNoSRGB);
  GLuint createUnderwaterReliefTexture(GLuint textureUnit, const map2D_f& waterMap, GLint magFilter, GLint minFilter);

private:
  GLuint createTextureObject(GLenum target, GLuint textureUnit, bool isBindless);
  void setTexture2DParameters(GLuint texture, GLint magFilter, GLint minFilter, GLenum wrapType);
  void setTex2DArrayParameters(GLint magFilter, GLint minFilter, GLenum wrapType);

  const ScreenResolution& screenResolution;
};

#endif // TEXTURELOADER_H
