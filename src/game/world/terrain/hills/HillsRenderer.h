#ifndef HILLSRENDERER_H
#define HILLSRENDERER_H
#include "HillsGenerator.h"
#include "HillsShader.h"
#include "../../src/util/BenchmarkTimer.h"

class HillsRenderer
{
public:
  HillsRenderer(HillsShader& shaders, HillsGenerator& generator);
  void render(bool useFrustumCulling, const glm::vec2 &viewAcceleration);
  void renderDepthmap();
  void debugRender(GLenum primitiveType);

private:
  const unsigned int FEEDBACK_UPDATE_DELAY = 12;
  HillsShader& shaders;
  HillsGenerator& generator;
};

#endif // HILLSRENDERER_H
