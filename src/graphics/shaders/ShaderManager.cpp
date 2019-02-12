#include "graphics/shaders/ShaderManager.h"

ShaderManager::ShaderManager()
{
  shaders[SHADER_HILLS_CULLING] = Shader({GL_VERTEX_SHADER, "hillsFC/hillsFC.vs"},
                                         {GL_GEOMETRY_SHADER, "hillsFC/hillsFC.gs"});
  shaders[SHADER_HILLS] = Shader({GL_VERTEX_SHADER, "hills/hills.vs"},
                                 {GL_FRAGMENT_SHADER, "hills/hills.fs"},
                                 {{GL_FRAGMENT_SHADER, "shadowSampling.ifs"},
                                 {GL_FRAGMENT_SHADER, "desaturationFunc.ifs"},
                                 {GL_FRAGMENT_SHADER, "shadingVariables.ifs"}});
  shaders[SHADER_HILLS_NORMALS] = Shader({GL_VERTEX_SHADER, "normals/hills_normals.vs"},
                                         {GL_GEOMETRY_SHADER, "normals/_normals.gs"},
                                         {GL_FRAGMENT_SHADER, "normals/_normals.fs"});
  shaders[SHADER_SHORE] = Shader({GL_VERTEX_SHADER, "shore/shore.vs"},
                                 {GL_FRAGMENT_SHADER, "shore/shore.fs"},
                                 {{GL_FRAGMENT_SHADER, "shadowSampling.ifs"},
                                 {GL_FRAGMENT_SHADER, "desaturationFunc.ifs"},
                                 {GL_FRAGMENT_SHADER, "shadingVariables.ifs"}});
  shaders[SHADER_SHORE_NORMALS] = Shader({GL_VERTEX_SHADER, "normals/shore_normals.vs"},
                                         {GL_GEOMETRY_SHADER, "normals/_normals.gs"},
                                         {GL_FRAGMENT_SHADER, "normals/_normals.fs"});
  shaders[SHADER_UNDERWATER] = Shader({GL_VERTEX_SHADER, "underwater/underwater.vs"},
                                      {GL_FRAGMENT_SHADER, "underwater/underwater.fs"},
                                      {{GL_FRAGMENT_SHADER, "desaturationFunc.ifs"},
                                      {GL_FRAGMENT_SHADER, "shadingVariables.ifs"}});
  shaders[SHADER_LAND] = Shader({GL_VERTEX_SHADER, "land/land.vs"},
                                {GL_FRAGMENT_SHADER, "land/land.fs"},
                                {{GL_FRAGMENT_SHADER, "shadowSampling.ifs"},
                                {GL_FRAGMENT_SHADER, "desaturationFunc.ifs"},
                                {GL_FRAGMENT_SHADER, "shadingVariables.ifs"}});
  shaders[SHADER_WATER_CULLING] = Shader({GL_VERTEX_SHADER, "waterFC/waterFC.vs"},
                                         {GL_GEOMETRY_SHADER, "waterFC/waterFC.gs"});
  shaders[SHADER_WATER] = Shader({GL_VERTEX_SHADER, "water/water.vs"},
                                 {GL_FRAGMENT_SHADER, "water/water.fs"},
                                 {{GL_FRAGMENT_SHADER, "shadowSampling.ifs"},
                                 {GL_FRAGMENT_SHADER, "desaturationFunc.ifs"},
                                 {GL_FRAGMENT_SHADER, "shadingVariables.ifs"}});
  shaders[SHADER_WATER_NORMALS] = Shader({GL_VERTEX_SHADER, "normals/water_normals.vs"},
                                         {GL_GEOMETRY_SHADER, "normals/_normals.gs"},
                                         {GL_FRAGMENT_SHADER, "normals/_normals.fs"});
  shaders[SHADER_SKYBOX] = Shader({GL_VERTEX_SHADER, "skybox/skybox.vs"},
                                  {GL_FRAGMENT_SHADER, "skybox/skybox.fs"},
                                  {{GL_FRAGMENT_SHADER, "shadingVariables.ifs"},
                                  {GL_FRAGMENT_SHADER, "desaturationFunc.ifs"}});
  shaders[SHADER_SUN] = Shader({GL_VERTEX_SHADER, "theSun/theSun.vs"},
                               {GL_FRAGMENT_SHADER, "theSun/theSun.fs"});
  shaders[SHADER_MODELS_GOURAUD] = Shader({GL_VERTEX_SHADER, "modelGouraud/model.vs"},
                                  {GL_FRAGMENT_SHADER, "modelGouraud/model.fs"},
                                  {{GL_VERTEX_SHADER, "modelGrassAnimationAndBlending.ivs"},
                                  {GL_FRAGMENT_SHADER, "shadowSampling.ifs"},
                                  {GL_FRAGMENT_SHADER, "desaturationFunc.ifs"},
                                  {GL_FRAGMENT_SHADER, "shadingVariables.ifs"}});
  shaders[SHADER_MODELS_PHONG] = Shader({GL_VERTEX_SHADER, "modelPhong/modelPhong.vs"},
                                        {GL_FRAGMENT_SHADER, "modelPhong/modelPhong.fs"},
                                        {{GL_VERTEX_SHADER, "modelGrassAnimationAndBlending.ivs"},
                                        {GL_FRAGMENT_SHADER, "shadowSampling.ifs"},
                                        {GL_FRAGMENT_SHADER, "desaturationFunc.ifs"},
                                        {GL_FRAGMENT_SHADER, "shadingVariables.ifs"}});
  shaders[SHADER_FONT] = Shader({GL_VERTEX_SHADER, "font/font.vs"},
                                {GL_FRAGMENT_SHADER, "font/font.fs"});
  shaders[SHADER_COORDINATE_SYSTEM] = Shader({GL_VERTEX_SHADER, "coordinateSystem/coordinateSystem.vs"},
                                             {GL_GEOMETRY_SHADER, "coordinateSystem/coordinateSystem.gs"},
                                             {GL_FRAGMENT_SHADER, "coordinateSystem/coordinateSystem.fs"});
  shaders[SHADER_BUILDABLE] = Shader({GL_VERTEX_SHADER, "buildable/buildableTiles.vs"},
                                     {GL_FRAGMENT_SHADER, "buildable/buildableTiles.fs"});
  shaders[SHADER_SELECTED] = Shader({GL_VERTEX_SHADER, "selected/selectedTile.vs"},
                                    {GL_FRAGMENT_SHADER, "selected/selectedTile.fs"});
  shaders[SHADER_MS_TO_DEFAULT] = Shader({GL_VERTEX_SHADER, "screen/MS_toDefault.vs"},
                                         {GL_FRAGMENT_SHADER, "screen/MS_toDefault_hdr.fs"});
  shaders[SHADER_SHADOW_TERRAIN] = Shader({GL_VERTEX_SHADER, "shadow/terrain_shadow.vs"},
                                          {GL_GEOMETRY_SHADER, "shadow/shadow.gs"});
  shaders[SHADER_SHADOW_MODELS] = Shader({GL_VERTEX_SHADER, "shadow/model_shadow.vs"},
                                         {GL_GEOMETRY_SHADER, "shadow/shadow.gs"});
  shaders[SHADER_FRUSTUM] = Shader({GL_VERTEX_SHADER, "frustum/frustum.vs"},
                                   {GL_FRAGMENT_SHADER, "frustum/frustum.fs"});
  shaders[SHADER_LENS_FLARE] = Shader({GL_VERTEX_SHADER, "lensFlare/lensFlare.vs"},
                                      {GL_FRAGMENT_SHADER, "lensFlare/lensFlare.fs"});
  shaders[SHADER_SKYSPHERE] = Shader({GL_VERTEX_SHADER, "skysphere/skysphere.vs"},
                                      {GL_FRAGMENT_SHADER, "skysphere/skysphere.fs"});
}

ShaderManager::~ShaderManager()
{
  glUseProgram(0);
  for (unsigned int shaderIndex = 0; shaderIndex < shaders.size(); shaderIndex++)
    shaders[shaderIndex].cleanUp();
}

#define bindShaderUnit(shader, type) \
  shader = &shaders[type]; \
  shader->use();

void ShaderManager::setupConstantUniforms(const ScreenResolution& screenResolution)
{
  Shader* shader = nullptr;
  bindShaderUnit(shader, SHADER_HILLS);
  shader->setInt("u_landDiffuse[0]", TEX_LAND);
  shader->setInt("u_landDiffuse[1]", TEX_LAND_2);
  shader->setInt("u_hillsDiffuse[0]", TEX_HILL);
  shader->setInt("u_hillsDiffuse[1]", TEX_HILL_2);
  shader->setInt("u_hillsSpecular", TEX_HILL_SPECULAR);
  shader->setInt("u_diffuseMixMap", TEX_DIFFUSE_MIX_MAP);
  shader->setInt("u_normalMap", TEX_TERRAIN_NORMAL);
  shader->setInt("u_textureTilingDimension", HILL_TILING_PER_TEXTURE_QUAD);
  shader->setFloat("u_normalMapTilingReciprocal", NORMAL_MAP_TILING_RECIPROCAL);
  shader->setFloat("u_mapDimensionReciprocal", 1.0f / (float)WORLD_WIDTH);
  shader->setInt("u_shadowMap", TEX_DEPTH_MAP_SUN);
  shader->setFloat("u_bias", 5.0f / DEPTH_MAP_TEXTURE_WIDTH);
  shader->setFloat("u_ambientDay", 0.08f);
  shader->setFloat("u_ambientNight", 0.03f);

  bindShaderUnit(shader, SHADER_SHORE);
  shader->setInt("u_landDiffuse[0]", TEX_LAND);
  shader->setInt("u_landDiffuse[1]", TEX_LAND_2);
  shader->setInt("u_sand_diffuse[0]", TEX_SHORE);
  shader->setInt("u_sand_diffuse[1]", TEX_SHORE_2);
  shader->setInt("u_diffuseMixMap", TEX_DIFFUSE_MIX_MAP);
  shader->setInt("u_normalMap", TEX_TERRAIN_NORMAL);
  shader->setFloat("u_mapDimensionReciprocal", 1.0f / (float)WORLD_WIDTH);
  shader->setInt("u_shadowMap", TEX_DEPTH_MAP_SUN);
  shader->setFloat("U_UNDERWATER_TILE_YPOS", -UNDERWATER_TILE_YPOS);
  shader->setFloat("U_WATER_LEVEL", WATER_LEVEL);
  shader->setFloat("u_bias", 1.0f / DEPTH_MAP_TEXTURE_WIDTH);
  shader->setFloat("u_ambientDay", 0.08f);
  shader->setFloat("u_ambientNight", 0.03f);

  bindShaderUnit(shader, SHADER_UNDERWATER);
  shader->setInt("u_underwater_diffuse", TEX_UNDERWATER_DIFFUSE);
  shader->setInt("u_bottomRelief_diffuse", TEX_UNDERWATER_RELIEF);
  shader->setInt("u_normalMap", TEX_TERRAIN_NORMAL);
  shader->setFloat("u_mapDimensionReciprocal", 1.0f / (float)WORLD_WIDTH);
  shader->setFloat("u_ambientDay", 0.08f);
  shader->setFloat("u_ambientNight", 0.03f);

  bindShaderUnit(shader, SHADER_LAND);
  shader->setInt("u_landDiffuse[0]", TEX_LAND);
  shader->setInt("u_landDiffuse[1]", TEX_LAND_2);
  shader->setInt("u_diffuseMixMap", TEX_DIFFUSE_MIX_MAP);
  shader->setInt("u_normalMap", TEX_TERRAIN_NORMAL);
  shader->setFloat("u_mapDimensionReciprocal", 1.0f / (float)WORLD_WIDTH);
  shader->setFloat("u_normalMapTilingReciprocal", NORMAL_MAP_TILING_RECIPROCAL);
  shader->setInt("u_shadowMap", TEX_DEPTH_MAP_SUN);
  shader->setFloat("u_bias", 5.0f / DEPTH_MAP_TEXTURE_WIDTH);
  shader->setFloat("u_ambientDay", 0.08f);
  shader->setFloat("u_ambientNight", 0.03f);

  bindShaderUnit(shader, SHADER_WATER);
  shader->setInt("u_normalMap", TEX_WATER_NORMAL);
  shader->setInt("u_specular_map", TEX_WATER_SPECULAR);
  shader->setInt("u_dudv_map", TEX_WATER_DUDV);
  shader->setInt("u_shadowMap", TEX_DEPTH_MAP_SUN);
  shader->setInt("u_reflectionMap", TEX_FRAME_WATER_REFLECTION);
  shader->setInt("u_refractionMap", TEX_FRAME_WATER_REFRACTION);
  shader->setInt("u_refractionDepthMap", TEX_FRAME_WATER_REFRACTION_DEPTH);
  shader->setFloat("u_bias", 4.0f / DEPTH_MAP_TEXTURE_WIDTH);
  shader->setFloat("u_ambientDay", 0.08f);
  shader->setFloat("u_ambientNight", 0.03f);
  shader->setFloat("U_SCREEN_WIDTH", screenResolution.getWidth());
  shader->setFloat("U_SCREEN_HEIGHT", screenResolution.getHeight());
  shader->setFloat("u_near", NEAR_PLANE);
  shader->setFloat("u_far", FAR_PLANE);

  bindShaderUnit(shader, SHADER_SKYBOX);
  shader->setInt("u_skyboxColor[1]", TEX_SKYBOX_HILLS_NEAR);
  shader->setInt("u_skyboxNormals[1]", TEX_SKYBOX_HILLS_NEAR_NORMAL);
  shader->setInt("u_skyboxColor[0]", TEX_SKYBOX_HILLS_FAR);
  shader->setInt("u_skyboxNormals[0]", TEX_SKYBOX_HILLS_FAR_NORMAL);
  shader->setInt("u_skyboxColor[2]", TEX_SKYBOX_CLOUDS);
  shader->setFloat("u_ambientDay", 0.08f);
  shader->setFloat("u_ambientNight", 0.03f);

  bindShaderUnit(shader, SHADER_SUN);
  shader->setInt("u_texture", TEX_THE_SUN);

  bindShaderUnit(shader, SHADER_FONT);
  shader->setMat4("u_projection", glm::ortho(0.0f, (float)screenResolution.getWidth(), 0.0f, (float)screenResolution.getHeight()));
  shader->setInt("u_fontTextureAtlas", TEX_FONT);

  bindShaderUnit(shader, SHADER_MODELS_GOURAUD);
  shader->setInt("u_shadowMap", TEX_DEPTH_MAP_SUN);
  shader->setFloat("u_bias", 8.0f / DEPTH_MAP_TEXTURE_WIDTH);
  shader->setFloat("u_ambientDay", 0.16f);
  shader->setFloat("u_ambientNight", 0.03f);

  bindShaderUnit(shader, SHADER_MODELS_PHONG);
  shader->setInt("u_shadowMap", TEX_DEPTH_MAP_SUN);
  shader->setFloat("u_bias", 8.0f / DEPTH_MAP_TEXTURE_WIDTH);
  shader->setFloat("u_ambientDay", 0.16f);
  shader->setFloat("u_ambientNight", 0.03f);

  bindShaderUnit(shader, SHADER_MS_TO_DEFAULT);
  shader->setInt("u_frameTexture", HDR_ENABLED ? TEX_FRAME_HDR : TEX_FRAME);
  shader->setInt("u_frameDepthTexture", TEX_FRAME_DEPTH);
  shader->setInt("u_vignetteTexture", TEX_FRAME_VIGNETTE);
  shader->setFloat("u_exposure", HDR_EXPOSURE);
  shader->setFloat("u_near", NEAR_PLANE);
  shader->setFloat("u_far", FAR_PLANE);
  shader->setFloat("u_aspectRatio", screenResolution.getAspectRatio());

  bindShaderUnit(shader, SHADER_WATER_NORMALS);
  shader->setInt("u_normalMap", TEX_TERRAIN_NORMAL);

  bindShaderUnit(shader, SHADER_HILLS_NORMALS);
  shader->setInt("u_normalMap", TEX_TERRAIN_NORMAL);

  bindShaderUnit(shader, SHADER_SHORE_NORMALS);
  shader->setInt("u_normalMap", TEX_TERRAIN_NORMAL);

  bindShaderUnit(shader, SHADER_SKYSPHERE);
  shader->setInt("u_theSunAmbientLightingDiffuse", TEX_SKYSPHERE_THE_SUN_AMBIENT_LIGHTING);
  shader->setInt("u_starsDiffuse", TEX_SKYSPHERE_STARS);
}

Shader &ShaderManager::get(SHADER_UNITS type)
{
  return shaders[type];
}
