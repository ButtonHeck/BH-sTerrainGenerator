#include "graphics/DepthmapBuffer.h"

DepthmapBuffer::~DepthmapBuffer()
{
  glDeleteFramebuffers(1, &FBO);
}

void DepthmapBuffer::setup(GLuint depthTextureUnit)
{
  BENCHMARK("DepthmapBuffer: setup", false);
  glGenFramebuffers(1, &FBO);
  glBindFramebuffer(GL_FRAMEBUFFER, FBO);
  glNamedFramebufferTexture(FBO, GL_DEPTH_ATTACHMENT, depthTextureUnit, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  /*
  //For some reason uncommenting this code leads to segfault in the load-from-file part of the game
  GLint* isLayered;
  glGetNamedFramebufferAttachmentParameteriv(FBO, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_LAYERED, isLayered);
  if (*isLayered != GL_TRUE)
    Logger::log("Framebuffer depth attachment is not layered!\n");
  */

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    Logger::log("Depthmap framebuffer is not complete\n");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthmapBuffer::bindToViewport(int frameWidth, int frameHeight)
{
  glViewport(0, 0, frameWidth, frameHeight);
  glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void DepthmapBuffer::unbindToViewport(int frameWidth, int frameHeight)
{
  glViewport(0, 0, frameWidth, frameHeight);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
