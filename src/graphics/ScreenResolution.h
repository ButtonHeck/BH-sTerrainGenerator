#ifndef SCREENRESOLUTION_H
#define SCREENRESOLUTION_H

class ScreenResolution
{
public:
  ScreenResolution() = default;
  ScreenResolution(int width, int height, float aspect);
  void updateResolution(int newWidth, int newHeight);
  int getWidth() const;
  int getHeight() const;
  float getAspectRatio() const;
  constexpr static float REFERENCE_WIDTH = 1440.0f;
  constexpr static float REFERENCE_HEIGHT = 900.0f;
  float getWidthRatioToReference() const;
  float getHeightRatioToReference() const;

private:
  void updateAspectRatio(float newAspect);
  int width = 0, height = 0;
  float aspectRatio = 1.0f;
};

#endif // SCREENRESOLUTION_H
