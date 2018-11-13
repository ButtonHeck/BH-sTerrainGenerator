#include "game/world/terrain/land/LandGenerator.h"

LandGenerator::LandGenerator()
  :
    Generator(),
    cellBuffers(VAO | VBO | INSTANCE_VBO | EBO | DIBO)
{
  randomizer.seed(std::chrono::system_clock::now().time_since_epoch().count());
  basicGLBuffers.add(INSTANCE_VBO);
}

void LandGenerator::setup(const map2D_f &shoreMap)
{
  initializeMap(chunkMap);
  generateMap(shoreMap);
  splitChunks(CHUNK_SIZE);
  tiles.shrink_to_fit();
  splitCellChunks(CHUNK_SIZE);
  fillBufferData();
  fillCellBufferData();
}

void LandGenerator::generateMap(const map2D_f &shoreMap)
{
  for (unsigned int y = 0; y <= WORLD_HEIGHT; y++)
    {
      for (unsigned int x = 0; x <= WORLD_WIDTH; x++)
        map[y][x] = shoreMap[y][x];
    }
}

void LandGenerator::splitChunks(int chunkSize)
{
  chunks.clear();
  tiles.clear();
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
                  if (map[y1][x1] != 0 || map[y1+1][x1] != 0 || map[y1+1][x1+1] != 0 || map[y1][x1+1] != 0)
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
              tiles.emplace_back(x, y, 0, 0, 0, 0, false);
              chunks.emplace_back(x, x + chunkSize, y, y + chunkSize, chunkOffset, 1);
              ++chunkOffset;
            }
        }
    }
}

void LandGenerator::splitCellChunks(int chunkSize)
{
  cellTiles.clear();
  cellChunks.clear();
  unsigned int chunkOffset = 0;
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
            cellChunks.emplace_back(x, x+chunkSize, y, y+chunkSize, chunkOffset, instances);
          chunkOffset += instances;
        }
    }
}

void LandGenerator::fillBufferData()
{
  GLfloat chunkVertices[20] = {
      -1.0f, 0.0f,  1.0f, 0.0f,               0.0f,
       1.0f, 0.0f,  1.0f, (float)CHUNK_SIZE,  0.0f,
       1.0f, 0.0f, -1.0f, (float)CHUNK_SIZE,  (float)CHUNK_SIZE,
      -1.0f, 0.0f, -1.0f, 0.0f,               (float)CHUNK_SIZE
  };
  basicGLBuffers.bind(VAO);
  bufferData(basicGLBuffers.get(EBO), basicGLBuffers.get(VBO), chunkVertices, 20);
  setupGLBufferAttributes();
  glm::mat4 baseInstanceChunkModels[tiles.size()];
  for (unsigned int i = 0; i < tiles.size(); i++)
    {
      glm::mat4 model;
      TerrainTile& tile = tiles[i];
      model = glm::translate(model, glm::vec3(- HALF_WORLD_WIDTH + tile.mapX + CHUNK_SIZE / 2, 0.0f, - HALF_WORLD_HEIGHT + tile.mapY + CHUNK_SIZE / 2));
      model = glm::scale(model, glm::vec3(CHUNK_SIZE / 2, 0.0f, CHUNK_SIZE / 2));
      baseInstanceChunkModels[i] = model;
    }
  basicGLBuffers.bind(INSTANCE_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * tiles.size(), &baseInstanceChunkModels[0], GL_STATIC_DRAW);
  setupGLBufferInstancedAttributes();
  resetAllGLBuffers();
}

void LandGenerator::fillCellBufferData()
{
  GLfloat cellVertices[20] = {
       0.0f, 0.0f,  1.0f, 0.0f,  0.0f,
       1.0f, 0.0f,  1.0f, 1.0f,  0.0f,
       1.0f, 0.0f,  0.0f, 1.0f,  1.0f,
       0.0f, 0.0f,  0.0f, 0.0f,  1.0f
  };
  cellBuffers.bind(VAO);
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, cellBuffers.get(DIBO));
  bufferData(cellBuffers.get(EBO), cellBuffers.get(VBO), cellVertices, 20);
  setupGLBufferAttributes();
  glm::mat4 cellInstanceModels[cellTiles.size()];
  for (unsigned int i = 0; i < cellTiles.size(); i++)
    {
      glm::mat4 model;
      TerrainTile& tile = cellTiles[i];
      model = glm::translate(model, glm::vec3(- HALF_WORLD_WIDTH + tile.mapX, 0.0f, -HALF_WORLD_HEIGHT + tile.mapY - 1));
      cellInstanceModels[i] = model;
    }
  cellBuffers.bind(INSTANCE_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * cellTiles.size(), &cellInstanceModels[0], GL_STATIC_DRAW);
  setupGLBufferInstancedAttributes();
  resetAllGLBuffers();
}

void LandGenerator::bufferData(GLuint &ebo, GLuint &vbo, GLfloat* buffer, size_t size)
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(QUAD_INDICES), QUAD_INDICES, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(buffer) * size, buffer, GL_STATIC_DRAW);
}

void LandGenerator::setupGLBufferAttributes()
{
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
}

void LandGenerator::setupGLBufferInstancedAttributes()
{
  for (unsigned int i = 0; i < 4; ++i)
    {
      glEnableVertexAttribArray(i+3);
      glVertexAttribPointer(i+3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
      glVertexAttribDivisor(i+3, 1);
    }
}