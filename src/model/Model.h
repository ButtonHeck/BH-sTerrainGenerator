#ifndef MODEL_H
#define MODEL_H
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <vector>
#include "Mesh.h"
#include "graphics/TextureLoader.h"
#include "chunk/ModelChunk.h"
#include "graphics/Camera.h"
#include "game/Settings.h"
#include "graphics/Frustum.h"

class Model
{
public:
  Model(const std::string& path, TextureLoader& textureLoader);
  void loadModel(const std::string& path);
  void draw(Shader& shaderProgram, const glm::vec2 &cameraPositionXZ, std::vector<ModelChunk>& chunks, unsigned int index,
            bool modelRenderOptimize, unsigned int chunkLoadingDistance, Frustum &frustum, bool bindTexture);
  void processNode(aiNode* node, const aiScene* scene);
  void loadInstances(glm::mat4* models, unsigned int numModels);
  Mesh processMesh(aiMesh* mesh, const aiScene* scene);
  std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);
private:
  std::vector<Mesh> meshes;
  std::vector<Texture> textures_loaded;
  std::string directory;
  TextureLoader& textureLoader;
};

#endif // MODEL_H
