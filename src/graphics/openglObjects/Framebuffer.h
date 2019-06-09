#pragma once

#include <GL/glew.h>

class TextureManager;

class Framebuffer
{
public:
  Framebuffer(TextureManager& textureManager);
  virtual ~Framebuffer();
  virtual void setup() = 0;
  void checkStatus();
  void bindToViewport(int viewportWidth, int viewportHeight);
  void unbindToViewport(int viewportWidth, int viewportHeight);

protected:
  GLuint fbo;
  TextureManager& textureManager;
};
