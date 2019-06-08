#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H
#include <GL/glew.h>
#include <unordered_map>
#include "TextureLoader"
#include "TextureUnits"

class TextureManager
{
public:
  TextureManager(TextureLoader& loader);
  virtual ~TextureManager();
  void createUnderwaterReliefTexture(const map2D_f &waterMap);
  GLuint& get(int textureUnit);
  TextureLoader& getLoader();

private:
  TextureLoader& loader;
  std::unordered_map<int, GLuint> textures;
};

#endif // TEXTUREMANAGER_H
