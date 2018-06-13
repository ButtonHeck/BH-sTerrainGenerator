#version 450

layout (location = 0) in vec3 i_pos;
layout (location = 1) in vec2 i_texCoords;
layout (location = 2) in vec3 i_normal;

uniform mat4 u_projectionView;
uniform vec3 u_lightDir;
uniform vec3 u_viewPosition;

out vec2  v_TexCoords;
out vec3  v_SkyboxCoords;
out float v_DiffuseComponent;
out float v_SpecularComponent;

void main()
{
    gl_Position = u_projectionView * vec4(i_pos, 1.0);
    v_TexCoords = i_texCoords;

    //diffuse
    v_DiffuseComponent = max(dot(i_normal, u_lightDir), 0.0);

    //specular
    vec3 ViewDir = normalize(u_viewPosition - i_pos);
    vec3 Reflect = reflect(-u_lightDir, i_normal);
    v_SpecularComponent = pow(max(dot(Reflect, ViewDir), 0.0), 32.0) * 3;

    //reflect skybox component
    v_SkyboxCoords = reflect(-ViewDir, normalize(0.5 * i_normal + vec3(0.0, 1.0, 0.0)));
}