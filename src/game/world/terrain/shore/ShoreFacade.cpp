#include "ShoreFacade.h"

ShoreFacade::ShoreFacade(Shader &renderShader, const map2D_f &waterMap)
  :
    shader(renderShader),
    generator(waterMap),
    renderer(generator)
{}

void ShoreFacade::setup()
{
  generator.setup();
}

void ShoreFacade::serialize(std::ofstream &output)
{
  generator.serialize(output);
}

void ShoreFacade::deserialize(std::ifstream &input)
{
  generator.deserialize(input);
}

void ShoreFacade::draw(glm::mat4 &projectionView, bool useShadows)
{
  shader.update(projectionView, useShadows);
  {
    BENCHMARK("Renderer: draw shore", true);
    renderer.render();
  }
}

void ShoreFacade::drawDepthmap()
{
  renderer.render();
}

const map2D_f &ShoreFacade::getMap() const
{
  return generator.getMap();
}
