#version 450

layout (location = 0) in vec4 i_pos;
layout (location = 1) in vec2 i_texCoords;
layout (location = 2) in vec3 i_normal;

uniform mat4      u_projectionView;
uniform vec3      u_lightDir;
uniform float     u_mapDimension;
uniform sampler2D u_diffuse_mix_map;
uniform mat4      u_lightSpaceMatrix;
uniform float     U_UNDERWATER_TILE_YPOS;

out vec3  v_FragPos;
out vec2  v_TexCoords;
out float v_TerrainTypeMix;
out vec3  v_Normal;
out vec3  v_ProjectedCoords;
out float v_AlphaBlend;

const float TERRAIN_TYPE_TRANSITION_RATIO = 1.75;

void main()
{
    gl_Position = u_projectionView * i_pos;
    v_AlphaBlend = (i_pos.y + U_UNDERWATER_TILE_YPOS) * 0.5;

    v_FragPos = i_pos.xyz;
    v_TexCoords = i_texCoords;
    v_Normal = i_normal;

    float TerrainSplattingRatio = texture(u_diffuse_mix_map, i_pos.xz * u_mapDimension + 0.5).g;
    v_TerrainTypeMix = i_pos.y * TERRAIN_TYPE_TRANSITION_RATIO + 1.5 - TerrainSplattingRatio * 0.5;

    vec4 fragPosLightSpace = u_lightSpaceMatrix * i_pos;
    v_ProjectedCoords = fragPosLightSpace.xyz * 0.5 + 0.5; //transform from [-1;1] to [0;1]
}
