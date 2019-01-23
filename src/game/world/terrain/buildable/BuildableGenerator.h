#ifndef BUILDABLEGENERATOR_H
#define BUILDABLEGENERATOR_H
#include "game/world/terrain/Generator.h"

class BuildableGenerator : public Generator
{
public:
  BuildableGenerator();
  virtual ~BuildableGenerator() = default;
  void setup(const map2D_f &landMap, const map2D_f &hillsMap);

private:
  friend class BuildableRenderer;

  void setupAndBindBuffers(BufferCollection& buffers);
  void fillBufferData();

  BufferCollection selectedBuffers;
};

#endif // BUILDABLEGENERATOR_H
