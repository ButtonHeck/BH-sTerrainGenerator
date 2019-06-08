#pragma once

#include "Scene"
#include "Logger"

#include <fstream>
#include <vector>

class Camera;

class SaveLoadManager
{
public:
  SaveLoadManager(Scene& scene, Camera& camera, Camera& shadowCamera);
  bool saveToFile(const std::string& filename);
  bool loadFromFile(const std::string& filename);
private:
  Scene& scene;
  Camera& camera;
  Camera& shadowCamera;
};
