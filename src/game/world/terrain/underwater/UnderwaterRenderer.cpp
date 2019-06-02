#include "UnderwaterRenderer.h"

UnderwaterRenderer::UnderwaterRenderer(UnderwaterGenerator &generator)
  :
    generator(generator)
{}

void UnderwaterRenderer::render()
{
  generator.basicGLBuffers.bind(VAO);
  glDrawElements(GL_TRIANGLES, VERTICES_PER_QUAD, GL_UNSIGNED_BYTE, 0);
}
