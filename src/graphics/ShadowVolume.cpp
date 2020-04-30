/*
 * Copyright 2019 Ilya Malgin
 * ShadowVolume.cpp
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
 * Purpose: contains definitions for ShadowVolume class
 * @version 0.1.0
 */

#include "ShadowVolume"
#include "TheSunFacade"
#include "Frustum"
#include "SceneSettings"

/**
* @brief updates light direction vectors and light space matrices
* @param frustums a set of frustums defining shadow regions position and dimension parameters
* @param theSunFacade the Sun facade, needed to update light direction vectors
*/
void ShadowVolume::update( const std::array<Frustum, NUM_SHADOW_LAYERS> & frustums, 
						   const TheSunFacade & theSunFacade )
{
	glm::vec3 sunPosition = theSunFacade.getPosition();

	//map sun position coordinates to [-1;1] assuming that the sun move trajectory is a circle
	float sunAbsPositionY = sunPosition.y / HALF_WORLD_WIDTH_F;
	float sunAbsPositionX = sunPosition.x / HALF_WORLD_WIDTH_F;

	lightDirTo = theSunFacade.getLightDir();

	//in case the sun is exactly at the zenith point (or exactly "below" the world) there might be problems with calculations of lightSpace matrices
	//this is a hackfix for such cases
	const float ABS_X_POS_MIN_STABLE_VALUE_LIGHTDIRTO = 0.00005;
	if( lightDirTo.x > 0.0 && lightDirTo.x < ABS_X_POS_MIN_STABLE_VALUE_LIGHTDIRTO )
	{
		lightDirTo.x = ABS_X_POS_MIN_STABLE_VALUE_LIGHTDIRTO;
	}
	else if( lightDirTo.x < 0.0 && lightDirTo.x > -ABS_X_POS_MIN_STABLE_VALUE_LIGHTDIRTO )
	{
		lightDirTo.x = -ABS_X_POS_MIN_STABLE_VALUE_LIGHTDIRTO;
	}

	lightDirRight = glm::normalize( glm::cross( lightDirTo, glm::vec3( 0.0f, 1.0f, 0.0f ) ) );
	/*
	 * for "up" vector no need to normalize it explicitly
	 * as the cross product of 2 orthogonal unit length vectors is already a unit length vector
	 */
	lightDirUp = glm::cross( lightDirRight, lightDirTo );

	for( unsigned int layerIndex = 0; layerIndex < NUM_SHADOW_LAYERS; layerIndex++ )
	{
		updateLightSpaceMatrix( frustums[layerIndex], layerIndex, sunAbsPositionY, sunAbsPositionX );
	}
}

const std::array<glm::mat4, NUM_SHADOW_LAYERS> & ShadowVolume::getLightSpaceMatrices() const noexcept
{
	return lightSpaceMatrices;
}

/**
* @brief applies some math and vector processing to get projectionView matrix paramters for each region
* @param frustum frustum defining shadow region position and dimension parameters
* @param layer layer index of shadow region
* @param sunAbsPositionY normalized value of the sun Y pos (in range [-1;1])
* @param sunAbsPositionX normalized value of the sun X pos (in range [-1;1])
*/
void ShadowVolume::updateLightSpaceMatrix( const Frustum & frustum, 
										   int layer, 
										   float sunAbsPositionY, 
										   float sunAbsPositionX )
{
	//step 1 - calculate bounding boxes bounds
	float boxMaxX = frustum.getMaxCoordX();
	float boxMinX = frustum.getMinCoordX();
	float boxMaxZ = frustum.getMaxCoordZ();
	float boxMinZ = frustum.getMinCoordZ();

	//step 2 - offset bounding box position relative to the sun position
	float offset = glm::mix( SHADOW_BOX_MAX_OFFSET_X, 0.0f, sunAbsPositionY );
	if( lightDirTo.x < 0.0f )
	{
		boxMaxX += offset;
	}
	else
	{
		boxMinX -= offset;
	}

	//limit box bounds with offset map bounds to hide shadow artefacts at map edges
	boxMaxZ = glm::min( HALF_WORLD_HEIGHT_F + SHADOW_BOX_MAP_BORDER_OFFSET, boxMaxZ );
	boxMinZ = glm::max( -HALF_WORLD_HEIGHT_F - SHADOW_BOX_MAP_BORDER_OFFSET, boxMinZ );
	boxMaxX = glm::min( HALF_WORLD_WIDTH_F + SHADOW_BOX_MAP_BORDER_OFFSET, boxMaxX );
	boxMinX = glm::max( -HALF_WORLD_WIDTH_F - SHADOW_BOX_MAP_BORDER_OFFSET, boxMinX );

	//step 3 - calculate box center
	float boxWidth = boxMaxX - boxMinX;
	float boxHalfWidth = boxWidth * 0.5f;
	float boxMidX = ( boxMaxX + boxMinX ) * 0.5f;
	float boxMidZ = ( boxMaxZ + boxMinZ ) * 0.5f;

	//step 4 - calculate light source position
	float angleRad = glm::atan( SHADOW_BOXES_MIN_HEIGHT[layer] / boxHalfWidth );
	float ellipseA = boxHalfWidth / glm::cos( angleRad );
	float ellipseB = SHADOW_BOXES_MIN_HEIGHT[layer] / glm::sin( angleRad );

	glm::vec3 lightSource( boxMidX + sunAbsPositionX * ellipseA,
						   sunAbsPositionY * ellipseB,
						   boxMidZ );

	 //step 5 - calculate frustum borders
	const float RIGHT_SIDE = glm::abs( boxMinZ - boxMidZ );
	const float LEFT_SIDE = -RIGHT_SIDE;
	glm::vec2 lightSourceXY = glm::vec2( lightSource );
	glm::vec2 lightDirToXY = glm::vec2( lightDirTo );
	glm::vec2 lightDirUpXY = glm::vec2( lightDirUp );

	glm::vec2 farPoint( lightDirTo.x > 0.0f ? boxMaxX : boxMinX, 0.0f );
	glm::vec2 fromLStoFarPoint = farPoint - lightSourceXY;
	float fromLStoFarPointLength = glm::length( fromLStoFarPoint );
	glm::vec2 fromLStoFarPointNorm = glm::normalize( fromLStoFarPoint );
	float cosAlpha = glm::dot( fromLStoFarPointNorm, lightDirToXY );
	const float FAR_SIDE = fromLStoFarPointLength * cosAlpha
		+ 4.0f * sunAbsPositionY; //+some offset when the sun is at its zenith

	glm::vec2 nearPoint( lightDirTo.x > 0.0f ? boxMinX : boxMaxX, SHADOW_BOXES_MIN_HEIGHT[layer] );
	glm::vec2 fromLStoNearPoint = nearPoint - lightSourceXY;
	float fromLStoNearPointLength = glm::length( fromLStoNearPoint );
	glm::vec2 fromLStoNearPointNorm = glm::normalize( fromLStoNearPoint );
	cosAlpha = glm::dot( fromLStoNearPointNorm, lightDirToXY );
	const float NEAR_SIDE = fromLStoNearPointLength * cosAlpha;

	glm::vec2 upPoint( lightDirTo.x > 0.0f ? boxMaxX : boxMinX, SHADOW_BOXES_MIN_HEIGHT[layer] );
	glm::vec2 fromLStoUpPoint = upPoint - lightSourceXY;
	float fromLStoUpPointLength = glm::length( fromLStoUpPoint );
	glm::vec2 fromLStoUpPointNorm = glm::normalize( fromLStoUpPoint );
	cosAlpha = glm::dot( fromLStoUpPointNorm, lightDirUpXY );
	const float UP_SIDE = fromLStoUpPointLength * cosAlpha;

	glm::vec2 bottomPoint( lightDirTo.x > 0.0f ? boxMinX : boxMaxX, 0.0f );
	glm::vec2 fromLStoBottomPoint = bottomPoint - lightSourceXY;
	float fromLStoBottomPointLength = glm::length( fromLStoBottomPoint );
	glm::vec2 fromLStoBottomPointNorm = glm::normalize( fromLStoBottomPoint );
	cosAlpha = glm::dot( fromLStoBottomPointNorm, -lightDirUpXY );
	/*
	 * for this we should consider additional offset, because lowest shore fragments are occured below 0.0 level,
	 * thus the offset must be no less than lowest shore fragment. But we need this offset during only dusk and dawn
	 */
	const float BOTTOM_SIDE = -fromLStoBottomPointLength * cosAlpha
		- 5.0f * ( 1.0f - sunAbsPositionY );

	//step 6 - calculate light space matrix
	bool isLightDirPosX = lightDirTo.x > 0.0f;
	glm::mat4 projection = glm::ortho( !isLightDirPosX ? RIGHT_SIDE : LEFT_SIDE,
									   !isLightDirPosX ? LEFT_SIDE : RIGHT_SIDE,
									   !isLightDirPosX ? UP_SIDE : BOTTOM_SIDE,
									   !isLightDirPosX ? BOTTOM_SIDE : UP_SIDE,
									   NEAR_SIDE,
									   FAR_SIDE );
	glm::mat4 view = glm::lookAt( lightSource, lightSource + lightDirTo, glm::vec3( 0.0f, 1.0f, 0.0f ) );
	lightSpaceMatrices[layer] = projection * view;

	//step 7 - update box struct (optional for visualization)
	shadowBoxes[layer].expectedLL = glm::vec2( boxMinX, boxMaxZ );
	shadowBoxes[layer].expectedLR = glm::vec2( boxMaxX, boxMaxZ );
	shadowBoxes[layer].expectedUR = glm::vec2( boxMaxX, boxMinZ );
	shadowBoxes[layer].expectedUL = glm::vec2( boxMinX, boxMinZ );
	shadowBoxes[layer].localLightSource = lightSource;
	shadowBoxes[layer].lsPovNearLL = lightSource + ( NEAR_SIDE * lightDirTo ) + ( LEFT_SIDE * lightDirRight ) + ( BOTTOM_SIDE * lightDirUp );
	shadowBoxes[layer].lsPovNearLR = lightSource + ( NEAR_SIDE * lightDirTo ) + ( RIGHT_SIDE * lightDirRight ) + ( BOTTOM_SIDE * lightDirUp );
	shadowBoxes[layer].lsPovNearUR = lightSource + ( NEAR_SIDE * lightDirTo ) + ( RIGHT_SIDE * lightDirRight ) + ( UP_SIDE * lightDirUp );
	shadowBoxes[layer].lsPovNearUL = lightSource + ( NEAR_SIDE * lightDirTo ) + ( LEFT_SIDE * lightDirRight ) + ( UP_SIDE * lightDirUp );
	shadowBoxes[layer].lsPovFarLL = lightSource + ( FAR_SIDE * lightDirTo ) + ( LEFT_SIDE * lightDirRight ) + ( BOTTOM_SIDE * lightDirUp );
	shadowBoxes[layer].lsPovFarLR = lightSource + ( FAR_SIDE * lightDirTo ) + ( RIGHT_SIDE * lightDirRight ) + ( BOTTOM_SIDE * lightDirUp );
	shadowBoxes[layer].lsPovFarUR = lightSource + ( FAR_SIDE * lightDirTo ) + ( RIGHT_SIDE * lightDirRight ) + ( UP_SIDE * lightDirUp );
	shadowBoxes[layer].lsPovFarUL = lightSource + ( FAR_SIDE * lightDirTo ) + ( LEFT_SIDE * lightDirRight ) + ( UP_SIDE * lightDirUp );
}
