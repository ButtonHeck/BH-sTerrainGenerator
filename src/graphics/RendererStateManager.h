#ifndef RENDERERSTATEMANAGER_H
#define RENDERERSTATEMANAGER_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace RendererStateManager
{
  void setInitialRenderingState(bool useMultisample);
  void setAmbienceRenderingState(bool isOn);
};

#endif // RENDERERSTATEMANAGER_H