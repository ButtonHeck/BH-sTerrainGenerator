#ifndef SHORERENDERER_H
#define SHORERENDERER_H
#include "ShoreGenerator"

class ShoreRenderer
{
public:
  ShoreRenderer(ShoreGenerator& generator);
  void render();
  void debugRender(GLenum primitiveType);

private:
  ShoreGenerator& generator;
};

#endif // SHORERENDERER_H
