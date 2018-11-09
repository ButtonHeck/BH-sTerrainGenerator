#ifndef SHORESHADER_H
#define SHORESHADER_H
#include "graphics/Shader.h"

class ShoreShader
{
public:
  ShoreShader(Shader& renderShader, Shader &normalsShader);
  void update(glm::mat4 &projectionView, bool useShadows);
  void updateNormals(glm::mat4& projectionView);
  void debugRenderMode(bool enable);
private:
  Shader& renderShader;
  Shader& normalsShader;
};

#endif // SHORESHADER_H
