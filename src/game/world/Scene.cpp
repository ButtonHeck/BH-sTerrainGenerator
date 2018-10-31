#include "game/world/Scene.h"

Scene::Scene(ShaderManager &shaderManager, Options &options, TextureManager &textureManager)
  :
    shaderManager(shaderManager),
    options(options),
    textureManager(textureManager),
    waterFacade(shaderManager.get(SHADER_WATER), shaderManager.get(SHADER_WATER_CULLING)),
    hillsFacade(shaderManager.get(SHADER_HILLS), shaderManager.get(SHADER_HILLS_CULLING), waterFacade.getMap()),
    shoreFacade(shaderManager.get(SHADER_SHORE), waterFacade.getMap()),
    buildableFacade(shaderManager.get(SHADER_BUILDABLE), shaderManager.get(SHADER_SELECTED)),
    plantsFacade(shaderManager.get(SHADER_MODELS_PHONG), shaderManager.get(SHADER_MODELS), shaderManager.get(SHADER_SHADOW_MODELS)),
    skyboxFacade(shaderManager.get(SHADER_SKYBOX)),
    theSunFacade(shaderManager.get(SHADER_SUN)),
    underwaterFacade(shaderManager.get(SHADER_UNDERWATER))
{
  landFacade = std::make_unique<LandFacade>(shaderManager.get(SHADER_LAND));
}

void Scene::setup()
{
  waterFacade.setup();
  hillsFacade.setup();
  shoreFacade.setup();
  landFacade->setup(shoreFacade.getMap());
  waterFacade.setupConsiderTerrain();
  buildableFacade.setup(landFacade->getMap(), hillsFacade.getMap());
  plantsFacade.setup(landFacade->getMap(), hillsFacade.getMap());
  textureManager.createUnderwaterReliefTexture(waterFacade.getMap());
}

void Scene::recreate()
{
  landFacade.reset(new LandFacade(shaderManager.get(SHADER_LAND)));
  initializeMap(waterFacade.getMap());
  initializeMap(hillsFacade.getMap());
  setup();
}

void Scene::load()
{
  hillsFacade.createTilesAndBufferData();
  shoreFacade.setup();
  landFacade->setup(shoreFacade.getMap());
  waterFacade.setupConsiderTerrain();
  buildableFacade.setup(landFacade->getMap(), hillsFacade.getMap());
  textureManager.createUnderwaterReliefTexture(waterFacade.getMap());
}

void Scene::serialize(std::ofstream &output)
{
  landFacade->serialize(output);
  shoreFacade.serialize(output);
  hillsFacade.serialize(output);
  waterFacade.serialize(output);
  plantsFacade.serialize(output);
}

void Scene::deserialize(std::ifstream &input)
{
  landFacade->deserialize(input);
  shoreFacade.deserialize(input);
  hillsFacade.deserialize(input);
  waterFacade.deserialize(input);
  plantsFacade.deserialize(input);
}

void Scene::drawWorld(glm::mat4& projectionView,
                      glm::mat4 &skyProjectionView,
                      Frustum &viewFrustum,
                      Camera& camera,
                      MouseInputManager& mouseInput)
{
  this->projectionView = projectionView;
  glm::vec3 viewPosition = camera.getPosition();

  hillsFacade.draw(options.get(OPT_HILLS_CULLING), options.get(OPT_USE_SHADOWS), projectionView, viewPosition, viewFrustum);
  if (options.get(OPT_DRAW_LAND))
    landFacade->draw(projectionView, options.get(OPT_USE_SHADOWS), viewFrustum, textureManager.get(TEX_LAND));
  underwaterFacade.draw(projectionView);
  shoreFacade.draw(projectionView, options.get(OPT_USE_SHADOWS));
  if (options.get(OPT_DRAW_TREES))
    plantsFacade.draw(projectionView,
                       viewPosition,
                       options.get(OPT_MODELS_PHONG_SHADING),
                       options.get(OPT_MODELS_SHADOW_EMPHASIZE),
                       options.get(OPT_USE_SHADOWS),
                       options.get(OPT_MODELS_FLAT_BLENDING));
  if (options.get(OPT_DRAW_BUILDABLE))
    buildableFacade.drawBuildable(projectionView);
  if (options.get(OPT_SHOW_CURSOR))
    {
      mouseInput.updateCursorMappingCoordinates(camera, landFacade->getMap(), hillsFacade.getMap(), buildableFacade.getMap());
      buildableFacade.drawSelected(mouseInput, projectionView);
    }
  if (options.get(OPT_DRAW_WATER))
    waterFacade.draw(options.get(OPT_WATER_CULLING), projectionView, viewPosition, viewFrustum);
  theSunFacade.draw(skyProjectionView);
  skyboxFacade.draw(skyProjectionView, viewPosition);
}

void Scene::drawWorldDepthmap()
{
  glClear(GL_DEPTH_BUFFER_BIT);
  glDisable(GL_CULL_FACE); //or set front face culling

  shaderManager.get(SHADER_SHADOW_TERRAIN).use();
  hillsFacade.drawDepthmap();
  shoreFacade.drawDepthmap();
  if (options.get(OPT_DRAW_TREES))
    plantsFacade.drawDepthmap();

  glEnable(GL_CULL_FACE); //or set back face culling
}

WaterFacade &Scene::getWaterFacade()
{
  return waterFacade;
}

HillsFacade &Scene::getHillsFacade()
{
  return hillsFacade;
}

PlantsFacade &Scene::getPlantsFacade()
{
  return plantsFacade;
}
