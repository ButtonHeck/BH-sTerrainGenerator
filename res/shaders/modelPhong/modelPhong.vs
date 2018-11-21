#version 450

layout (location = 0) in vec4 i_pos;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_texCoords;
layout (location = 3) in vec3 i_tangent;
layout (location = 4) in vec3 i_bitangent;
layout (location = 5) in mat4 i_model;

uniform mat4  u_projectionView;
uniform mat4  u_lightSpaceMatrix;
uniform bool  u_isGrass;
uniform float u_grassPosDistrubution;
uniform vec3  u_viewPosition;

out vec2  v_TexCoords;
out vec3  v_Normal;
out vec3  v_ProjectedCoords;
out vec3  v_FragPos;

@include modelGrassAnimationAndBlending.ivs

void main()
{
    vec4 ModelWorldPosition = i_model * i_pos;
    v_FragPos = vec3(ModelWorldPosition);

    float distanceToObject = distance(u_viewPosition, v_FragPos);
    float normalDistributionImitation = 1.0;
    v_AlphaValue = 4.0;

    if (distanceToObject < 30.0)
        ext_animateAndBlend(ModelWorldPosition, normalDistributionImitation);

    gl_Position = u_projectionView * ModelWorldPosition;
    v_TexCoords = i_texCoords;
    v_Normal = vec3(i_model * vec4(i_normal, 0));
    v_Normal.y *= normalDistributionImitation; //as far as we render no grass this stays 1.0
    vec4 fragPosLightSpace = u_lightSpaceMatrix * ModelWorldPosition;
    v_ProjectedCoords = fragPosLightSpace.xyz * 0.5 + 0.5; //transform from [-1;1] to [0;1]
}
