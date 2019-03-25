/*
 * Copyright 2019 Ilya Malgin
 * Model.h
 * This file is part of The Sugarpunky Chocolate Disaster project
 *
 * The Sugarpunky Chocolate Disaster project is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The Sugarpunky Chocolate Disaster project is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * See <http://www.gnu.org/licenses/>
 *
 * Purpose: contains declaration for Model class
 * @version 0.1.0
 */

#ifndef MODEL_H
#define MODEL_H
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "game/world/models/Mesh.h"
#include "graphics/textures/TextureLoader.h"
#include "graphics/textures/TextureUnits.h"
#include "game/world/models/ModelChunk.h"
#include "game/world/models/ModelGPUDataManager.h"
#include "game/world/models/ModelRenderer.h"
#include "graphics/textures/BindlessTextureManager.h"

/**
 * @brief Wrapper for .obj model.
 * Responsible for compiling model's meshes data into one contiguous storage and delegating tasks
 * among its member objects
 */
class Model
{
public:
  Model(const std::string& path, bool isLowPoly, unsigned int numRepetitions = 1);
  static void bindTextureLoader(TextureLoader& textureLoader);
  void draw(bool isShadow);
  void drawOneInstance();
  void prepareIndirectBufferData(const std::vector<ModelChunk> &chunks,
                                 unsigned int modelIndex,
                                 const glm::vec2& cameraPositionXZ,
                                 const Frustum& frustum,
                                 float loadingDistance,
                                 float loadingDistanceShadow,
                                 float loadingDistanceLowPoly);
  void updateIndirectBufferData();
  void loadModelInstances(const std::vector<glm::mat4> &instanceMatrices, unsigned int numInstances);
  unsigned int getRepeatCount() const;

private:
  static TextureLoader* textureLoader;
  void load(const std::string& path);
  void processNode(const aiNode *node, const aiScene* scene, GLuint &meshVertexIndexOffset);
  void loadMaterialTextures(const aiMaterial *material, aiTextureType type, const std::string &typeName, unsigned int &textureIndex);

  std::string directory;
  std::vector<Mesh::Vertex> vertices;
  std::vector<GLuint> indices;
  /**
   * @brief isLowPoly defines whether this model would be approached as low-poly
   */
  bool isLowPoly;
  /**
   * @brief numRepetitions defines how many times in a row this model would be used during allocation on the map
   */
  unsigned int numRepetitions;
  ModelGPUDataManager GPUDataManager;
  ModelRenderer renderer;
};

#endif // MODEL_H
