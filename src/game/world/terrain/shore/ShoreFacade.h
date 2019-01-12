#ifndef SHOREFACADE_H
#define SHOREFACADE_H
#include "game/world/terrain/shore/ShoreGenerator.h"
#include "game/world/terrain/shore/ShoreShader.h"
#include "game/world/terrain/shore/ShoreRenderer.h"
#include "util/BenchmarkTimer.h"

class ShoreFacade
{
public:
  ShoreFacade(Shader &renderShader, Shader &normalsShader, const map2D_f& waterMap);
  void setup();
  void serialize(std::ofstream& output);
  void deserialize(std::ifstream& input);
  void draw(glm::vec3& lightDir,
            const std::array<glm::mat4, NUM_SHADOW_LAYERS> &lightSpaceMatrices,
            glm::mat4& projectionView,
            bool useShadows,
            bool useDebugRender,
            bool useClipDistanceReflection,
            bool useClipDistanceRefraction);
  void drawDepthmap();
  const map2D_f& getMap() const;

private:
  ShoreShader shader;
  ShoreGenerator generator;
  ShoreRenderer renderer;
};

#endif // SHOREFACADE_H
