#ifndef RENDERER_H
#define RENDERER_H

#include "debug.h"
#include<SDL2/SDL.h>
#define MAX_HANDLES 10

#define WHITE 255, 255, 255
#define RED 255, 0, 0
#define GREY 128, 128, 128
#define DARK_GREY 64, 64, 64
#define GREEN 0, 255, 0
#define DARK_GREEN 0, 128, 0
#define DARK_GREEN2 0, 64, 0
#define BLUE 0, 0, 255
#define LIGHT_BLUE 50, 50, 255
#define BLACK 0, 0, 0
#define YELLOW 255 , 255, 0

struct colour_type
{
  int red;
  int green;
  int blue;
};

struct point_type
{
  int x;
  int y;
};

struct resolution_type
{
  int width;
  int height;
  int depth;
};

class rendererSdl;
class rendererCairo;

class renderer
{
public:
  int init (int width, int height);
  virtual void setPixel (int handle, int x, int y) = 0;
  virtual void setColour (int handle, int red, int green, int blue) = 0;
  virtual void setColourForground (int handle, int red, int green, int blue) = 0;
  virtual void setColourBackground (int handle, int red, int green, int blue) = 0;
  int getWidth (int handle) { return m_width; };
  int getHeight (int handle) { return m_height; };
  virtual int drawLine (int handle, int x1, int y1, int x2, int y2) = 0;
  virtual void drawCircle (int handle, int x, int y, int radius, bool fill) = 0;
  virtual void drawRectangle (int handle, int x1, int y1, int x2, int y2, bool fill) = 0;
  virtual int drawColor (int handle, int r, int g, int b) = 0;
  virtual int textureRGB (int handle, int x, int y, void *buffer) = 0;
  virtual void scale (int handle, float x) = 0;
  virtual void present (int handle) = 0;
private:
  int m_height;
  int m_width;
  colour_type m_forground_colour;
  colour_type m_background_colour;
  friend class rendererSdl;
  friend class rendererCairo;
};

#endif
