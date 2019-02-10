#ifndef RENDERER_H
#define RENDERER_H

#include<SDL2/SDL.h>
#define MAX_HANDLES 10

#define WHITE 255, 255, 255
#define RED 255, 0, 0
#define GREEN 0, 255, 0
#define BLUE 0, 0, 255
#define BLACK 0, 0, 0

struct colour_type
{
  int red;
  int green;
  int blue;
};

struct resolution_type
{
  int height;
  int width;
  int depth;
};

class rendererSdl;
class rendererCairo;

class renderer
{
public:
  virtual int init (int width, int height) = 0;
  virtual void setPixel (int handle, int x, int y) = 0;
  virtual void setColour (int handle, int red, int green, int blue) = 0;
  virtual void setColourForground (int handle, int red, int green, int blue) = 0;
  virtual void setColourBackground (int handle, int red, int green, int blue) = 0;
  virtual int drawLine (int handle, int x1, int y1, int x2, int y2) = 0;
  virtual void drawCircle (int handle, int x, int y, int radius) = 0;
  virtual void fillCircle (int handle, int x, int y, int radius) = 0;
  virtual int drawColor (int handle, int r, int g, int b) = 0;
  virtual int textureRGB (int handle, int x, int y, void *buffer) = 0;
  virtual void scale (int handle, float x) = 0;
  virtual void present (int handle) = 0;
private:
  colour_type m_forground_colour;
  colour_type m_background_colour;
  friend class rendererSdl;
  friend class rendererCairo;
};

#endif
