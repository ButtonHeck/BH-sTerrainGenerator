#include "generator/BaseMapGenerator.h"

BaseMapGenerator::BaseMapGenerator(std::vector<std::vector<float> > &waterMap, std::vector<std::vector<float> > &hillMap)
  :
    MapGenerator(),
    waterMap(waterMap),
    hillMap(hillMap)
{
  randomizer.seed(std::chrono::system_clock::now().time_since_epoch().count());
  squareTiles.reserve(NUM_TILES / CHUNK_SIZE * CHUNK_SIZE);
  initializeMap(chunkMap);
  glCreateVertexArrays(1, &shoreVao);
  glCreateBuffers(1, &shoreVbo);
  glCreateVertexArrays(1, &squareVao);
  glCreateBuffers(1, &squareVbo);
  glCreateBuffers(1, &squareEbo);
  glCreateBuffers(1, &squareModelVbo);
  glCreateVertexArrays(1, &cellVao);
  glCreateBuffers(1, &cellVbo);
  glCreateBuffers(1, &cellEbo);
  glCreateBuffers(1, &cellModelVbo);
  glCreateBuffers(1, &cellMultiDE_I_DIBO);
}

BaseMapGenerator::~BaseMapGenerator()
{
  deleteGLObjects();
}

void BaseMapGenerator::setup()
{
  generateMap();
  for (unsigned int i = 0; i < 5; i++)
    {
      smoothMap();
      float baseWeight = 0.5f - 0.02f * i;
      float evenWeight = (1.0f - baseWeight) / 8.0f;
      float diagonalWeight = evenWeight;
      smoothMapHeightChunks(baseWeight, evenWeight, diagonalWeight);
    }
  randomizeShore();
  compressMap(2.0f);
  correctMapAtEdges();
  splitSquareChunks(CHUNK_SIZE);
  squareTiles.shrink_to_fit();
  removeUnderwaterTiles(SHORE_CLIP_LEVEL);
  tiles.shrink_to_fit();
  splitCellChunks(CHUNK_SIZE);
  splitShoreChunks(CHUNK_SIZE);
  fillShoreBufferData();
  fillSquareBufferData();
  fillCellBufferData();
}

void BaseMapGenerator::fillShoreBufferData()
{
  unsigned int numChunks = shoreChunks.size();
  glBindVertexArray(shoreVao);
  glBindBuffer(GL_ARRAY_BUFFER, shoreVbo);
  GLfloat *vertices = new GLfloat[NUM_TILES * 48];
  size_t bytesToBuffer = 0;
  for (unsigned int i = 0; i < numChunks; i++)
    {
      unsigned int numTiles = shoreChunks[i].getNumInstances();
      for (unsigned int c = 0; c < numTiles; c++)
        {
          TerrainTile& tile = tiles[shoreChunks[i].getInstanceOffset() + c];
          int x = tile.mapX, y = tile.mapY;
          //ll1
          vertices[bytesToBuffer++] =   -1- HALF_WORLD_WIDTH + tile.mapX;
          vertices[bytesToBuffer++] = tile.lowLeft;
          vertices[bytesToBuffer++] = - HALF_WORLD_HEIGHT + tile.mapY;
          vertices[bytesToBuffer++] = 0.0f;
          vertices[bytesToBuffer++] = 0.0f;
          vertices[bytesToBuffer++] = shoreNormals[y][x-1].x;
          vertices[bytesToBuffer++] = shoreNormals[y][x-1].y;
          vertices[bytesToBuffer++] = shoreNormals[y][x-1].z;
          ++shoreVerticesToDraw;
          //lr1
          vertices[bytesToBuffer++] =  - HALF_WORLD_WIDTH + tile.mapX;
          vertices[bytesToBuffer++] =  tile.lowRight;
          vertices[bytesToBuffer++] = - HALF_WORLD_HEIGHT + tile.mapY;
          vertices[bytesToBuffer++] = 1.0f;
          vertices[bytesToBuffer++] = 0.0f;
          vertices[bytesToBuffer++] = shoreNormals[y][x].x;
          vertices[bytesToBuffer++] = shoreNormals[y][x].y;
          vertices[bytesToBuffer++] = shoreNormals[y][x].z;
          ++shoreVerticesToDraw;
          //ur1
          vertices[bytesToBuffer++] = - HALF_WORLD_WIDTH + tile.mapX;
          vertices[bytesToBuffer++] = tile.upperRight;
          vertices[bytesToBuffer++] = -1 - HALF_WORLD_HEIGHT + tile.mapY;
          vertices[bytesToBuffer++] = 1.0f;
          vertices[bytesToBuffer++] = 1.0f;
          vertices[bytesToBuffer++] = shoreNormals[y-1][x].x;
          vertices[bytesToBuffer++] = shoreNormals[y-1][x].y;
          vertices[bytesToBuffer++] = shoreNormals[y-1][x].z;
          ++shoreVerticesToDraw;
          //ur2
          vertices[bytesToBuffer++] = - HALF_WORLD_WIDTH + tile.mapX;
          vertices[bytesToBuffer++] = tile.upperRight;
          vertices[bytesToBuffer++] = -1 - HALF_WORLD_HEIGHT + tile.mapY;
          vertices[bytesToBuffer++] = 1.0f;
          vertices[bytesToBuffer++] = 1.0f;
          vertices[bytesToBuffer++] = shoreNormals[y-1][x].x;
          vertices[bytesToBuffer++] = shoreNormals[y-1][x].y;
          vertices[bytesToBuffer++] = shoreNormals[y-1][x].z;
          ++shoreVerticesToDraw;
          //ul2
          vertices[bytesToBuffer++] = -1 - HALF_WORLD_WIDTH + tile.mapX;
          vertices[bytesToBuffer++] = tile.upperLeft;
          vertices[bytesToBuffer++] = -1 - HALF_WORLD_HEIGHT + tile.mapY;
          vertices[bytesToBuffer++] = 0.0f;
          vertices[bytesToBuffer++] = 1.0f;
          vertices[bytesToBuffer++] = shoreNormals[y-1][x-1].x;
          vertices[bytesToBuffer++] = shoreNormals[y-1][x-1].y;
          vertices[bytesToBuffer++] = shoreNormals[y-1][x-1].z;
          ++shoreVerticesToDraw;
          //ll2
          vertices[bytesToBuffer++] = -1- HALF_WORLD_WIDTH + tile.mapX;
          vertices[bytesToBuffer++] = tile.lowLeft;
          vertices[bytesToBuffer++] = - HALF_WORLD_HEIGHT + tile.mapY;
          vertices[bytesToBuffer++] = 0.0f;
          vertices[bytesToBuffer++] = 0.0f;
          vertices[bytesToBuffer++] = shoreNormals[y][x-1].x;
          vertices[bytesToBuffer++] = shoreNormals[y][x-1].y;
          vertices[bytesToBuffer++] = shoreNormals[y][x-1].z;
          ++shoreVerticesToDraw;
        }
    }
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * bytesToBuffer, vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
  delete[] vertices;
  resetAllGLBuffers();
}

void BaseMapGenerator::fillSquareBufferData()
{
  GLfloat baseChunkInstanceVertices[20] = {
      -1.0f, 0.0f,  1.0f, 0.0f,               0.0f,
       1.0f, 0.0f,  1.0f, (float)CHUNK_SIZE,  0.0f,
       1.0f, 0.0f, -1.0f, (float)CHUNK_SIZE,  (float)CHUNK_SIZE,
      -1.0f, 0.0f, -1.0f, 0.0f,               (float)CHUNK_SIZE
  };
  glBindVertexArray(squareVao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, squareEbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(QUAD_INDICES), QUAD_INDICES, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, squareVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(baseChunkInstanceVertices), baseChunkInstanceVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
  NUM_SQUARE_INSTANCES = squareTiles.size();
  glm::mat4* baseInstanceChunkModels = new glm::mat4[NUM_SQUARE_INSTANCES];
  for (unsigned int i = 0; i < squareTiles.size(); i++)
    {
      glm::mat4 model;
      TerrainTile& tile = squareTiles[i];
      model = glm::translate(model, glm::vec3(- HALF_WORLD_WIDTH + tile.mapX + CHUNK_SIZE / 2, 0.0f, - HALF_WORLD_HEIGHT + tile.mapY + CHUNK_SIZE / 2));
      model = glm::scale(model, glm::vec3(CHUNK_SIZE / 2, 0.0f, CHUNK_SIZE / 2));
      baseInstanceChunkModels[i] = model;
    }
  glBindBuffer(GL_ARRAY_BUFFER, squareModelVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * squareTiles.size(), &baseInstanceChunkModels[0], GL_STATIC_DRAW);
  for (unsigned int i = 0; i < 4; ++i)
    {
      glEnableVertexAttribArray(i+3);
      glVertexAttribPointer(i+3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
      glVertexAttribDivisor(i+3, 1);
    }
  resetAllGLBuffers();
  delete[] baseInstanceChunkModels;
}

void BaseMapGenerator::fillCellBufferData()
{
  GLfloat cellVertices[20] = {
       0.0f, 0.0f,  1.0f, 0.0f,  0.0f,
       1.0f, 0.0f,  1.0f, 1.0f,  0.0f,
       1.0f, 0.0f,  0.0f, 1.0f,  1.0f,
       0.0f, 0.0f,  0.0f, 0.0f,  1.0f
  };
  glBindVertexArray(cellVao);
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, cellMultiDE_I_DIBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cellEbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(QUAD_INDICES), QUAD_INDICES, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, cellVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cellVertices), cellVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
  NUM_CELL_INSTANCES = cellTiles.size();
  glm::mat4* cellInstanceModels = new glm::mat4[NUM_CELL_INSTANCES];
  for (unsigned int i = 0; i < cellTiles.size(); i++)
    {
      glm::mat4 model;
      TerrainTile& tile = cellTiles[i];
      model = glm::translate(model, glm::vec3(- HALF_WORLD_WIDTH + tile.mapX, 0.0f, -HALF_WORLD_HEIGHT + tile.mapY - 1));
      cellInstanceModels[i] = model;
    }

  glBindBuffer(GL_ARRAY_BUFFER, cellModelVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * cellTiles.size(), &cellInstanceModels[0], GL_STATIC_DRAW);
  for (unsigned int i = 0; i < 4; ++i)
    {
      glEnableVertexAttribArray(i+3);
      glVertexAttribPointer(i+3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
      glVertexAttribDivisor(i+3, 1);
    }
  delete[] cellInstanceModels;
  resetAllGLBuffers();
}

void BaseMapGenerator::generateMap()
{
  std::uniform_real_distribution<float> distribution(0.9f, 1.1f);
  for (unsigned int y = 0; y <= WORLD_HEIGHT; y++)
    {
      for (unsigned int x = 0; x <= WORLD_WIDTH; x++)
        {
          map[y][x] = waterMap[y][x] * 1.1f * distribution(randomizer);
        }
    }
}

void BaseMapGenerator::smoothMap()
{
  float waterLevel = WATER_LEVEL + 0.25f;
  //smooth tile below on map
  for (unsigned int y = 1; y < WORLD_HEIGHT + 1; y++)
    {
      for (unsigned int x = 0; x < WORLD_WIDTH + 1; x++)
        {
          if (map[y-1][x] < waterLevel - waterLevel / 4)
            map[y][x] += waterLevel / 2.0f;
        }
    }
  //smooth tile upper on map
  for (unsigned int y = 0; y < WORLD_HEIGHT; y++)
    {
      for (unsigned int x = 0; x < WORLD_WIDTH + 1; x++)
        {
          if (map[y+1][x] < waterLevel - waterLevel / 4)
            map[y][x] += waterLevel / 2.0f;
        }
    }
  //smooth tile left on map
  for (unsigned int y = 0; y < WORLD_HEIGHT + 1; y++)
    {
      for (unsigned int x = 0; x < WORLD_WIDTH; x++)
        {
          if (map[y][x+1] < waterLevel - waterLevel / 4)
            map[y][x] += waterLevel / 2.0f;
        }
    }
  //smooth tile right on map
  for (unsigned int y = 0; y < WORLD_HEIGHT + 1; y++)
    {
      for (unsigned int x = 1; x < WORLD_WIDTH + 1; x++)
        {
          if (map[y][x-1] < waterLevel - waterLevel / 4)
            map[y][x] += waterLevel / 2.0f;
        }
    }
}

void BaseMapGenerator::smoothMapHeightChunks(float baseWeight, float evenWeight, float diagonalWeight)
{
  std::vector<std::vector<float>> shoreMapSmoothed;
  initializeMap(shoreMapSmoothed);
  for (unsigned int y = 1; y < WORLD_HEIGHT; y++)
    {
      for (unsigned int x = 1; x < WORLD_WIDTH; x++)
        {
          if (map[y][x] == 0)
            continue;
          float smoothedHeight =
                map[y][x] * baseWeight
              + map[y-1][x] * evenWeight
              + map[y+1][x] * evenWeight
              + map[y][x-1] * evenWeight
              + map[y][x+1] * evenWeight
              + map[y-1][x-1] * diagonalWeight
              + map[y-1][x+1] * diagonalWeight
              + map[y+1][x-1] * diagonalWeight
              + map[y+1][x+1] * diagonalWeight;
          shoreMapSmoothed[y][x] = smoothedHeight;
        }
    }
  map.assign(shoreMapSmoothed.begin(), shoreMapSmoothed.end());
}

void BaseMapGenerator::randomizeShore()
{
  std::uniform_real_distribution<float> distribution(-0.24f, 0.24f);
  for (unsigned int y = 0; y < WORLD_HEIGHT; y++)
    {
      for (unsigned int x = 0; x < WORLD_WIDTH; x++)
        {
          if (map[y][x] < 0)
            map[y][x] += distribution(randomizer);
        }
    }
}

void BaseMapGenerator::correctMapAtEdges()
{
  //correct top and bottom sides of the map
  for (unsigned int x = 0; x < WORLD_WIDTH; ++x)
    {
      if (waterMap[0][x] != 0)
        map[0][x] = map[1][x];
      if (waterMap[WORLD_HEIGHT][x] != 0)
        map[WORLD_HEIGHT][x] = map[WORLD_HEIGHT - 1][x];
    }
  //correct left and right sides of the map
  for (unsigned int y = 0; y < WORLD_HEIGHT; ++y)
    {
      if (waterMap[y][0] != 0)
        map[y][0] = map[y][1];
      if (waterMap[y][WORLD_WIDTH] != 0)
        map[y][WORLD_WIDTH] = map[y][WORLD_WIDTH - 1];
    }
}

void BaseMapGenerator::compressMap(float ratio)
{
  for (std::vector<float>& row : map)
    {
      for (float& height : row)
        {
          if (height < 0)
            height /= ratio;
        }
    }
}

void BaseMapGenerator::splitSquareChunks(int chunkSize)
{
  squareChunks.clear();
  unsigned int chunkOffset = 0;
  for (int y = 0; y < WORLD_HEIGHT - chunkSize + 1; y += chunkSize)
    {
      for (int x = 0; x < WORLD_WIDTH - chunkSize + 1; x += chunkSize)
        {
          bool emptyChunk = true;
          for (int y1 = y; y1 < y + chunkSize; y1++)
            {
              for (int x1 = x; x1 < x + chunkSize; x1++)
                {
                  if ((map[y1][x1] != 0 && map[y1][x1] != TILE_NO_RENDER_VALUE)
                      || (map[y1+1][x1] != 0 && map[y1+1][x1] != TILE_NO_RENDER_VALUE)
                      || (map[y1+1][x1+1] != 0 && map[y1+1][x1+1] != TILE_NO_RENDER_VALUE)
                      || (map[y1][x1+1] != 0 && map[y1][x1+1] != TILE_NO_RENDER_VALUE))
                    {
                      emptyChunk = false;
                      break;
                    }
                }
              if (!emptyChunk)
                break;
            }
          if (emptyChunk)
            {
              for (int ydel = y + 1; ydel < y + chunkSize; ydel++)
                {
                  for (int xdel = x + 1; xdel < x + chunkSize; xdel++)
                    {
                      map[ydel][xdel] = 0;
                      chunkMap[ydel][xdel] = CHUNK_NO_RENDER_VALUE;
                    }
                }
              squareTiles.emplace_back(x, y, 0, 0, 0, 0, false);
              squareChunks.emplace_back(x, x + chunkSize, y, y + chunkSize, chunkOffset, 1);
              ++chunkOffset;
            }
        }
    }
}

void BaseMapGenerator::splitShoreChunks(int chunkSize)
{
  tiles.clear();
  shoreChunks.clear();
  shoreNormals.clear();
  shoreNormals.reserve(WORLD_HEIGHT + 1);
  for (size_t row = 0; row < WORLD_HEIGHT + 1; row++)
    {
      glm::vec3 emptyNormal(0.0f);
      std::vector<glm::vec3> emptyVec(WORLD_WIDTH + 1, emptyNormal);
      shoreNormals.emplace_back(emptyVec);
    }
  unsigned int offset = 0;
  for (int y = 0; y < WORLD_HEIGHT - chunkSize + 1; y += chunkSize)
    {
      for (int x = 0; x < WORLD_WIDTH - chunkSize + 1; x += chunkSize)
        {
          unsigned int instances = 0;
          for (int y1 = y + 1; y1 < y + chunkSize + 1; y1++)
            {
              for (int x1 = x + 1; x1 < x + chunkSize + 1; x1++)
                {
                  if (map[y1][x1] == TILE_NO_RENDER_VALUE)
                    continue;
                  bool toCreate = map[y1][x1] != 0 || map[y1-1][x1] != 0 || map[y1][x1-1] != 0 || map[y1-1][x1-1] != 0;
                    if (toCreate)
                      {
                        float ll = map[y1][x1-1];
                        if (ll == TILE_NO_RENDER_VALUE)
                          ll = map[y1][x1];
                        float lr = map[y1][x1];
                        float ur = map[y1-1][x1];
                        if (ur == TILE_NO_RENDER_VALUE)
                          ur = map[y1][x1];
                        float ul = map[y1-1][x1-1];
                        if (ul == TILE_NO_RENDER_VALUE)
                          ul = map[y1][x1];
                        tiles.emplace_back(x1, y1, ll, lr, ur, ul);
                        instances++;
                      }
                }
            }
          if (instances != 0)
            shoreChunks.emplace_back(x, x + chunkSize, y, y + chunkSize, offset, instances);
          offset += instances;
        }
    }
  //calculate normals for smooth shading
  //nX where X is a relative direction for n0 to nearby polygon expressed as a clockface number
  for (unsigned int y = 1; y < map.size() - 1; y++)
    {
      for (unsigned int x = 1; x < map[0].size() - 1; x++)
        {
          glm::vec3 n0 = glm::normalize(glm::vec3(map[y][x-1] - map[y][x], 1, map[y-1][x] - map[y][x]));
          glm::vec3 n3 = glm::normalize(glm::vec3(map[y][x] - map[y][x+1], 1, map[y-1][x+1] - map[y][x+1]));
          glm::vec3 n6 = glm::normalize(glm::vec3(map[y+1][x-1] - map[y+1][x], 1, map[y][x] - map[y+1][x]));
          glm::vec3 n1= glm::normalize(glm::vec3(map[y-1][x] - map[y-1][x+1], 1, map[y-1][x] - map[y][x]));
          glm::vec3 n4 = glm::normalize(glm::vec3(map[y][x] - map[y][x+1], 1, map[y][x] - map[y+1][x]));
          glm::vec3 n9 = glm::normalize(glm::vec3(map[y][x-1] - map[y][x], 1, map[y][x-1] - map[y+1][x-1]));
          glm::vec3 avgNormal = glm::normalize(n0 + n1 + n3 + n4 + n6 + n9);
          shoreNormals[y][x] = avgNormal;
        }
    }
}

void BaseMapGenerator::removeUnderwaterTiles(float thresholdValue)
{
  for (unsigned int y = 1; y < WORLD_HEIGHT; y++)
    {
      for (unsigned int x = 1; x < WORLD_WIDTH; x++)
        {
          if (map[y-1][x-1] < thresholdValue
              && map[y-1][x] < thresholdValue
              && map[y-1][x+1] < thresholdValue
              && map[y][x-1] < thresholdValue
              && map[y][x] < thresholdValue
              && map[y][x+1] < thresholdValue
              && map[y+1][x-1] < thresholdValue
              && map[y+1][x] < thresholdValue
              && map[y+1][x+1] < thresholdValue)
            map[y][x] = TILE_NO_RENDER_VALUE;
        }
    }
}

void BaseMapGenerator::splitCellChunks(int chunkSize)
{
  cellChunks.clear();
  unsigned int offset = 0;
  for (int y = 0; y < WORLD_HEIGHT - chunkSize + 1; y += chunkSize)
    {
      for (int x = 0; x < WORLD_WIDTH - chunkSize + 1; x += chunkSize)
        {
          unsigned int instances = 0;
          for (int y1 = y+1; y1 < y + chunkSize + 1; y1++)
            {
              for (int x1 = x; x1 < x + chunkSize; x1++)
                {
                  if ((map[y1][x1] == 0
                       && map[y1-1][x1] == 0
                       && map[y1-1][x1+1] == 0
                       && map[y1][x1+1] == 0)
                      &&
                      (chunkMap[y1][x1] != CHUNK_NO_RENDER_VALUE
                       && chunkMap[y1-1][x1] != CHUNK_NO_RENDER_VALUE
                       && chunkMap[y1-1][x1+1] != CHUNK_NO_RENDER_VALUE
                       && chunkMap[y1][x1+1] != CHUNK_NO_RENDER_VALUE))
                    {
                      cellTiles.emplace_back(x1, y1, 0, 0, 0, 0, false);
                      instances++;
                    }
                }
            }
          if (instances != 0)
            cellChunks.emplace_back(x, x+chunkSize, y, y+chunkSize, offset, instances);
          offset += instances;
        }
    }
}

void BaseMapGenerator::deleteGLObjects()
{
  MapGenerator::deleteGLObjects();
  glDeleteVertexArrays(1, &squareVao);
  glDeleteBuffers(1, &squareVbo);
  glDeleteBuffers(1, &squareEbo);
  glDeleteBuffers(1, &squareModelVbo);
  glDeleteVertexArrays(1, &cellVao);
  glDeleteBuffers(1, &cellVbo);
  glDeleteBuffers(1, &cellEbo);
  glDeleteBuffers(1, &cellModelVbo);
  glDeleteBuffers(1, &cellMultiDE_I_DIBO);
  glDeleteVertexArrays(1, &shoreVao);
  glDeleteBuffers(1, &shoreVbo);
}

std::vector<TerrainChunk> &BaseMapGenerator::getShoreChunks()
{
  return shoreChunks;
}

GLuint &BaseMapGenerator::getShoreVao()
{
  return shoreVao;
}

std::vector<TerrainChunk> &BaseMapGenerator::getSquareChunks()
{
  return squareChunks;
}

std::vector<TerrainChunk> &BaseMapGenerator::getCellChunks()
{
  return cellChunks;
}

std::vector<TerrainTile> &BaseMapGenerator::getSquareTiles()
{
  return squareTiles;
}

std::vector<TerrainTile> &BaseMapGenerator::getCellTiles()
{
  return cellTiles;
}

GLuint &BaseMapGenerator::getSquareVAO()
{
  return squareVao;
}

GLuint &BaseMapGenerator::getCellVAO()
{
  return cellVao;
}

GLuint &BaseMapGenerator::getCellDIBO()
{
  return cellMultiDE_I_DIBO;
}

std::vector<std::vector<float> > &BaseMapGenerator::getChunkMap()
{
  return chunkMap;
}

int BaseMapGenerator::getNumSquareInstances()
{
  return NUM_SQUARE_INSTANCES;
}

int BaseMapGenerator::getNumCellInstances()
{
  return NUM_CELL_INSTANCES;
}

void BaseMapGenerator::deserialize(std::ifstream &input)
{
  MapGenerator::deserialize(input);
  for (std::vector<float>& row : chunkMap)
    {
      for (float& value : row)
        {
          value = 0;
        }
    }
}

size_t BaseMapGenerator::getShoreVerticesToDraw() const
{
  return shoreVerticesToDraw;
}
