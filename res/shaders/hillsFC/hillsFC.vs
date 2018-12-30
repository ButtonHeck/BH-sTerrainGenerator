#version 450

layout (location = 0) in vec3 i_pos;
layout (location = 1) in vec2 i_texCoords;
layout (location = 2) in vec3 i_normal;

out vec3 v_pos;
out vec2 v_texCoords;
out vec3 v_normal;
out int  v_visible;

uniform vec4 u_frustumPlanes[5];
const float TILE_RADIUS = 2;

bool frustumCulling(vec3 position)
{
    for (int i = 0; i < u_frustumPlanes.length(); i++)
    {
        if (dot(u_frustumPlanes[i].xyz, position) <= -u_frustumPlanes[i].w - TILE_RADIUS)
          return false;
    }
  return true;
}

void main()
{
    v_pos = i_pos;
    v_texCoords = i_texCoords;
    v_normal = i_normal;
    if (frustumCulling(i_pos))
        v_visible = 1;
    else
        v_visible = 0;
}
