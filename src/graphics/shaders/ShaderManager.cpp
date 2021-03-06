/*
 * Copyright 2019 Ilya Malgin
 * ShaderManager.cpp
 * This file is part of The Sugarpunky Chocolate Disaster project
 *
 * The Sugarpunky Chocolate Disaster project is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The Sugarpunky Chocolate Disaster project is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * See <http://www.gnu.org/licenses/>
 *
 * Purpose: contains definitions for ShaderManager class
 * @version 0.1.0
 */

#include "ShaderManager"
#include "Shader"
#include "ScreenResolution"
#include "TextureUnits"
#include "SceneSettings"
#include "SettingsManager"

#include <glm/gtc/matrix_transform.hpp>

/**
* @brief plain ctor, initializes all shaders
*/
ShaderManager::ShaderManager() noexcept
{
	shaders.reserve( NUM_SHADERS );
	shaders[SHADER_HILLS_CULLING] = Shader( { GL_VERTEX_SHADER, "hillsFC\\hillsFC.vs" },
											{ GL_GEOMETRY_SHADER, "hillsFC\\hillsFC.gs" } );
	shaders[SHADER_HILLS] = Shader( { GL_VERTEX_SHADER, "hills\\hills.vs" },
									{ GL_FRAGMENT_SHADER, "hills\\hills.fs" },
									{ {GL_FRAGMENT_SHADER, "include\\shadowSampling.ifs"},
									{GL_FRAGMENT_SHADER, "include\\desaturationFunc.ifs"},
									{GL_FRAGMENT_SHADER, "include\\shadingVariables.ifs"} } );
	shaders[SHADER_HILLS_NORMALS] = Shader( { GL_VERTEX_SHADER, "normals\\hills_normals.vs" },
											{ GL_GEOMETRY_SHADER, "normals\\_normals.gs" },
											{ GL_FRAGMENT_SHADER, "normals\\_normals.fs" } );
	shaders[SHADER_SHORE] = Shader( { GL_VERTEX_SHADER, "shore\\shore.vs" },
									{ GL_FRAGMENT_SHADER, "shore\\shore.fs" },
									{ {GL_FRAGMENT_SHADER, "include\\shadowSampling.ifs"},
									{GL_FRAGMENT_SHADER, "include\\desaturationFunc.ifs"},
									{GL_FRAGMENT_SHADER, "include\\shadingVariables.ifs"} } );
	shaders[SHADER_SHORE_NORMALS] = Shader( { GL_VERTEX_SHADER, "normals\\shore_normals.vs" },
											{ GL_GEOMETRY_SHADER, "normals\\_normals.gs" },
											{ GL_FRAGMENT_SHADER, "normals\\_normals.fs" } );
	shaders[SHADER_UNDERWATER] = Shader( { GL_VERTEX_SHADER, "underwater\\underwater.vs" },
										 { GL_FRAGMENT_SHADER, "underwater\\underwater.fs" },
										 { {GL_FRAGMENT_SHADER, "include\\desaturationFunc.ifs"},
										 {GL_FRAGMENT_SHADER, "include\\shadingVariables.ifs"} } );
	shaders[SHADER_LAND] = Shader( { GL_VERTEX_SHADER, "land\\land.vs" },
								   { GL_FRAGMENT_SHADER, "land\\land.fs" },
								   { {GL_FRAGMENT_SHADER, "include\\shadowSampling.ifs"},
								   {GL_FRAGMENT_SHADER, "include\\desaturationFunc.ifs"},
								   {GL_FRAGMENT_SHADER, "include\\shadingVariables.ifs"} } );
	shaders[SHADER_WATER_CULLING] = Shader( { GL_VERTEX_SHADER, "waterFC\\waterFC.vs" },
											{ GL_GEOMETRY_SHADER, "waterFC\\waterFC.gs" } );
	shaders[SHADER_WATER] = Shader( { GL_VERTEX_SHADER, "water\\water.vs" },
									{ GL_FRAGMENT_SHADER, "water\\water.fs" },
									{ {GL_FRAGMENT_SHADER, "include\\shadowSampling.ifs"},
									{GL_FRAGMENT_SHADER, "include\\desaturationFunc.ifs"},
									{GL_FRAGMENT_SHADER, "include\\shadingVariables.ifs"} } );
	shaders[SHADER_WATER_NORMALS] = Shader( { GL_VERTEX_SHADER, "normals\\water_normals.vs" },
											{ GL_GEOMETRY_SHADER, "normals\\_normals.gs" },
											{ GL_FRAGMENT_SHADER, "normals\\_normals.fs" } );
	shaders[SHADER_SKYBOX] = Shader( { GL_VERTEX_SHADER, "skybox\\skybox.vs" },
									 { GL_FRAGMENT_SHADER, "skybox\\skybox.fs" },
									 { {GL_FRAGMENT_SHADER, "include\\shadingVariables.ifs"},
									 {GL_FRAGMENT_SHADER, "include\\desaturationFunc.ifs"} } );
	shaders[SHADER_SUN] = Shader( { GL_VERTEX_SHADER, "theSun\\theSun.vs" },
								  { GL_FRAGMENT_SHADER, "theSun\\theSun.fs" } );
	shaders[SHADER_MODELS_GOURAUD] = Shader( { GL_VERTEX_SHADER, "modelGouraud\\model.vs" },
											 { GL_FRAGMENT_SHADER, "modelGouraud\\model.fs" },
											 { {GL_VERTEX_SHADER, "include\\modelGrassAnimation.ivs"},
											 {GL_FRAGMENT_SHADER, "include\\shadowSampling.ifs"},
											 {GL_FRAGMENT_SHADER, "include\\desaturationFunc.ifs"},
											 {GL_FRAGMENT_SHADER, "include\\shadingVariables.ifs"} } );
	shaders[SHADER_MODELS_PHONG] = Shader( { GL_VERTEX_SHADER, "modelPhong\\modelPhong.vs" },
										   { GL_FRAGMENT_SHADER, "modelPhong\\modelPhong.fs" },
										   { {GL_VERTEX_SHADER, "include\\modelGrassAnimation.ivs"},
										   {GL_FRAGMENT_SHADER, "include\\shadowSampling.ifs"},
										   {GL_FRAGMENT_SHADER, "include\\desaturationFunc.ifs"},
										   {GL_FRAGMENT_SHADER, "include\\shadingVariables.ifs"} } );
	shaders[SHADER_FONT] = Shader( { GL_VERTEX_SHADER, "font\\font.vs" },
								   { GL_FRAGMENT_SHADER, "font\\font.fs" } );
	shaders[SHADER_COORDINATE_SYSTEM] = Shader( { GL_VERTEX_SHADER, "coordinateSystem\\coordinateSystem.vs" },
												{ GL_GEOMETRY_SHADER, "coordinateSystem\\coordinateSystem.gs" },
												{ GL_FRAGMENT_SHADER, "coordinateSystem\\coordinateSystem.fs" } );
	shaders[SHADER_BUILDABLE] = Shader( { GL_VERTEX_SHADER, "buildable\\buildableTiles.vs" },
										{ GL_FRAGMENT_SHADER, "buildable\\buildableTiles.fs" } );
	shaders[SHADER_SELECTED] = Shader( { GL_VERTEX_SHADER, "selected\\selectedTile.vs" },
									   { GL_FRAGMENT_SHADER, "selected\\selectedTile.fs" } );
	shaders[SHADER_MS_TO_DEFAULT] = Shader( { GL_VERTEX_SHADER, "screen\\MS_toDefault.vs" },
											{ GL_FRAGMENT_SHADER, "screen\\MS_toDefault_hdr.fs" } );
	shaders[SHADER_SHADOW_TERRAIN] = Shader( { GL_VERTEX_SHADER, "shadow\\terrain_shadow.vs" },
											 { GL_GEOMETRY_SHADER, "shadow\\shadow.gs" } );
	shaders[SHADER_SHADOW_MODELS] = Shader( { GL_VERTEX_SHADER, "shadow\\model_shadow.vs" },
											{ GL_GEOMETRY_SHADER, "shadow\\shadow.gs" } );
	shaders[SHADER_FRUSTUM] = Shader( { GL_VERTEX_SHADER, "frustum\\frustum.vs" },
									  { GL_FRAGMENT_SHADER, "frustum\\frustum.fs" } );
	shaders[SHADER_LENS_FLARE] = Shader( { GL_VERTEX_SHADER, "lensFlare\\lensFlare.vs" },
										 { GL_FRAGMENT_SHADER, "lensFlare\\lensFlare.fs" } );
	shaders[SHADER_SKYSPHERE] = Shader( { GL_VERTEX_SHADER, "skysphere\\skysphere.vs" },
										{ GL_FRAGMENT_SHADER, "skysphere\\skysphere.fs" } );
}

/**
* @brief unbinds any shader that is currently bound and delegates cleanup command to each shader in storage
*/
ShaderManager::~ShaderManager()
{
	glUseProgram( 0 );
	for( unsigned int shaderIndex = 0; shaderIndex < shaders.size(); shaderIndex++ )
	{
		shaders[shaderIndex].cleanUp();
	}
}

/**
* @brief helper macro that sets shader pointer to the given shader unit
*/
#define bindShaderUnit(shader, unit) \
  shader = &shaders[unit]; \
  shader->use();

/**
* @brief sets constant uniform values for each shader in the storage
* @param screenResolution current resolution of the screen
*/
void ShaderManager::setupConstantUniforms( const ScreenResolution & screenResolution )
{
	const float AMBIENT_DAY_TERRAIN = SettingsManager::getFloat( "SHADERS", "u_ambient_day_terrain" );
	const float AMBIENT_DAY_PLANTS = SettingsManager::getFloat( "SHADERS", "u_ambient_day_plants" );
	const float AMBEINT_NIGHT_TERRAIN = SettingsManager::getFloat( "SHADERS", "u_ambient_night_terrain" );
	const float AMBIENT_NIGHT_PLANTS = SettingsManager::getFloat( "SHADERS", "u_ambient_night_plants" );
	const int DEPTHMAP_TEXTURE_WIDTH = SettingsManager::getInt( "GRAPHICS", "depthmap_texture_width" );

	Shader * shader = nullptr;
	bindShaderUnit( shader, SHADER_HILLS );
	shader->setInt( "u_landDiffuse[0]", TEX_LAND );
	shader->setInt( "u_landDiffuse[1]", TEX_LAND_2 );
	shader->setInt( "u_hillsDiffuse", TEX_HILL );
	shader->setInt( "u_hillsSpecular", TEX_HILL_SPECULAR );
	shader->setInt( "u_diffuseMixMap", TEX_DIFFUSE_MIX_MAP );
	shader->setInt( "u_normalMap", TEX_TERRAIN_NORMAL );
	shader->setInt( "u_textureTilingDimension", HILL_TILING_PER_TEXTURE_QUAD );
	shader->setFloat( "u_normalMapTilingReciprocal", NORMAL_MAP_TILING_RECIPROCAL );
	shader->setFloat( "u_mapDimensionReciprocal", 1.0f / (float)WORLD_WIDTH );
	shader->setInt( "u_shadowMap", TEX_DEPTH_MAP_SUN );
	shader->setFloat( "u_bias", SettingsManager::getFloat( "SHADERS", "hills_bias" ) / DEPTHMAP_TEXTURE_WIDTH );
	shader->setFloat( "u_ambientDay", AMBIENT_DAY_TERRAIN );
	shader->setFloat( "u_ambientNight", AMBEINT_NIGHT_TERRAIN );

	bindShaderUnit( shader, SHADER_SHORE );
	shader->setInt( "u_landDiffuse[0]", TEX_LAND );
	shader->setInt( "u_landDiffuse[1]", TEX_LAND_2 );
	shader->setInt( "u_shoreDiffuse", TEX_SHORE );
	shader->setInt( "u_diffuseMixMap", TEX_DIFFUSE_MIX_MAP );
	shader->setInt( "u_normalMap", TEX_TERRAIN_NORMAL );
	shader->setInt( "u_underwaterDiffuse", TEX_UNDERWATER_DIFFUSE );
	shader->setInt( "u_bottomReliefDiffuse", TEX_UNDERWATER_RELIEF );
	shader->setFloat( "u_normalMapTilingReciprocal", NORMAL_MAP_TILING_RECIPROCAL );
	shader->setFloat( "u_mapDimensionReciprocal", 1.0f / (float)WORLD_WIDTH );
	shader->setInt( "u_shadowMap", TEX_DEPTH_MAP_SUN );
	shader->setFloat( "u_underwaterSurfaceLevel", -SettingsManager::getFloat( "SCENE", "underwater_level" ) );
	shader->setFloat( "u_waterLevel", SettingsManager::getFloat( "SCENE", "water_level" ) );
	shader->setFloat( "u_bias", SettingsManager::getFloat( "SHADERS", "shore_bias" ) / DEPTHMAP_TEXTURE_WIDTH );
	shader->setFloat( "u_ambientDay", AMBIENT_DAY_TERRAIN );
	shader->setFloat( "u_ambientNight", AMBEINT_NIGHT_TERRAIN );

	bindShaderUnit( shader, SHADER_UNDERWATER );
	shader->setInt( "u_underwaterDiffuse", TEX_UNDERWATER_DIFFUSE );
	shader->setInt( "u_bottomReliefDiffuse", TEX_UNDERWATER_RELIEF );
	shader->setInt( "u_normalMap", TEX_TERRAIN_NORMAL );
	shader->setFloat( "u_normalMapTilingReciprocal", NORMAL_MAP_TILING_RECIPROCAL );
	shader->setFloat( "u_mapDimensionReciprocal", 1.0f / (float)WORLD_WIDTH );
	shader->setFloat( "u_ambientDay", AMBIENT_DAY_TERRAIN );
	shader->setFloat( "u_ambientNight", AMBEINT_NIGHT_TERRAIN );

	bindShaderUnit( shader, SHADER_LAND );
	shader->setInt( "u_landDiffuse[0]", TEX_LAND );
	shader->setInt( "u_landDiffuse[1]", TEX_LAND_2 );
	shader->setInt( "u_diffuseMixMap", TEX_DIFFUSE_MIX_MAP );
	shader->setInt( "u_normalMap", TEX_TERRAIN_NORMAL );
	shader->setFloat( "u_mapDimensionReciprocal", 1.0f / (float)WORLD_WIDTH );
	shader->setFloat( "u_normalMapTilingReciprocal", NORMAL_MAP_TILING_RECIPROCAL );
	shader->setInt( "u_shadowMap", TEX_DEPTH_MAP_SUN );
	shader->setFloat( "u_bias", SettingsManager::getFloat( "SHADERS", "land_bias" ) / DEPTHMAP_TEXTURE_WIDTH );
	shader->setFloat( "u_ambientDay", AMBIENT_DAY_TERRAIN );
	shader->setFloat( "u_ambientNight", AMBEINT_NIGHT_TERRAIN );

	bindShaderUnit( shader, SHADER_WATER );
	shader->setInt( "u_normalMap", TEX_WATER_NORMAL );
	shader->setInt( "u_specularTexture", TEX_WATER_SPECULAR );
	shader->setInt( "u_dudvMap", TEX_WATER_DUDV );
	shader->setInt( "u_shadowMap", TEX_DEPTH_MAP_SUN );
	shader->setInt( "u_reflectionMap", TEX_FRAME_WATER_REFLECTION );
	shader->setInt( "u_refractionMap", TEX_FRAME_WATER_REFRACTION );
	shader->setInt( "u_refractionDepthMap", TEX_FRAME_WATER_REFRACTION_DEPTH );
	shader->setFloat( "u_normalMapTilingReciprocal", NORMAL_MAP_TILING_RECIPROCAL );
	shader->setFloat( "u_bias", SettingsManager::getFloat( "SHADERS", "water_bias" ) / DEPTHMAP_TEXTURE_WIDTH );
	shader->setFloat( "u_ambientDay", AMBIENT_DAY_TERRAIN );
	shader->setFloat( "u_ambientNight", AMBEINT_NIGHT_TERRAIN );
	shader->setFloat( "u_screenWidth", screenResolution.getWidth() );
	shader->setFloat( "u_screenHeight", screenResolution.getHeight() );
	shader->setFloat( "u_near", SettingsManager::getFloat( "GRAPHICS", "near_plane" ) );
	shader->setFloat( "u_far", SettingsManager::getFloat( "GRAPHICS", "far_plane" ) );

	bindShaderUnit( shader, SHADER_SKYBOX );
	shader->setInt( "u_skyboxColor[1]", TEX_SKYBOX_HILLS_NEAR );
	shader->setInt( "u_skyboxNormals[1]", TEX_SKYBOX_HILLS_NEAR_NORMAL );
	shader->setInt( "u_skyboxColor[0]", TEX_SKYBOX_HILLS_FAR );
	shader->setInt( "u_skyboxNormals[0]", TEX_SKYBOX_HILLS_FAR_NORMAL );
	shader->setFloat( "u_ambientDay", AMBIENT_DAY_TERRAIN );
	shader->setFloat( "u_ambientNight", AMBEINT_NIGHT_TERRAIN );

	bindShaderUnit( shader, SHADER_SUN );
	shader->setInt( "u_texture", TEX_THE_SUN );

	bindShaderUnit( shader, SHADER_FONT );
	shader->setMat4( "u_projection", glm::ortho( 0.0f, (float)screenResolution.getWidth(), 0.0f, (float)screenResolution.getHeight() ) );
	shader->setInt( "u_fontTextureAtlas", TEX_FONT );

	bindShaderUnit( shader, SHADER_MODELS_GOURAUD );
	shader->setInt( "u_shadowMap", TEX_DEPTH_MAP_SUN );
	shader->setFloat( "u_bias", SettingsManager::getFloat( "SHADERS", "models_bias" ) / DEPTHMAP_TEXTURE_WIDTH );
	shader->setFloat( "u_ambientDay", AMBIENT_DAY_PLANTS );
	shader->setFloat( "u_ambientNight", AMBIENT_NIGHT_PLANTS );

	bindShaderUnit( shader, SHADER_MODELS_PHONG );
	shader->setInt( "u_shadowMap", TEX_DEPTH_MAP_SUN );
	shader->setFloat( "u_bias", SettingsManager::getFloat( "SHADERS", "models_bias" ) / DEPTHMAP_TEXTURE_WIDTH );
	shader->setFloat( "u_ambientDay", AMBIENT_DAY_PLANTS );
	shader->setFloat( "u_ambientNight", AMBIENT_NIGHT_PLANTS );

	bindShaderUnit( shader, SHADER_MS_TO_DEFAULT );
	shader->setInt( "u_frameTexture", SettingsManager::getBool( "GRAPHICS", "hdr" ) ? TEX_FRAME_HDR : TEX_FRAME );
	shader->setInt( "u_frameDepthTexture", TEX_FRAME_DEPTH );
	shader->setInt( "u_vignetteTexture", TEX_FRAME_VIGNETTE );
	shader->setFloat( "u_exposure", SettingsManager::getFloat( "GRAPHICS", "hdr_exposure" ) );
	shader->setFloat( "u_near", SettingsManager::getFloat( "GRAPHICS", "near_plane" ) );
	shader->setFloat( "u_far", SettingsManager::getFloat( "GRAPHICS", "far_plane" ) );
	shader->setFloat( "u_screenWidth", screenResolution.getWidth() );
	shader->setFloat( "u_aspectRatio", screenResolution.getAspectRatio() );
	shader->setFloat( "u_dofDistanceLinear", SettingsManager::getFloat( "GRAPHICS", "dof_distance_linear" ) );

	bindShaderUnit( shader, SHADER_WATER_NORMALS );
	shader->setInt( "u_normalMap", TEX_TERRAIN_NORMAL );

	bindShaderUnit( shader, SHADER_HILLS_NORMALS );
	shader->setInt( "u_normalMap", TEX_TERRAIN_NORMAL );

	bindShaderUnit( shader, SHADER_SHORE_NORMALS );
	shader->setInt( "u_normalMap", TEX_TERRAIN_NORMAL );

	bindShaderUnit( shader, SHADER_SKYSPHERE );
	shader->setInt( "u_theSunAmbientLightingDiffuse", TEX_SKYSPHERE_THE_SUN_AMBIENT_LIGHTING );
	shader->setInt( "u_starsDiffuse", TEX_SKYSPHERE_STARS );
	shader->setInt( "u_cloudsDiffuse", TEX_SKYSPHERE_CLOUDS );
}

Shader & ShaderManager::get( SHADER_UNIT type )
{
	return shaders[type];
}
