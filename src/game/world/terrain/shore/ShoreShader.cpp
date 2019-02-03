#include "game/world/terrain/shore/ShoreShader.h"

ShoreShader::ShoreShader(Shader &renderShader, Shader &normalsShader)
  :
    renderShader(renderShader),
    normalsShader(normalsShader)
{}

void ShoreShader::update(const glm::vec3 &lightDir,
                         const std::array<glm::mat4, NUM_SHADOW_LAYERS> &lightSpaceMatrices,
                         const glm::mat4 &projectionView,
                         bool useShadows,
                         bool useClipDistanceReflection,
                         bool useClipDistanceRefraction)
{
  renderShader.use();
  renderShader.setMat4("u_projectionView", projectionView);
  renderShader.setBool("u_shadowEnable", useShadows);
  renderShader.setVec3("u_lightDir", -lightDir);
  renderShader.setMat4("u_lightSpaceMatrix[0]", lightSpaceMatrices[0]);
  renderShader.setMat4("u_lightSpaceMatrix[1]", lightSpaceMatrices[1]);
  renderShader.setMat4("u_lightSpaceMatrix[2]", lightSpaceMatrices[2]);
  renderShader.setBool("u_useClipDistanceReflection", useClipDistanceReflection);
  renderShader.setBool("u_useClipDistanceRefraction", useClipDistanceRefraction);
}

void ShoreShader::updateNormals(const glm::mat4 &projectionView)
{
  normalsShader.use();
  normalsShader.setMat4("u_projectionView", projectionView);
}

void ShoreShader::debugRenderMode(bool enable)
{
  renderShader.use();
  renderShader.setBool("u_debugRenderMode", enable);
}
