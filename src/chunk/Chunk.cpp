#include "Chunk.h"

Chunk::Chunk(unsigned int left, unsigned int right, unsigned int top, unsigned int bottom)
  :
    left(left),
    right(right),
    top(top),
    bottom(bottom)
{
  midPointX = -HALF_TILES_WIDTH + (right - left) / 2.0f + left;
  midPointY = -HALF_TILES_HEIGHT + (bottom - top) / 2.0f + top;
}