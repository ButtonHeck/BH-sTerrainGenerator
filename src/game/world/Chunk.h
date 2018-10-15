#ifndef CHUNK_H
#define CHUNK_H
#include "util/Settings.h"

class Chunk
{
public:
  Chunk(unsigned int left, unsigned int right, unsigned int top, unsigned int bottom);
  bool containsPoint(float x, float z) const;
  unsigned int getLeft() const;
  unsigned int getRight() const;
  unsigned int getTop() const;
  unsigned int getBottom() const;
  glm::vec2 getMidPoint() const;
protected:
  unsigned int left, right, top, bottom;
  float midPointX, midPointZ;
};

inline bool Chunk::containsPoint(float x, float z) const
{
  if (z >= top && z < bottom)
    return x >= left && x < right;
  else
    return false;
}

inline unsigned int Chunk::getLeft() const
{
  return left;
}

inline unsigned int Chunk::getRight() const
{
  return right;
}

inline unsigned int Chunk::getTop() const
{
  return top;
}

inline unsigned int Chunk::getBottom() const
{
  return bottom;
}

inline glm::vec2 Chunk::getMidPoint() const
{
  return glm::vec2(midPointX, midPointZ);
}

#endif // CHUNK_H
