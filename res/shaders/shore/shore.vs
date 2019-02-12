#version 450

layout (location = 0) in vec4 i_pos;
layout (location = 1) in vec2 i_texCoords;
layout (location = 2) in vec3 i_normal;

uniform mat4      u_projectionView;
uniform float     u_mapDimensionReciprocal;
uniform sampler2D u_diffuseMixMap;
uniform float     U_UNDERWATER_TILE_YPOS;
uniform float     U_WATER_LEVEL;
uniform bool      u_useClipDistanceReflection;
uniform bool      u_useClipDistanceRefraction;

const vec4  CLIP_PLANE_REFLECTION = vec4(0.0, 1.0, 0.0, U_WATER_LEVEL);
const vec4  CLIP_PLANE_REFRACTION = vec4(0.0, -1.0, 0.0, -U_WATER_LEVEL);

out vec3  v_FragPos;
out vec2  v_TexCoords;
out float v_TerrainTypeMix;
out vec3  v_Normal;
out float v_AlphaBlend;

const float TERRAIN_TYPE_TRANSITION_RATIO = 1.75;

void main()
{
    gl_Position = u_projectionView * i_pos;
    if (u_useClipDistanceReflection)
        gl_ClipDistance[0] = dot(CLIP_PLANE_REFLECTION, i_pos);
    if (u_useClipDistanceRefraction)
        gl_ClipDistance[0] = dot(CLIP_PLANE_REFRACTION, i_pos);

    v_FragPos = i_pos.xyz;
    v_TexCoords = i_texCoords;
    v_Normal = i_normal;

    float TerrainSplattingRatio = texture(u_diffuseMixMap, i_pos.xz * u_mapDimensionReciprocal + 0.5).g;
    v_TerrainTypeMix = i_pos.y * TERRAIN_TYPE_TRANSITION_RATIO + 1.5 - TerrainSplattingRatio * 0.5;
    v_AlphaBlend = (i_pos.y + U_UNDERWATER_TILE_YPOS) * 0.5 - TerrainSplattingRatio * 0.25;
}
