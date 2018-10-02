#include "game/SaveLoadManager.h"

SaveLoadManager::SaveLoadManager(std::shared_ptr<BaseMapGenerator> baseGenerator,
                                 std::shared_ptr<HillsMapGenerator> hillGenerator,
                                 std::shared_ptr<WaterMapGenerator> waterGenerator,
                                 std::shared_ptr<BuildableMapGenerator> buildableGenerator, Camera& camera)
  :
    baseGenerator(baseGenerator),
    hillGenerator(hillGenerator),
    waterGenerator(waterGenerator),
    buildableGenerator(buildableGenerator),
    camera(camera),
    baseMap(baseGenerator->getMap()),
    hillMap(hillGenerator->getMap()),
    waterMap(waterGenerator->getMap()),
    chunkMap(baseGenerator->getChunkMap())
{

}

bool SaveLoadManager::saveToFile(const std::string &filename)
{
  std::ofstream output(filename);
  if (!output)
    {
      std::cerr << "Could not open file: " << filename << std::endl;
      return false;
    }
  output << WORLD_WIDTH << " " << WORLD_HEIGHT << " ";
  for (std::vector<float>& row : baseMap)
    {
      for (float& value : row)
        {
          output << value << " ";
        }
    }
  for (std::vector<float>& row : chunkMap)
    {
      for (unsigned int i = 0; i < row.size(); i++)
        output << 0 << " ";
    }
  for (std::vector<float>& row : hillMap)
    {
      for (float& value : row)
        {
          output << value << " ";
        }
    }
  for (std::vector<float>& row : waterMap)
    {
      for (float& value : row)
        {
          output << value << " ";
        }
    }
  treeGenerator->serialize(output);
  output << camera.getPosition().x << " ";
  output << camera.getPosition().y << " ";
  output << camera.getPosition().z << " ";
  output << camera.getPitch() << " ";
  output << camera.getYaw() << " ";
  output.close();
  return true;
}

bool SaveLoadManager::loadFromFile(const std::string &filename)
{
  std::ifstream input(filename);
  if (!input)
    {
      std::cerr << "Could not open file: " << filename;
      return false;
    }
  int tiles_width, tiles_height;
  input >> tiles_width >> tiles_height;
  for (std::vector<float>& row : baseMap)
    {
      for (float& value : row)
        {
          input >> value;
        }
    }
  for (std::vector<float>& row : chunkMap)
    {
      for (float& value : row)
        {
          input >> value;
        }
    }
  for (std::vector<float>& row : hillMap)
    {
      for (float& value : row)
        {
          input >> value;
        }
    }
  for (std::vector<float>& row : waterMap)
    {
      for (float& value : row)
        {
          input >> value;
        }
    }

  std::vector<ModelChunk>& treeModelChunks = treeGenerator->getPlainPlantsModelChunks();
  for (unsigned int chunk = 0; chunk < treeModelChunks.size(); chunk++)
    {
      for (unsigned int i = 0; i < treeModelChunks[chunk].getNumInstancesVector().size(); i++)
        {
          unsigned int numInstances;
          input >> numInstances;
          treeModelChunks[chunk].setNumInstances(i, numInstances);
        }
      for (unsigned int i = 0; i < treeModelChunks[chunk].getInstanceOffsetVector().size(); i++)
        {
          unsigned int offset;
          input >> offset;
          treeModelChunks[chunk].setInstanceOffset(i, offset);
        }
    }
  std::vector<ModelChunk>& hillTreeModelChunks = treeGenerator->getHillTreeModelChunks();
  for (unsigned int chunk = 0; chunk < hillTreeModelChunks.size(); chunk++)
    {
      for (unsigned int i = 0; i < hillTreeModelChunks[chunk].getNumInstancesVector().size(); i++)
        {
          unsigned int numInstances;
          input >> numInstances;
          hillTreeModelChunks[chunk].setNumInstances(i, numInstances);
        }
      for (unsigned int i = 0; i < hillTreeModelChunks[chunk].getInstanceOffsetVector().size(); i++)
        {
          unsigned int offset;
          input >> offset;
          hillTreeModelChunks[chunk].setInstanceOffset(i, offset);
        }
    }

  std::vector<glm::mat4*> treeModels;
  unsigned int numAllTrees[treeGenerator->getPlainPlantsMatrices().size()];
  for (unsigned int i = 0; i < treeGenerator->getPlainPlantsMatrices().size(); i++)
    {
      unsigned int numTrees = 0;
      input >> numTrees;
      numAllTrees[i] = numTrees;
      treeModels.emplace_back(new glm::mat4[numTrees]);
      for (unsigned int t = 0; t < numTrees; t++)
        {
          glm::mat4 model;
          float* modelData = (float*)glm::value_ptr(model);
          for (unsigned int e = 0; e < 16; e++)
            {
              input >> modelData[e];
            }
          treeModels[i][t] = std::move(model);
        }
    }
  std::vector<glm::mat4*> hillTreeModels;
  unsigned int numAllHillTrees[treeGenerator->getHillTreesMatrices().size()];
  for (unsigned int i = 0; i < treeGenerator->getHillTreesMatrices().size(); i++)
    {
      unsigned int numHillTrees = 0;
      input >> numHillTrees;
      numAllHillTrees[i] = numHillTrees;
      hillTreeModels.emplace_back(new glm::mat4[numHillTrees]);
      for (unsigned int m = 0; m < numHillTrees; m++)
        {
          glm::mat4 model;
          float* modelData = (float*)glm::value_ptr(model);
          for (unsigned int e = 0; e < 16; e++)
            {
              input >> modelData[e];
            }
          hillTreeModels[i][m] = std::move(model);
        }
    }
  treeGenerator->updatePlainModels(treeModels, numAllTrees);
  treeGenerator->updateHillModels(hillTreeModels, numAllHillTrees);

  hillGenerator->createTiles();
  hillGenerator->fillBufferData();
  baseGenerator->getSquareTiles().clear();
  baseGenerator->getCellTiles().clear();
  baseGenerator->prepareMap();
  baseGenerator->fillShoreBufferData();
  baseGenerator->fillSquareBufferData();
  baseGenerator->fillCellBufferData();
  waterGenerator->postPrepareMap();
  waterGenerator->fillBufferData();

  float camPosX, camPosY, camPosZ, pitch, yaw;
  input >> camPosX >> camPosY >> camPosZ >> pitch >> yaw;
  camera.setPosition(camPosX, camPosY, camPosZ);
  camera.setPitch(pitch);
  camera.setYaw(yaw);
  camera.updateVectors();
  return true;
}

void SaveLoadManager::setTreeGenerator(const std::shared_ptr<PlantGenerator> treeGenerator)
{
  this->treeGenerator = treeGenerator;
}
