#ifndef HILLSSHADER_H
#define HILLSSHADER_H
#include "graphics/shaders/Shader.h"
#include "graphics/Frustum.h"

class HillsShader
{
public:
  HillsShader(Shader& renderShader, Shader& cullingShader, Shader& normalsShader);
  void setupCulling();
  void update(glm::vec3& lightDir,
              const std::array<glm::mat4, NUM_SHADOW_LAYERS> &lightSpaceMatrices,
              glm::mat4& projectionView,
              glm::vec3& viewPosition,
              Frustum& viewFrustum,
              float maxHillHeight,
              bool useFC,
              bool useShadows);
  void updateNormals(glm::mat4& projectionView);
  void debugRenderMode(bool enable);
private:
  friend class HillsRenderer;
  Shader& renderShader;
  Shader& cullingShader;
  Shader& normalsShader;
};

#endif // HILLSSHADER_H
