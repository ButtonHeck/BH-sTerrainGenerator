#version 450

layout (location = 0) in vec4 i_pos;
layout (location = 1) in vec2 i_texCoords;

out vec2 v_TexCoords;

void main()
{
    v_TexCoords = i_texCoords;
    gl_Position = i_pos;
}
