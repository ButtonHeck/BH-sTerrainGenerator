#version 450

layout (location = 0) in vec4 i_pos;

uniform mat4 u_lightSpaceMatrix;

void main()
{
    gl_Position = u_lightSpaceMatrix * i_pos;
}
