#ifndef MODEL_H
#define MODEL_H
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Mesh.h"
#include "TextureLoader.h"
#include <vector>
#include "ModelChunk.h"
#include "Camera.h"
#include "Settings.h"

class Model
{
public:
  Model(const std::string& path, TextureLoader& textureLoader);
  void loadModel(const std::string& path);
  void draw(Shader& shaderProgram, Camera& camera, std::vector<ModelChunk>& chunks, unsigned int index,
            bool modelRenderOptimize, unsigned int CHUNK_LOADING_DISTANCE);
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
