#include "ShaderManager.h"

ShaderManager::ShaderManager()
{
  shaders.assign(
  {
    {SHADER_HILLS_FC,               Shader("hillsFC.vs", "hillsFC.gs", "_FC.fs")},
    {SHADER_HILLS_NOFC,             Shader("hills.vs", "hills.fs")},
    {SHADER_SHORE,                  Shader("shore.vs", "shore.fs")},
    {SHADER_UNDERWATER,             Shader("underwater.vs", "underwater.fs")},
    {SHADER_FLAT,                   Shader("flat.vs", "flat.fs")},
    {SHADER_WATER_FC,               Shader("waterFC.vs", "waterFC.gs", "_FC.fs")},
    {SHADER_WATER_NOFC,             Shader("water.vs", "water.fs")},
    {SHADER_SKY,                    Shader("skybox.vs", "skybox.fs")},
    {SHADER_MODELS,                 Shader("model.vs", "model.fs")},
    {SHADER_FONT,                   Shader("font.vs", "font.fs")},
    {SHADER_CS,                     Shader("coordinateSystem.vs", "coordinateSystem.gs", "coordinateSystem.fs")},
    {SHADER_BUILDABLE,              Shader("buildableTiles.vs", "buildableTiles.fs")},
    {SHADER_SELECTED,               Shader("selectedTile.vs", "selectedTile.fs")},
    {SHADER_MS_TO_DEFAULT,          Shader("MS_toDefault.vs", "MS_toDefault_hdr.fs")},
    {SHADER_SHADOW_TERRAIN,         Shader("terrain_shadow.vs")},
    {SHADER_SHADOW_MODELS,          Shader("model_shadow.vs")},
    {SHADER_SHADOW_TERRAIN_CAMERA,  Shader("terrain_shadow.vs")},
    {SHADER_SHADOW_MODELS_CAMERA,   Shader("model_shadow.vs")},
    {SHADER_MODELS_PHONG,           Shader("modelPhong.vs", "modelPhong.fs")}
        });
}

void ShaderManager::setupConstantUniforms()
{
  Shader* shader = &shaders[SHADER_HILLS_NOFC].second;
  shader->use();
  shader->setVec3("u_lightDir", glm::normalize(-LIGHT_DIR_TO));
  shader->setInt("u_flat_diffuse", FLAT_x2);
  shader->setInt("u_flat_diffuse2", FLAT_2_x2);
  shader->setInt("u_hills_diffuse", HILL);
  shader->setInt("u_hills_diffuse2", HILL_2);
  shader->setInt("u_hills_specular", HILL_SPECULAR);
  shader->setInt("u_diffuse_mix_map", DIFFUSE_MIX_MAP);
  shader->setInt("u_normal_map", TERRAIN_NORMAL);
  shader->setFloat("u_mapDimension", 1.0f / (float)WORLD_WIDTH);
  shader->setMat4("u_lightSpaceMatrix", LIGHT_SPACE_MATRIX);
  shader->setInt("u_shadowMap", DEPTH_MAP_SUN);

  shader = &shaders[SHADER_SHORE].second;
  shader->use();
  shader->setInt("u_flat_diffuse", FLAT);
  shader->setInt("u_flat_diffuse2", FLAT_2);
  shader->setInt("u_sand_diffuse", SHORE);
  shader->setInt("u_sand_diffuse2", SHORE_2);
  shader->setInt("u_diffuse_mix_map", DIFFUSE_MIX_MAP);
  shader->setInt("u_normal_map", TERRAIN_NORMAL);
  shader->setFloat("u_mapDimension", 1.0f / (float)WORLD_WIDTH);
  shader->setVec3("u_lightDir", glm::normalize(-LIGHT_DIR_TO));
  shader->setMat4("u_lightSpaceMatrix", LIGHT_SPACE_MATRIX);
  shader->setInt("u_shadowMap", DEPTH_MAP_SUN);
  shader->setFloat("U_UNDERWATER_TILE_YPOS", -UNDERWATER_TILE_YPOS);

  shader = &shaders[SHADER_UNDERWATER].second;
  shader->use();
  shader->setInt("u_underwater_diffuse", UNDERWATER_DIFFUSE);
  shader->setInt("u_bottomRelief_diffuse", UNDERWATER_RELIEF);
  shader->setInt("u_normal_map", TERRAIN_NORMAL);
  shader->setVec3("u_lightDir", glm::normalize(-LIGHT_DIR_TO));
  shader->setFloat("u_mapDimension", 1.0f / (float)WORLD_WIDTH);

  shader = &shaders[SHADER_FLAT].second;
  shader->use();
  shader->setInt("u_flat_diffuse", FLAT);
  shader->setInt("u_flat_diffuse2", FLAT_2);
  shader->setInt("u_diffuse_mix_map", DIFFUSE_MIX_MAP);
  shader->setInt("u_normal_map", TERRAIN_NORMAL);
  shader->setFloat("u_mapDimension", 1.0f / (float)WORLD_WIDTH);
  shader->setVec3("u_lightDir", glm::normalize(-LIGHT_DIR_TO));
  shader->setMat4("u_lightSpaceMatrix", LIGHT_SPACE_MATRIX);
  shader->setInt("u_shadowMap", DEPTH_MAP_SUN);

  shader = &shaders[SHADER_WATER_NOFC].second;
  shader->use();
  shader->setVec3("u_lightDir", glm::normalize(-LIGHT_DIR_TO));
  shader->setInt("u_skybox", SKYBOX);
  shader->setInt("u_bottomRelief_diffuse", UNDERWATER_RELIEF);
  shader->setInt("u_normal_map", WATER_NORMAL);
  shader->setInt("u_specular_map", WATER_SPECULAR);
  shader->setFloat("u_mapDimension", 1.0f / WORLD_WIDTH);

  shader = &shaders[SHADER_SKY].second;
  shader->use();
  shader->setInt("u_skybox", SKYBOX);

  shader = &shaders[SHADER_MODELS].second;
  shader->use();
  shader->setVec3("u_lightDir", glm::normalize(-LIGHT_DIR_TO));
  shader->setMat4("u_lightSpaceMatrix", LIGHT_SPACE_MATRIX);
  shader->setInt("u_shadowMap", DEPTH_MAP_SUN);

  shader = &shaders[SHADER_MODELS_PHONG].second;
  shader->use();
  shader->setVec3("u_lightDir", glm::normalize(-LIGHT_DIR_TO));
  shader->setMat4("u_lightSpaceMatrix", LIGHT_SPACE_MATRIX);
  shader->setInt("u_shadowMap", DEPTH_MAP_SUN);

  shader = &shaders[SHADER_MS_TO_DEFAULT].second;
  shader->use();
  shader->setInt("u_frameTexture", HDR_ENABLED ? FRAME_HDR_TEXTURE : FRAME_TEXTURE);
  shader->setFloat("u_exposure", 2.2f);

  //below we setup shadow shader uniforms
  shader = &shaders[SHADER_SHADOW_TERRAIN].second;
  shader->use();
  shader->setMat4("u_lightSpaceMatrix", LIGHT_SPACE_MATRIX);

  shader = &shaders[SHADER_SHADOW_MODELS].second;
  shader->use();
  shader->setMat4("u_lightSpaceMatrix", LIGHT_SPACE_MATRIX);
}

Shader &ShaderManager::get(SHADER_TYPE type)
{
  return shaders[type].second;
}

void ShaderManager::updateHillsShaders(bool useFC, bool useShadows, glm::mat4 &projectionView, glm::vec3 &viewPosition, Frustum &viewFrustum, float maxHillHeight)
{
  Shader* shader;
  if (useFC)
    {
      shader = &shaders[SHADER_HILLS_FC].second;
      shader->use();
      shader->setVec4("u_frustumPlanes[0]", viewFrustum.getPlane(FRUSTUM_LEFT));
      shader->setVec4("u_frustumPlanes[1]", viewFrustum.getPlane(FRUSTUM_RIGHT));
      shader->setVec4("u_frustumPlanes[2]", viewFrustum.getPlane(FRUSTUM_BOTTOM));
      shader->setVec4("u_frustumPlanes[3]", viewFrustum.getPlane(FRUSTUM_TOP));
      shader->setVec4("u_frustumPlanes[4]", viewFrustum.getPlane(FRUSTUM_BACK));
    }
  shader = &shaders[SHADER_HILLS_NOFC].second;
  shader->use();
  shader->setMat4("u_projectionView", projectionView);
  shader->setVec3("u_viewPosition", viewPosition);
  shader->setBool("u_shadowEnable", useShadows);
  shader->setFloat("u_maxHillHeight", maxHillHeight);
}

void ShaderManager::updateShoreShader(glm::mat4 &projectionView, bool useShadows)
{
  Shader* shader = &shaders[SHADER_SHORE].second;
  shader->use();
  shader->setMat4("u_projectionView", projectionView);
  shader->setBool("u_shadowEnable", useShadows);
}

void ShaderManager::updateFlatShader(glm::mat4 &projectionView, bool useShadows)
{
  Shader* shader = &shaders[SHADER_FLAT].second;
  shader->use();
  shader->setMat4("u_projectionView", projectionView);
  shader->setBool("u_shadowEnable", useShadows);
}

void ShaderManager::updateUnderwaterShader(glm::mat4 &projectionView)
{
  Shader* shader = &shaders[SHADER_UNDERWATER].second;
  shader->use();
  shader->setMat4("u_projectionView", projectionView);
}

void ShaderManager::updateBuildableShader(glm::mat4 &projectionView)
{
  Shader* shader = &shaders[SHADER_BUILDABLE].second;
  shader->use();
  shader->setMat4("u_projectionView", projectionView);
}

void ShaderManager::updateSelectedShader(glm::mat4 &projectionView, glm::mat4 &selectedModel)
{
  Shader* shader = &shaders[SHADER_SELECTED].second;
  shader->use();
  shader->setMat4("u_projectionView", projectionView);
  shader->setMat4("u_model", selectedModel);
}

void ShaderManager::updateWaterShaders(bool useFC, glm::mat4 &projectionView, glm::vec3 &viewPosition, Frustum &viewFrustum)
{
  Shader* shader;
  if (useFC)
    {
      shader = &shaders[SHADER_WATER_FC].second;
      shader->use();
      shader->setVec4("u_frustumPlanes[0]", viewFrustum.getPlane(FRUSTUM_LEFT));
      shader->setVec4("u_frustumPlanes[1]", viewFrustum.getPlane(FRUSTUM_RIGHT));
      shader->setVec4("u_frustumPlanes[2]", viewFrustum.getPlane(FRUSTUM_BOTTOM));
      shader->setVec4("u_frustumPlanes[3]", viewFrustum.getPlane(FRUSTUM_TOP));
      shader->setVec4("u_frustumPlanes[4]", viewFrustum.getPlane(FRUSTUM_BACK));
    }
  shader = &shaders[SHADER_WATER_NOFC].second;
  shader->use();
  shader->setMat4("u_projectionView", projectionView);
  shader->setVec3("u_viewPosition", viewPosition);
}

void ShaderManager::updateSkyShader(glm::mat4 &projectionView)
{
  Shader* shader = &shaders[SHADER_SKY].second;
  shader->use();
  shader->setMat4("u_projectionView", projectionView);
}

void ShaderManager::updateModelShader(glm::mat4 &projectionView, glm::vec3 &viewPosition,
                                      bool shadowOnTrees,
                                      bool useShadows,
                                      bool useFlatBlending)
{
  Shader* shader = &shaders[SHADER_MODELS].second;
  shader->use();
  shader->setMat4("u_projectionView", projectionView);
  shader->setVec3("u_viewPosition", viewPosition);
  shader->setBool("u_shadow", shadowOnTrees);
  shader->setBool("u_shadowEnable", useShadows);
  shader->setBool("u_useFlatBlending", useFlatBlending);

  shader = &shaders[SHADER_MODELS_PHONG].second;
  shader->use();
  shader->setMat4("u_projectionView", projectionView);
  shader->setVec3("u_viewPosition", viewPosition);
  shader->setBool("u_shadow", shadowOnTrees);
  shader->setBool("u_shadowEnable", useShadows);
  shader->setBool("u_useFlatBlending", useFlatBlending);
}

void ShaderManager::deleteShaders()
{
  glUseProgram(0);
  for (unsigned int i = 0; i < shaders.size(); i++)
    shaders[i].second.cleanUp();
}
