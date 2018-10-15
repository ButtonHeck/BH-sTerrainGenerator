#ifndef TERRAINCHUNK_H
#define TERRAINCHUNK_H
#include "util/Chunk.h"

class TerrainChunk : public Chunk
{
public:
  TerrainChunk(unsigned int left, unsigned int right, unsigned int top, unsigned int bottom, unsigned int offset, unsigned int instances);

  //instance offset
  void setInstanceOffset(unsigned int offset);
  unsigned int getInstanceOffset() const;

  //num instances
  void setNumInstances(unsigned int instances);
  unsigned int getNumInstances() const;

private:
  unsigned int instanceOffset;
  unsigned int numInstances;
};

inline void TerrainChunk::setInstanceOffset(unsigned int offset)
{
  this->instanceOffset = offset;
}

inline unsigned int TerrainChunk::getInstanceOffset() const
{
  return instanceOffset;
}

inline void TerrainChunk::setNumInstances(unsigned int instances)
{
  this->numInstances = instances;
}

inline unsigned int TerrainChunk::getNumInstances() const
{
  return numInstances;
}

#endif // TERRAINCHUNK_H