#ifndef UNDERWATERQUADMAPGENERATOR_H
#define UNDERWATERQUADMAPGENERATOR_H
#include "MapGenerator.h"

class UnderwaterQuadMapGenerator : public MapGenerator
{
public:
  UnderwaterQuadMapGenerator();
  void fillBufferData();
};

#endif // UNDERWATERQUADMAPGENERATOR_H