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

class Animal {
public:
    void /*non-virtual*/ move(void) { 
        std::cout << "This animal moves in some way" << std::endl; 
    }
    virtual void eat(void) = 0;
};

// The class "Animal" may possess a definition for eat() if desired.
class Llama : public Animal {
public:
    // The non virtual function move() is inherited but not overridden
    void eat(void) override { 
        std::cout << "Llamas eat grass!" << std::endl; 
    }
};



class rendererSdl : public renderer
{
public:
  rendererSdl (void);
   ~rendererSdl ();
  int init (int width, int height);
  void setPixel (int handle, int x, int y);
  int drawLine (int handle, int x1, int y1, int x2, int y2);
  void drawCircle (int handle, int x, int y, int radius);
  void fillCircle (int handle, int x, int y, int radius);
  int drawColor (int handle, int r, int g, int b);
  int textureRGB (int handle, int x, int y, void *buffer);
  void scale (int handle, float x);
  void present (int handle);
private:
  int getFreeHandle ();
  handle_type m_render_handle[MAX_HANDLES];
  pthread_t m_event_thread;
};

#endif
