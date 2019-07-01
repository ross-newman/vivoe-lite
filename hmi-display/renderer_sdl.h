#ifndef RENDERER_SDL_H
#define RENDERER_SDL_H

#include <iostream>
#include <stdio.h>
#include <SDL2/SDL.h>
#include "renderer.h"

#define MAX_HANDLES 10

struct handle_type
{
  int handle;
  bool inUse;
  resolution_type size;
  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;
};

class rendererSdl : public renderer
{
public:
  rendererSdl (void);
   ~rendererSdl ();
  int init (int width, int height);
  void setPixel (int x, int y);
  int drawLine (int x1, int y1, int x2, int y2);
  void drawCircle (int x, int y, int radius, bool fill);
  void drawRectangle (int x1, int y1, int x2, int y2, bool fill);
  int drawColor (int r, int g, int b);
  int textureRGB (int x, int y, void *buffer);
  void scale (float x);
  void present ();
private:
  void fillCircle (int x, int y, int radius);
  int getFreeHandle ();
  handle_type m_render_handle[MAX_HANDLES];
  pthread_t m_event_thread;
};

#endif
