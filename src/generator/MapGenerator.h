#ifndef MAPGENERATOR_H
#define MAPGENERATOR_H
#include <vector>
#include <GL/glew.h>
#include "src/model/TerrainTile.h"
#include "src/game/Settings.h"
#include "src/graphics/Shader.h"

class MapGenerator
{
public:
  MapGenerator();
  virtual ~MapGenerator() = default;
  void initializeMap(std::vector<std::vector<float>>& map);
  void resetAllGLBuffers();
  void createTiles(bool flat, bool createOnZeroTiles, std::vector<std::vector<float>>& map, float offsetY);
  std::vector<std::vector<float>>& getMap();
  std::vector<TerrainTile>& getTiles();
  GLuint& getVAO();
  GLuint& getVBO();
  GLuint& getEBO();
  virtual void deleteGLObjects();
protected:
  std::vector<std::vector<float>> map;
  std::vector<TerrainTile> tiles;
  GLuint vao, vbo, ebo;
};

#endif // MAPGENERATOR_H
