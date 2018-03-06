#ifndef SETTINGS
#define SETTINGS
#include <string>

constexpr int SCR_WIDTH = 1440;
constexpr int SCR_HEIGHT = 900;
constexpr int TILES_WIDTH = 384;
constexpr int TILES_HEIGHT = 384;
constexpr int NUM_TILES = TILES_WIDTH * TILES_HEIGHT;
constexpr float WATER_LEVEL = -0.75f;
constexpr int DENY_VALUE = -10;
constexpr int BASE_TERRAIN_CHUNK_SIZE = 32;
const std::string PROJ_PATH("/home/ilya/QtProjects/MyOpenGL/TerrainGenerationTest");

enum HILL_DENSITY
{
  THIN, MEDIUM, DENSE
};

/*FMG - aka Fucking Magic Value
* this one is used for the arrayLength of @baseTerrainHeightOffsets(main.cpp)
* without it added to the arrayLength we could see a tile that goes straight to the
* hell/heaven at the very bottom-right corner...
* because shit happens. If you read this, please, do something.
*/
constexpr int FMG = 1;

#endif // SETTINGS

