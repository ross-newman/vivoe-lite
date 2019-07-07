// 
// MIT License
// 
// Copyright (c) 2019 Ross Newman (ross@rossnewman.com)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// 

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
