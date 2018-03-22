#ifndef MODEL_H
#define MODEL_H
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Mesh.h"
#include "TextureLoader.h"

class Model
{
public:
  Model(const std::string& path, TextureLoader& textureLoader);
  void loadModel(const std::string& path);
  void draw(Shader shaderProgram);
  void processNode(aiNode* node, const aiScene* scene);
  Mesh processMesh(aiMesh* mesh, const aiScene* scene);
  std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);
private:
  std::vector<Mesh> meshes;
  std::vector<Texture> textures_loaded;
  std::string directory;
  TextureLoader& textureLoader;
};

#endif // MODEL_H
