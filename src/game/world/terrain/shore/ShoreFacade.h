#ifndef SHOREFACADE_H
#define SHOREFACADE_H
#include <memory>
#include "game/world/terrain/shore/ShoreGenerator.h"
#include "game/world/terrain/shore/ShoreShader.h"
#include "game/world/terrain/shore/ShoreRenderer.h"
#include "util/BenchmarkTimer.h"

class ShoreFacade
{
public:
  ShoreFacade(Shader &renderShader, map2D_f& waterMap);
  void setup();
  void serialize(std::ofstream& output);
  void deserialize(std::ifstream& input);
  void draw(glm::mat4& projectionView, bool useShadows);
  void drawDepthmap();
  map2D_f& getMap() const;

private:
  ShoreShader shader;
  std::shared_ptr<ShoreGenerator> generator;
  ShoreRenderer renderer;
};

#endif // SHOREFACADE_H
