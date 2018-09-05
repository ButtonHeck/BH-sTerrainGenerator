#version 450

layout (location = 0) in vec4 i_pos;
layout (location = 1) in vec2 i_texCoords;
layout (location = 2) in vec3 i_normal;

uniform mat4      u_projectionView;
uniform vec3      u_lightDir;
uniform sampler2D u_diffuse_mix_map;
uniform float     u_mapDimension;
uniform mat4      u_lightSpaceMatrix;

out vec3  v_FragPos;
out vec2  v_TexCoords;
out float v_PosHeight;
out float v_PositionDiffuseComponent;
out float v_TextureMixRatio;
out vec3  v_Normal;
out vec3  v_ProjectedCoords;

const vec3 NORMAL = vec3(0.0, 1.0, 0.0);
const float POSITION_HEIGHT_MULTIPLIER = 2.1;

void main()
{
    gl_Position = u_projectionView * i_pos;

    v_FragPos = i_pos.xyz;
    v_TexCoords = i_texCoords;
    v_Normal = i_normal;
    v_PosHeight = i_pos.y * POSITION_HEIGHT_MULTIPLIER;
    vec4 fragPosLightSpace = u_lightSpaceMatrix * i_pos;
    v_ProjectedCoords = fragPosLightSpace.xyz * 0.5 + 0.5; //transform from [-1;1] to [0;1]

    v_TextureMixRatio = texture(u_diffuse_mix_map, i_pos.xz * u_mapDimension + 0.5).r;
    float TransitionRatio = clamp(1.0 + v_PosHeight, 0.0, 1.0);

    //diffuse
    v_PositionDiffuseComponent = 1.0 + i_pos.y * 0.4;
}
