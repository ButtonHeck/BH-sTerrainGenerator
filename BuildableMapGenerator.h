#ifndef BUILDABLEMAPGENERATOR_H
#define BUILDABLEMAPGENERATOR_H
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "MapGenerator.h"
#include "BaseMapGenerator.h"
#include "WaterMapGenerator.h"
#include "HillsMapGenerator.h"
#include "Settings.h"

class BuildableMapGenerator : public MapGenerator
{
public:
  BuildableMapGenerator(std::vector<std::vector<float>>& baseMap, std::vector<std::vector<float>>& hillMap);
  void prepareMap();
  void fillBufferData();
private:
  std::vector<std::vector<float>>& baseMap;
  std::vector<std::vector<float>>& hillMap;
};

#endif // BUILDABLEMAPGENERATOR_H
