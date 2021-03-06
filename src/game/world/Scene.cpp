/*
 * Copyright 2019 Ilya Malgin
 * Scene.cpp
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
 * Purpose: contains definitions for Scene class
 * @version 0.1.0
 */

#include "Scene"
#include "Camera"
#include "ShaderManager"
#include "Shader"
#include "TextureManager"
#include "MouseInputManager"
#include "ShadowVolume"
#include "RendererState"
#include "Options"
#include "Logger"
#include "SettingsManager"

/**
* @brief plain ctor, creates subsystems objects
* @param shaderManager global shader manager to request shader programs from
* @param options set of options
* @param textureManager texture manager
* @param screenResolution current resolution of screen
*/
Scene::Scene( ShaderManager & shaderManager,
			  Options & options,
			  TextureManager & textureManager,
			  const ScreenResolution & screenResolution,
			  const ShadowVolume & shadowVolume )
	: PLANET_MOVE_SPEED( SettingsManager::getFloat( "SCENE", "planet_move_speed" ) )
	, shaderManager( shaderManager )
	, options( options )
	, textureManager( textureManager )
	, shadowVolume( shadowVolume )
	, waterFacade( shaderManager.get( SHADER_WATER ),
				   shaderManager.get( SHADER_WATER_CULLING ),
				   shaderManager.get( SHADER_WATER_NORMALS ) )
	, hillsFacade( shaderManager.get( SHADER_HILLS ),
				   shaderManager.get( SHADER_HILLS_CULLING ),
				   shaderManager.get( SHADER_HILLS_NORMALS ),
				   waterFacade.getMap() )
	, shoreFacade( shaderManager.get( SHADER_SHORE ),
				   shaderManager.get( SHADER_SHORE_NORMALS ),
				   waterFacade.getMap() )
	, buildableFacade( shaderManager.get( SHADER_BUILDABLE ),
					   shaderManager.get( SHADER_SELECTED ) )
	, plantsFacade( shaderManager.get( SHADER_MODELS_PHONG ),
					shaderManager.get( SHADER_MODELS_GOURAUD ) )
	, skyboxFacade( shaderManager.get( SHADER_SKYBOX ) )
	, theSunFacade( shaderManager.get( SHADER_SUN ), screenResolution )
	, underwaterFacade( shaderManager.get( SHADER_UNDERWATER ) )
	, landFacade( shaderManager.get( SHADER_LAND ) )
	, lensFlareFacade( shaderManager.get( SHADER_LENS_FLARE ), textureManager.getLoader(), screenResolution )
	, skysphereFacade( shaderManager.get( SHADER_SKYSPHERE ) )
{}

/**
* @brief prepares subsystems facades
*/
void Scene::setup()
{
	waterFacade.setup();
	hillsFacade.setup();
	shoreFacade.setup();
	landFacade.setup( shoreFacade.getMap() );
	waterFacade.setupConsiderTerrain( landFacade.getMap() );
	buildableFacade.setup( landFacade.getMap(), hillsFacade.getMap() );
	plantsFacade.setup( landFacade.getMap(), hillsFacade.getMap(), hillsFacade.getNormalMap() );
	textureManager.createUnderwaterReliefTexture( waterFacade.getMap() );
}

/**
* @brief explicitly reinitializes some terrain maps and prepares subsystems again
* @todo get rid of ugly castings (then probably this function will lose any reason to exist)
*/
void Scene::recreate()
{
	Generator::initializeMap( const_cast<map2D_f &>( landFacade.getMap() ) );
	Generator::initializeMap( const_cast<map2D_f &>( waterFacade.getMap() ) );
	Generator::initializeMap( const_cast<map2D_f &>( hillsFacade.getMap() ) );
	setup();
}

/**
* @brief delegates partial reinitialization to subsystems to synchronize them with loaded data
*/
void Scene::load()
{
	hillsFacade.recreateTilesAndBufferData();
	shoreFacade.setup();
	landFacade.setup( shoreFacade.getMap() );
	waterFacade.setupConsiderTerrain( landFacade.getMap() );
	buildableFacade.setup( landFacade.getMap(), hillsFacade.getMap() );
	textureManager.createUnderwaterReliefTexture( waterFacade.getMap() );
	plantsFacade.reinitializeModelRenderChunks( landFacade.getMap(), hillsFacade.getMap() );
}

/**
* @brief handles serialization process for susbsytems whose data is necessary to save
* @param output file stream to write data to
* @note it is programmer's responsibility to keep susbystems ordering matched for both save/load processes
* @see deserialize
*/
void Scene::serialize( std::ofstream & output )
{
	landFacade.serialize( output );
	Logger::log( "land serialized successfully\n" );
	hillsFacade.serialize( output );
	Logger::log( "hills serialized successfully\n" );
	waterFacade.serialize( output );
	Logger::log( "water serialized successfully\n" );
	plantsFacade.serialize( output );
	Logger::log( "plants serialized successfully\n" );
	theSunFacade.serialize( output );
	Logger::log( "the Sun serialized successfully\n" );
}

/**
* @brief handles deserialization process for subsystems whose data is necessary to load
* @param input file stream to read data from
* @note it is programmer's responsibility to keep susbystems ordering matched for both save/load processes
* @see serialize
*/
void Scene::deserialize( std::ifstream & input )
{
	landFacade.deserialize( input );
	Logger::log( "land deserialized successfully\n" );
	hillsFacade.deserialize( input );
	Logger::log( "hills deserialized successfully\n" );
	waterFacade.deserialize( input );
	Logger::log( "water deserialized successfully\n" );
	plantsFacade.deserialize( input );
	Logger::log( "plants deserialized successfully\n" );
	theSunFacade.deserialize( input );
	Logger::log( "the Sun deserialized successfully\n" );
}

/**
* @brief handles plain onscreen rendering of the scene objects
* @param projectionView 'projection * view' matrix
* @param ambienceProjectionView auxiliary 'projection * view' matrix used for ambience-like objects (skybox, sun etc.)
* @param viewFrustum camera's view frustum
* @param cullingViewFrustum auxiliary view frustum, used for hills frustum culling routine
* @param camera player's camera
* @param mouseInput mouse input manager
*/
void Scene::drawWorld( const glm::mat4 & projectionView,
					   const glm::mat4 & ambienceProjectionView,
					   const Frustum & viewFrustum,
					   const Frustum & cullingViewFrustum,
					   const Camera & camera,
					   MouseInputManager & mouseInput )
{
	glm::vec3 viewPosition = camera.getPosition();
	const glm::vec3 & lightDir = theSunFacade.getLightDir();
	const std::array<glm::mat4, NUM_SHADOW_LAYERS> & lightSpaceMatrices = shadowVolume.getLightSpaceMatrices();
	bool useShadows = options[OPT_USE_SHADOWS];
	bool isDebugRender = options[OPT_DEBUG_RENDER];

	if( options[OPT_DRAW_HILLS] )
	{
		hillsFacade.draw( lightDir,
						  lightSpaceMatrices,
						  projectionView,
						  viewPosition,
						  camera.getViewAcceleration(),
						  cullingViewFrustum,
						  options[OPT_HILLS_CULLING],
						  useShadows,
						  isDebugRender );
	}	

	if( options[OPT_DRAW_LAND] )
	{
		landFacade.draw( lightDir, lightSpaceMatrices, projectionView, useShadows );
	}

	underwaterFacade.draw( lightDir, projectionView, useShadows );

	shoreFacade.draw( lightDir, lightSpaceMatrices, projectionView, useShadows, isDebugRender, false, false );

	//render everything that requires blending

	RendererState::enableState( GL_BLEND );

	if( options[OPT_DRAW_BUILDABLE] )
	{
		buildableFacade.drawBuildable( projectionView );
	}

	if( options[OPT_SHOW_CURSOR] )
	{
		mouseInput.updateCursorMappingCoordinates( landFacade.getMap(), hillsFacade.getMap(), buildableFacade.getMap() );
		buildableFacade.drawSelected( projectionView, mouseInput );
	}

	RendererState::setAmbienceRenderingState( true );
	skysphereFacade.draw( theSunFacade.getRotationTransform(), ambienceProjectionView, lightDir );
	theSunFacade.draw( ambienceProjectionView, true, false );
	skyboxFacade.draw( ambienceProjectionView, viewPosition, lightDir );
	RendererState::setAmbienceRenderingState( false );

	if( options[OPT_DRAW_WATER] )
	{
		waterFacade.draw( lightDir, lightSpaceMatrices, projectionView, viewPosition, viewFrustum, options[OPT_WATER_CULLING], isDebugRender );
	}

	if( options[OPT_DRAW_TREES] )
	{
		plantsFacade.draw( lightDir,
						   lightSpaceMatrices,
						   projectionView,
						   viewPosition,
						   options[OPT_MODELS_PHONG_SHADING],
						   useShadows,
						   options[OPT_MODELS_LAND_BLENDING] );
	}

	float flareBrightness = theSunFacade.getSunFlareBrightness( options[OPT_USE_MULTISAMPLING], viewPosition.y );
	if( flareBrightness > 0 )
	{
		lensFlareFacade.draw( theSunFacade.getPosition(), ambienceProjectionView, flareBrightness );
	}

	RendererState::disableState( GL_BLEND );
}

/**
* @brief handles offscreen rendering of the world parts to depthmap (from the sun point of view)
* @param grassCastShadow defines whether grass models would be rendered to depthmap
*/
void Scene::drawWorldDepthmap( bool grassCastShadow )
{
	const std::array<glm::mat4, NUM_SHADOW_LAYERS> & lightSpaceMatrices = shadowVolume.getLightSpaceMatrices();
	glClear( GL_DEPTH_BUFFER_BIT );

	//must disable multisampling for this rendering mode
	RendererState::disableState( GL_MULTISAMPLE );

	/** @todo smells like it is code duplicate, may be move this part to shader manager as separate function */
	shaderManager.get( SHADER_SHADOW_TERRAIN ).use();
	if( options[OPT_DRAW_HILLS] )
	{
		shaderManager.get( SHADER_SHADOW_TERRAIN ).setMat4( "u_lightSpaceMatrix[0]", lightSpaceMatrices[0] );
		shaderManager.get( SHADER_SHADOW_TERRAIN ).setMat4( "u_lightSpaceMatrix[1]", lightSpaceMatrices[1] );
		shaderManager.get( SHADER_SHADOW_TERRAIN ).setMat4( "u_lightSpaceMatrix[2]", lightSpaceMatrices[2] );
		shaderManager.get( SHADER_SHADOW_TERRAIN ).setInt( "u_terrainType", 0 );
		hillsFacade.drawDepthmap();
	}	

	shaderManager.get( SHADER_SHADOW_TERRAIN ).setInt( "u_terrainType", 1 );
	shoreFacade.drawDepthmap();

	if( options[OPT_DRAW_TREES] )
	{
		shaderManager.get( SHADER_SHADOW_MODELS ).use();
		shaderManager.get( SHADER_SHADOW_MODELS ).setMat4( "u_lightSpaceMatrix[0]", lightSpaceMatrices[0] );
		shaderManager.get( SHADER_SHADOW_MODELS ).setMat4( "u_lightSpaceMatrix[1]", lightSpaceMatrices[1] );
		shaderManager.get( SHADER_SHADOW_MODELS ).setMat4( "u_lightSpaceMatrix[2]", lightSpaceMatrices[2] );
		plantsFacade.drawDepthmap( grassCastShadow );
	}

	RendererState::enableState( GL_MULTISAMPLE );
}

/**
* @brief handles offscreen rendering of the world reflection image from the water surface point of view
* @param projectionView 'projection * view' matrix
* @param ambienceProjectionView auxiliary 'projection * view' matrix used for ambience-like objects (i.e. skyboxes)
* @param cullingViewFrustum auxiliary view frustum, used for hills frustum culling routine
* @param camera player's camera
*/
void Scene::drawWorldReflection( const glm::mat4 & projectionView,
								 const glm::mat4 & ambienceProjectionView,
								 const Frustum & cullingViewFrustum,
								 const Camera & camera )
{
	glm::vec3 viewPosition = camera.getPosition();
	const glm::vec3 & lightDir = theSunFacade.getLightDir();
	const std::array<glm::mat4, NUM_SHADOW_LAYERS> & lightSpaceMatrices = shadowVolume.getLightSpaceMatrices();
	viewPosition.y *= -1;

	if( options[OPT_DRAW_HILLS] )
	{
		hillsFacade.draw( lightDir,
						  lightSpaceMatrices,
						  projectionView,
						  viewPosition,
						  camera.getViewAcceleration(),
						  cullingViewFrustum,
						  options[OPT_HILLS_CULLING],
						  options[OPT_USE_SHADOWS],
						  false );
	}	

	shoreFacade.draw( lightDir, lightSpaceMatrices, projectionView, false, false, true, false );

	RendererState::enableState( GL_BLEND );

	if( options[OPT_DRAW_TREES] )
	{
		plantsFacade.draw( lightDir, lightSpaceMatrices, projectionView, viewPosition, false, false, false, true );
	}

	RendererState::setAmbienceRenderingState( true );
	skysphereFacade.draw( theSunFacade.getRotationTransform(), ambienceProjectionView, lightDir );
	skyboxFacade.draw( ambienceProjectionView, viewPosition, lightDir );
	RendererState::setAmbienceRenderingState( false );

	RendererState::disableState( GL_BLEND );
}

/**
* @brief handles offscreen rendering of the world refraction image from the water surface point of view
* @param projectionView 'projection * view' matrix
*/
void Scene::drawWorldRefraction( const glm::mat4 & projectionView )
{
	const glm::vec3 & lightDir = theSunFacade.getLightDir();
	const std::array<glm::mat4, NUM_SHADOW_LAYERS> & lightSpaceMatrices = shadowVolume.getLightSpaceMatrices();

	underwaterFacade.draw( lightDir, projectionView, options[OPT_USE_SHADOWS] );

	shoreFacade.draw( lightDir, lightSpaceMatrices, projectionView, false, false, false, true );
}

WaterFacade & Scene::getWaterFacade() noexcept
{
	return waterFacade;
}

HillsFacade & Scene::getHillsFacade() noexcept
{
	return hillsFacade;
}

PlantsFacade & Scene::getPlantsFacade() noexcept
{
	return plantsFacade;
}

TheSunFacade & Scene::getSunFacade() noexcept
{
	return theSunFacade;
}

SkysphereFacade & Scene::getSkysphereFacade() noexcept
{
	return skysphereFacade;
}

LandFacade & Scene::getLandFacade() noexcept
{
	return landFacade;
}
