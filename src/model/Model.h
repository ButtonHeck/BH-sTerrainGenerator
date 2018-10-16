#ifndef MODEL_H
#define MODEL_H
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <vector>
#include "model/Mesh.h"
#include "graphics/TextureLoader.h"
#include "game/world/plants/ModelChunk.h"
#include "game/Camera.h"
#include "util/Settings.h"
#include "graphics/Frustum.h"

class Model
{
public:
  Model(const std::string& path);
  Model(Model&& old) noexcept;
  void cleanup();
  static void bindTextureLoader(TextureLoader& textureLoader);
  void loadModel(const std::string& path);
  void draw(bool bindTexture, bool updateIndirect);
  void prepareMeshesIndirectData(std::vector<ModelChunk>& chunks,
                                 unsigned int index,
                                 const glm::vec2& cameraPositionXZ,
                                 const Frustum& frustum);
  void processNode(aiNode* node, const aiScene* scene);
  void loadInstances(glm::mat4* models, unsigned int numModels);
  std::vector<Mesh>& getMeshes();
  Mesh processMesh(aiMesh* mesh, const aiScene* scene);
  std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);
private:
  std::vector<Mesh> meshes;
  std::vector<Texture> textures_loaded;
  std::string directory;
  static TextureLoader* textureLoader;
};

#endif // MODEL_H
