#ifndef SCENE_H
#define SCENE_H
#include <memory>
#include "game/world/terrain/land/LandFacade.h"
#include "game/world/terrain/hills/HillsFacade.h"
#include "game/world/plants/PlantsFacade.h"
#include "game/world/terrain/water/WaterFacade.h"
#include "game/world/terrain/buildable/BuildableFacade.h"
#include "game/world/terrain/shore/ShoreFacade.h"
#include "game/world/terrain/underwater/UnderwaterFacade.h"
#include "game/world/ambience/skybox/SkyboxFacade.h"
#include "game/world/ambience/theSun/TheSunFacade.h"
#include "graphics/ShaderManager.h"
#include "graphics/TextureManager.h"
#include "input/MouseInputManager.h"

class Scene
{
public:
  Scene(ShaderManager& shaderManager, Options& options, TextureManager& textureManager);

  //internal generators functions
  void setup();
  void recreate();
  void load();
  void serialize(std::ofstream& output);
  void deserialize(std::ifstream& input);

  //rendering part
  void drawWorld(glm::vec3 lightDir,
                 glm::mat4 lightSpaceMatrixNear,
                 glm::mat4 lightSpaceMatrixMiddle,
                 glm::mat4 lightSpaceMatrixFar,
                 glm::mat4& projectionView,
                 glm::mat4& skyProjectionView,
                 Frustum &viewFrustum,
                 Camera& camera,
                 MouseInputManager& mouseInput);
  void drawWorldDepthmap(glm::mat4 lightSpaceMatrixNear,
                         glm::mat4 lightSpaceMatrixMiddle,
                         glm::mat4 lightSpaceMatrixFar);

  //getters
  WaterFacade &getWaterFacade();
  HillsFacade &getHillsFacade();
  PlantsFacade &getPlantsFacade();
  TheSunFacade &getSunFacade();

private:
  ShaderManager& shaderManager;
  Options& options;
  TextureManager& textureManager;

  WaterFacade waterFacade;
  HillsFacade hillsFacade;
  ShoreFacade shoreFacade;
  BuildableFacade buildableFacade;
  PlantsFacade plantsFacade;
  SkyboxFacade skyboxFacade;
  TheSunFacade theSunFacade;
  UnderwaterFacade underwaterFacade;
  std::unique_ptr<LandFacade> landFacade;
};

#endif // SCENE_H
