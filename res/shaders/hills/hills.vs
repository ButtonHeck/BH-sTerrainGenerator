#version 450

layout (location = 0) in vec3 i_pos;
layout (location = 1) in vec2 i_texCoords;
layout (location = 2) in vec3 i_normal;
layout (location = 3) in vec3 i_tangent;
layout (location = 4) in vec3 i_bitangent;

uniform mat4        u_projectionView;
uniform vec3        u_lightDir;
uniform float       u_mapDimension;
uniform sampler2D   u_diffuse_mix_map;
uniform vec3        u_viewPosition;

out vec3  v_FragPos;
out vec3  v_Normal;
out mat3  v_TNB;
out vec2  v_TexCoords;
out float v_TerrainTypeMix;
out float v_SpecularComponent;

void main()
{
    gl_Position = u_projectionView * vec4(i_pos, 1.0);

    v_FragPos = i_pos;
    v_Normal = i_normal;
    v_TexCoords = i_texCoords;
    v_TNB = mat3(i_tangent, i_normal, i_bitangent);

    float TerrainSplattingRatio = texture(u_diffuse_mix_map, i_pos.xz * u_mapDimension + 0.5).g;
    v_TerrainTypeMix = i_pos.y * (1.0 - TerrainSplattingRatio * 0.75);

    //specular component
    vec3 Reflect = reflect(-u_lightDir, i_normal);
    vec3 ViewDir = normalize(u_viewPosition - i_pos);
    v_SpecularComponent = pow(max(dot(Reflect, ViewDir), 0.0), 64.0);
}
