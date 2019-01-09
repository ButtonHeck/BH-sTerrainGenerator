#include "SkyboxRenderer.h"

SkyboxRenderer::SkyboxRenderer(Skybox &skybox)
  :
    skybox(skybox)
{}

void SkyboxRenderer::render()
{
  BENCHMARK("SkyboxRenderer: draw", true);
  skybox.basicGLBuffers.bind(VAO);
  glDrawArrays(GL_TRIANGLES, 0, VERTICES_PER_QUAD * VERTICES_PER_QUAD);
}
