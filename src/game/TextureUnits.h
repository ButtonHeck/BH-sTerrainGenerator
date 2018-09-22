#ifndef TEXTUREUNITS_H
#define TEXTUREUNITS_H

enum TEXTURE_UNITS
{
  FLAT = 0,
  FLAT_2 = 1,
  FLAT_x2 = 2,
  FLAT_2_x2 = 3,
  DIFFUSE_MIX_MAP = 4,
  HILL = 5,
  HILL_2 = 6,
  HILL_SPECULAR = 7,
  SHORE = 8,
  SHORE_2 = 9,
  UNDERWATER_RELIEF = 10,
  SKYBOX = 11,
  FRAME_MS_TEXTURE = 12,
  FRAME_TEXTURE = 13,
  FRAME_HDR_TEXTURE = 14,
  DEPTH_MAP_SUN = 15,
  FONT_TEXTURE = 16,
  TERRAIN_NORMAL = 17,
  WATER_NORMAL = 18,
  WATER_SPECULAR = 19,
  UNDERWATER_DIFFUSE = 20
};

#endif // TEXTUREUNITS_H