#ifndef MESH_H
#define MESH_H
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/detail/func_common.hpp>
#include <glm/detail/func_geometric.hpp>
#include "src/graphics/Shader.h"
#include "src/game/Settings.h"
#include "src/chunk/ModelChunk.h"
#include "src/graphics/Camera.h"

struct Vertex
{
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoords;
  glm::vec3 Tangent;
  glm::vec3 Bitangent;
};

struct Texture
{
  unsigned int id;
  std::string type;
  std::string path;
};

class Mesh
{
public:
  Mesh(std::vector<Vertex> vertices, std::vector<Texture> textures, std::vector<unsigned int> indices);
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
  void setupMesh();
  void setupInstances(glm::mat4* models, unsigned int numModels);
  void draw(Shader& shader, Camera& camera, std::vector<ModelChunk>& chunks, unsigned int index,
            bool modelRenderOptimize, unsigned int chunkLoadingDistance);
  unsigned int VAO;
private:
  std::vector<Vertex> vertices;
  std::vector<Texture> textures;
  std::vector<unsigned int> indices;
  unsigned int VBO, EBO, instanceVBO;
  unsigned int numInstances;
};

#endif // MESH_H
