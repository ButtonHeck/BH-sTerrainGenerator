#include "generator/BuildableMapGenerator.h"

BuildableMapGenerator::BuildableMapGenerator(std::shared_ptr<BaseMapGenerator>& baseMapGenerator,
                                             std::shared_ptr<HillsMapGenerator>& hillsGenerator)
  :
    MapGenerator(),
    baseMapGenerator(baseMapGenerator),
    hillsGenerator(hillsGenerator)
{
  glCreateBuffers(1, &modelVbo);
  glCreateVertexArrays(1, &selectedVAO);
  glCreateBuffers(1, &selectedVBO);
  glCreateBuffers(1, &selectedEBO);
}

BuildableMapGenerator::~BuildableMapGenerator()
{
  deleteGLObjects();
}

void BuildableMapGenerator::setup(std::shared_ptr<BaseMapGenerator> &baseMapGenerator,
                                  std::shared_ptr<HillsMapGenerator> &hillsGenerator)
{
  this->baseMapGenerator = baseMapGenerator;
  this->hillsGenerator = hillsGenerator;
  auto& baseMap = this->baseMapGenerator->getMap();
  auto& hillMap = this->hillsGenerator->getMap();
  for (unsigned int y = 2; y < WORLD_HEIGHT; y++)
    {
      for (unsigned int x = 0; x < WORLD_WIDTH - 1; x++)
        {
          map[y][x] = false;
          if (baseMap[y][x] == 0
              && baseMap[y-1][x] == 0
              && baseMap[y-1][x+1] == 0
              && baseMap[y][x+1] == 0
              && hillMap[y][x] <= -HILLS_OFFSET_Y
              && hillMap[y-1][x] <= -HILLS_OFFSET_Y
              && hillMap[y-1][x+1] <= -HILLS_OFFSET_Y
              && hillMap[y][x+1] <= -HILLS_OFFSET_Y)
            {
              map[y][x] = true;
            }
        }
    }
  createTiles(true, false, map, 0);
  tiles.shrink_to_fit();
  fillBufferData();
}

void BuildableMapGenerator::fillBufferData()
{
  glBindVertexArray(vao);
  constexpr GLfloat CELL_VERTICES[12] = {
       0.05f, 0.01f,  -0.05f,
       0.95f, 0.01f,  -0.05f,
       0.95f, 0.01f,  -0.95f,
       0.05f, 0.01f,  -0.95f
  };
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(QUAD_INDICES), QUAD_INDICES, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(CELL_VERTICES), CELL_VERTICES, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
  num_instances = tiles.size();
  std::unique_ptr<glm::mat4[]> instanceModels(new glm::mat4[num_instances]);
  for (unsigned int i = 0; i < tiles.size(); i++)
    {
      glm::mat4 model;
      TerrainTile& tile = tiles[i];
      model = glm::translate(model, glm::vec3(-HALF_WORLD_WIDTH + tile.mapX, 0.0f, -HALF_WORLD_HEIGHT + tile.mapY));
      instanceModels[i] = model;
    }
  glBindBuffer(GL_ARRAY_BUFFER, modelVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * tiles.size(), &instanceModels[0], GL_STATIC_DRAW);
  for (unsigned int i = 0; i < 4; ++i)
    {
      glEnableVertexAttribArray(i+3);
      glVertexAttribPointer(i+3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
      glVertexAttribDivisor(i+3, 1);
    }
  resetAllGLBuffers();

  glBindVertexArray(selectedVAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, selectedEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(QUAD_INDICES), QUAD_INDICES, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, selectedVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(CELL_VERTICES), CELL_VERTICES, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
  resetAllGLBuffers();
}

GLuint &BuildableMapGenerator::getNumInstances()
{
  return num_instances;
}

GLuint &BuildableMapGenerator::getSelectedTileVAO()
{
  return selectedVAO;
}

void BuildableMapGenerator::deleteGLObjects()
{
  MapGenerator::deleteGLObjects();
  glDeleteVertexArrays(1, &selectedVAO);
  glDeleteBuffers(1, &modelVbo);
  glDeleteBuffers(1, &selectedVBO);
  glDeleteBuffers(1, &selectedEBO);
}
