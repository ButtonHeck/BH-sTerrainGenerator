#include "Model.h"
#include <IL/il.h>

Model::Model(const std::string& path, TextureLoader& textureLoader)
  :
    textureLoader(textureLoader)
{
  loadModel(std::string(CWD + path));
}

void Model::loadModel(const std::string &path)
{
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(path, aiProcess_CalcTangentSpace | aiProcess_FlipUVs | aiProcess_Triangulate);
  if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
    {
      std::cerr << "Error while loading Assimp" << importer.GetErrorString() << std::endl;
    }
  directory = path.substr(0, path.find_last_of('/'));
  processNode(scene->mRootNode, scene);
}

void Model::draw(Shader &shaderProgram, Camera& camera, std::vector<ModelChunk>& chunks, unsigned int index,
                 bool modelRenderOptimize, unsigned int chunkLoadingDistance)
{
  for (unsigned int i = 0; i < meshes.size(); i++)
    meshes[i].draw(shaderProgram, camera, chunks, index, modelRenderOptimize, chunkLoadingDistance);
}

void Model::processNode(aiNode *node, const aiScene* scene)
{
  for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
      aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      meshes.push_back(processMesh(mesh, scene));
    }
  for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
      processNode(node->mChildren[i], scene);
    }
}

void Model::loadInstances(glm::mat4 *models, unsigned int numModels)
{
  for (Mesh& mesh : meshes)
    mesh.setupInstances(models, numModels);
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene* scene)
{
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
      Vertex vertex;
      glm::vec3 vector;
      //position
      vector.x = mesh->mVertices[i].x;
      vector.y = mesh->mVertices[i].y;
      vector.z = mesh->mVertices[i].z;
      vertex.Position = vector;
      //normals
      vector.x = mesh->mNormals[i].x;
      vector.y = mesh->mNormals[i].y;
      vector.z = mesh->mNormals[i].z;
      vertex.Normal = vector;
      //TexCoords (if any)
      if (mesh->mTextureCoords[0])
        {
          glm::vec2 texVec;
          texVec.x = mesh->mTextureCoords[0][i].x;
          texVec.y = mesh->mTextureCoords[0][i].y;
          vertex.TexCoords = texVec;
        }
      else
        vertex.TexCoords = glm::vec2(0.0, 0.0);
      //tangent
      vector.x = mesh->mTangents[i].x;
      vector.y = mesh->mTangents[i].y;
      vector.z = mesh->mTangents[i].z;
      vertex.Tangent = vector;
      //bitangent
      vector.x = mesh->mBitangents[i].x;
      vector.y = mesh->mBitangents[i].y;
      vector.z = mesh->mBitangents[i].z;
      vertex.Bitangent = vector;
      vertices.push_back(vertex);
    }
  //process indices
  for (unsigned int i=0; i < mesh->mNumFaces; i++)
    {
      aiFace face = mesh->mFaces[i];
      for (unsigned int j = 0; j < face.mNumIndices; j++)
        indices.push_back(face.mIndices[j]);
    }
  //process materials
  aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

  std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "u_texture_diffuse");
  textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

  std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "u_texture_specular");
  textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

  std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "u_texture_normal");
  textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

  std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "u_texture_height");
  textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

  return Mesh(vertices, textures, indices);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *material, aiTextureType type, std::__cxx11::string typeName)
{
  std::vector<Texture> textures;
  for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
    {
      bool skip = false;
      aiString texturePath;
      material->GetTexture(type, i, &texturePath);
      for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
          if (strcmp(texturePath.C_Str(), textures_loaded[j].path.data()) == 0)
            {
              skip = true;
              break;
            }
        }
      if (!skip)
        {
          Texture texture;
          texture.type = typeName;
          std::string path = this->directory + '/' + std::string(texturePath.C_Str());
          texture.id = textureLoader.loadTexture(path, 0, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, !INCLUDE_CWD);
          texture.path = texturePath.C_Str();
          textures.push_back(texture);
          textures_loaded.push_back(texture);
        }
    }
  return textures;
}
