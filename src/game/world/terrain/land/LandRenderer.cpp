#include "LandRenderer.h"

LandRenderer::LandRenderer(LandGenerator &generator)
  :
    generator(generator)
{}

void LandRenderer::render(Frustum &frustum, GLuint &texture)
{
  glBindTextureUnit(TEX_LAND, texture);
  //square chunks are better to render without FC
  generator.basicGLBuffers.bind(VAO);
  glDrawElementsInstanced(GL_TRIANGLES, VERTICES_PER_TILE, GL_UNSIGNED_BYTE, 0, generator.tiles.size());

  //these ones should probably be FC-ed with multiDrawIndirect
  generator.cellBuffers.bind(VAO);
  auto& cellChunks = generator.cellChunks;
  GLuint multiDrawIndirectData[cellChunks.size() * 5]; // { indicesCount, numInstancesToDraw, firstIndex, baseVertex, baseInstance }
  GLuint dataOffset = 0;
  GLuint drawIndirectCommandPrimCount = 0;
  for (unsigned int i = 0; i < cellChunks.size(); i++)
    {
      glm::vec2 chunkMidPoint = cellChunks[i].getMidPoint();
      if (frustum.isInsideXZ(chunkMidPoint.x - HALF_CHUNK_SIZE, chunkMidPoint.y + HALF_CHUNK_SIZE, MODELS_FC_RADIUS) ||
          frustum.isInsideXZ(chunkMidPoint.x + HALF_CHUNK_SIZE, chunkMidPoint.y + HALF_CHUNK_SIZE, MODELS_FC_RADIUS) ||
          frustum.isInsideXZ(chunkMidPoint.x + HALF_CHUNK_SIZE, chunkMidPoint.y - HALF_CHUNK_SIZE, MODELS_FC_RADIUS) ||
          frustum.isInsideXZ(chunkMidPoint.x - HALF_CHUNK_SIZE, chunkMidPoint.y - HALF_CHUNK_SIZE, MODELS_FC_RADIUS))
        {
          addIndirectBufferData(drawIndirectCommandPrimCount,
                                multiDrawIndirectData,
                                dataOffset, cellChunks[i].getNumInstances(), cellChunks[i].getInstanceOffset());
        }
    }
  generator.cellBuffers.bind(DIBO);
  glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(GLuint) * 5 * drawIndirectCommandPrimCount, multiDrawIndirectData, GL_STATIC_DRAW);
  glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_BYTE, 0, drawIndirectCommandPrimCount, 0);
}

void LandRenderer::addIndirectBufferData(GLuint &primCount,
                                         GLuint *buffer,
                                         GLuint &dataOffset,
                                         GLuint numInstances,
                                         GLuint instanceOffset)
{
  ++primCount;
  buffer[dataOffset++] = VERTICES_PER_TILE;
  buffer[dataOffset++] = numInstances;
  buffer[dataOffset++] = 0;
  buffer[dataOffset++] = 0;
  buffer[dataOffset++] = instanceOffset;
}
