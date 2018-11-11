#ifndef LANDFACADE_H
#define LANDFACADE_H
#include "game/world/terrain/land/LandGenerator.h"
#include "game/world/terrain/land/LandShader.h"
#include "game/world/terrain/land/LandRenderer.h"

class LandFacade
{
public:
  LandFacade(Shader& renderShader);
  void setup(const map2D_f& shoreMap);
  void serialize(std::ofstream& output);
  void deserialize(std::ifstream& input);
  void draw(glm::mat4 &projectionView, Frustum &viewFrustum, bool useShadows, GLuint &texture);
  const map2D_f &getMap() const;

private:
  LandShader shader;
  LandGenerator generator;
  LandRenderer renderer;
};

#endif // LANDFACADE_H
