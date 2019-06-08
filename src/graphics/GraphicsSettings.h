#pragma once

#include <glm/vec4.hpp>

constexpr float NEAR_PLANE = 0.3f;
constexpr float FAR_PLANE = 500.0f;
constexpr bool HDR_ENABLED = true;
constexpr float HDR_EXPOSURE = 2.2f;
constexpr unsigned int MULTISAMPLES = 4;
constexpr float ANISOTROPY = 2.0f;
constexpr float DOF_DISTANCE_LINEAR = 0.17f;

//constants for lighting calculations
const glm::vec4 NIGHT_SKY_COLOR(0.02f, 0.0112f, 0.0088f, 1.0f);
const glm::vec4 DAY_SKY_COLOR(0.75f, 0.38f, 0.3f, 1.0f);
constexpr int DEPTH_MAP_TEXTURE_WIDTH = 4096;
constexpr int DEPTH_MAP_TEXTURE_HEIGHT = 4096;
constexpr int FRAME_WATER_REFLECTION_WIDTH = 400;
constexpr int FRAME_WATER_REFLECTION_HEIGHT = 300;
constexpr int FRAME_WATER_REFRACTION_WIDTH = FRAME_WATER_REFLECTION_WIDTH;
constexpr int FRAME_WATER_REFRACTION_HEIGHT = FRAME_WATER_REFLECTION_HEIGHT;
constexpr float SHADOW_DISTANCE_LAYER1 = 20.0f;
constexpr float SHADOW_DISTANCE_LAYER2 = 60.0f;
constexpr unsigned int NUM_SHADOW_LAYERS = 3;
