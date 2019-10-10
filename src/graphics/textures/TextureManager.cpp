/*
 * Copyright 2019 Ilya Malgin
 * TextureManager.cpp
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
 * Purpose: contains definitions for TextureManager class
 * @version 0.1.0
 */

#include "TextureManager"
#include "GraphicsSettings"
#include "TextureLoader"

/**
* @brief plain ctor, initializes all non-bindless textures with appropriate parameters
* @param loader texture loader that does all 'low-level' stuff
*/
TextureManager::TextureManager( TextureLoader & loader )
	: loader( loader )
{
	textures[TEX_LAND] = loader.loadTextureResource( "land.jpg", TEX_LAND, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true );
	textures[TEX_LAND_2] = loader.loadTextureResource( "land2.jpg", TEX_LAND_2, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true );
	textures[TEX_DIFFUSE_MIX_MAP] = loader.loadTextureResource( "diffuseMixMap.jpg", TEX_DIFFUSE_MIX_MAP, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true, false, true );
	textures[TEX_TERRAIN_NORMAL] = loader.loadTextureResource( "terrainNormal.jpg", TEX_TERRAIN_NORMAL, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true, false, true );
	textures[TEX_WATER_NORMAL] = loader.loadTextureResource( "waterNormal.jpg", TEX_WATER_NORMAL, GL_REPEAT, GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST, true, false, true );
	textures[TEX_WATER_SPECULAR] = loader.loadTextureResource( "waterSpecular.jpg", TEX_WATER_SPECULAR, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true, false, true );
	textures[TEX_WATER_DUDV] = loader.loadTextureResource( "waterDUDV.png", TEX_WATER_DUDV, GL_MIRRORED_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true, false, true );
	textures[TEX_HILL] = loader.loadTextureResource( "hill.jpg", TEX_HILL, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true );
	textures[TEX_HILL_SPECULAR] = loader.loadTextureResource( "hill_specular.png", TEX_HILL_SPECULAR, GL_REPEAT, GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST, true, false, true );
	textures[TEX_SHORE] = loader.loadTextureResource( "shore.jpg", TEX_SHORE, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true );
	textures[TEX_UNDERWATER_DIFFUSE] = loader.loadTextureResource( "underwater.jpg", TEX_UNDERWATER_DIFFUSE, GL_REPEAT, GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST, false );
	textures[TEX_SKYBOX_HILLS_NEAR] = loader.loadCubemapResource( "cubemapHillsNear\\", TEX_SKYBOX_HILLS_NEAR, false );
	textures[TEX_SKYBOX_HILLS_NEAR_NORMAL] = loader.loadCubemapResource( "cubemapHillsNearNormal\\", TEX_SKYBOX_HILLS_NEAR_NORMAL, true );
	textures[TEX_SKYBOX_HILLS_FAR] = loader.loadCubemapResource( "cubemapHillsFar\\", TEX_SKYBOX_HILLS_FAR, false );
	textures[TEX_SKYBOX_HILLS_FAR_NORMAL] = loader.loadCubemapResource( "cubemapHillsFarNormal\\", TEX_SKYBOX_HILLS_FAR_NORMAL, true );
	textures[TEX_THE_SUN] = loader.loadTextureResource( "theSun.png", TEX_THE_SUN, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true );
	textures[TEX_SKYSPHERE_THE_SUN_AMBIENT_LIGHTING] = loader.loadTextureResource( "theSunEnvironmentLight.png", TEX_SKYSPHERE_THE_SUN_AMBIENT_LIGHTING, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, false );
	textures[TEX_SKYSPHERE_STARS] = loader.loadTextureResource( "stars.png", TEX_SKYSPHERE_STARS, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true );
	textures[TEX_SKYSPHERE_CLOUDS] = loader.loadTextureResource( "cloudsSeamless.png", TEX_SKYSPHERE_CLOUDS, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true );
	textures[TEX_FRAME_MULTISAMPLED] = loader.createFrameMSTexture( TEX_FRAME_MULTISAMPLED, MULTISAMPLES );
	textures[TEX_FRAME] = loader.createFrameTexture( TEX_FRAME, false );
	textures[TEX_FRAME_HDR] = loader.createFrameTexture( TEX_FRAME_HDR, false );
	textures[TEX_FRAME_DEPTH] = loader.createFrameTexture( TEX_FRAME_DEPTH, true );
	textures[TEX_FRAME_VIGNETTE] = loader.loadTextureResource( "vignetteHoneycomb.png", TEX_FRAME_VIGNETTE, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, false, false, true );
	textures[TEX_FRAME_WATER_REFLECTION] = loader.createFrameTextureSized( TEX_FRAME_WATER_REFLECTION, false, FRAME_WATER_REFLECTION_WIDTH, FRAME_WATER_REFLECTION_HEIGHT, true );
	textures[TEX_FRAME_WATER_REFRACTION] = loader.createFrameTextureSized( TEX_FRAME_WATER_REFRACTION, false, FRAME_WATER_REFRACTION_WIDTH, FRAME_WATER_REFRACTION_HEIGHT, true );
	textures[TEX_FRAME_WATER_REFRACTION_DEPTH] = loader.createFrameTextureSized( TEX_FRAME_WATER_REFRACTION_DEPTH, true, FRAME_WATER_REFRACTION_WIDTH, FRAME_WATER_REFRACTION_HEIGHT, true );
	textures[TEX_DEPTH_MAP_SUN] = loader.createDepthMapTexture( TEX_DEPTH_MAP_SUN, DEPTH_MAP_TEXTURE_WIDTH, DEPTH_MAP_TEXTURE_HEIGHT );
}

/**
* @brief sends delete texture command to OpenGL for each texture in storage
*/
TextureManager::~TextureManager()
{
	for( unsigned int textureIndex = 0; textureIndex < textures.size(); textureIndex++ )
	{
		glDeleteTextures( 1, &( textures[textureIndex] ) );
	}
}

/**
* @brief explicitly delegates command to create underwater relief texture and stores (or replaces) it in storage
* @param waterMap map of the water
*/
void TextureManager::createUnderwaterReliefTexture( const map2D_f & waterMap )
{
	textures[TEX_UNDERWATER_RELIEF] = loader.createUnderwaterReliefTexture( TEX_UNDERWATER_RELIEF, waterMap, GL_LINEAR, GL_LINEAR );
}

GLuint & TextureManager::get( int textureUnit )
{
	return textures[textureUnit];
}

TextureLoader & TextureManager::getLoader() noexcept
{
	return loader;
}
