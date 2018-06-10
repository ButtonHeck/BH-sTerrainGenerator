#ifndef TREEGENERATOR_H
#define TREEGENERATOR_H
#include <vector>
#include <fstream>
#include "src/model/Model.h"
#include "src/graphics/Shader.h"
#include "src/game/Settings.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include "src/chunk/ModelChunk.h"
#include "src/graphics/Camera.h"

class TreeGenerator
{
public:
  TreeGenerator(std::initializer_list<Model> plainTrees, std::initializer_list<Model> hillTrees);
  void setupPlainModels(std::vector<std::vector<float>>& baseMap, std::vector<std::vector<float>>& hillMap, std::vector<ModelChunk>& chunks);
  void updatePlainModels(std::vector<glm::mat4*>& models, unsigned int* numAllTrees);
  void setupHillModels(std::vector<std::vector<float>>& hillMap, std::vector<ModelChunk>& chunks);
  void updateHillModels(std::vector<glm::mat4*>& models, unsigned int* numAllTrees);
  std::vector<glm::mat4*>& getTreeModels();
  std::vector<glm::mat4*>& getHillTreeModels();
  unsigned int getNumTrees(int i);
  unsigned int getNumHillTrees(int i);
  std::vector<Model>& getPlainTrees();
  std::vector<Model>& getHillTrees();
  void serialize(std::ofstream& out, std::vector<ModelChunk>& treeModelChunks, std::vector<ModelChunk>& hillTreeModelChunks);
private:
  std::vector<Model> plainTrees;
  std::vector<glm::mat4*> treeModels;
  unsigned int* numTrees;
  std::vector<Model> hillTrees;
  std::vector<glm::mat4*> hillTreeModels;
  unsigned int* numHillTrees;
  std::default_random_engine randomizer;
  bool treesAlreadyCreated = false;
  bool hillTreesAlreadyCreated = false;
};

#endif // TREEGENERATOR_H
