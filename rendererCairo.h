#ifndef RENDERER_SDL_H
#define RENDERER_SDL_H

#include <iostream>
#include <stdio.h>
#include <cairo.h>
#include <cairo-xlib.h>
#include "renderer.h"

#define MAX_COMMANDS 1000
#define MAX_IMAGES 100

typedef struct win {
    Display *dpy;
    int scr;

    Window win;
    GC gc;

    int width, height;
    KeyCode quit_code;
} win_t;

struct handle_type
{
  int handle;
  bool inUse;
  resolution_type size;
  win_t win;
  cairo_surface_t *surface;
  cairo_t * cr;  
};

enum draw_type
{
  COMMAND_CIRCLE = 0,
  COMMAND_COLOUR_BG,
  COMMAND_COLOUR_FG,
  COMMAND_IMAGE_PNG,
  COMMAND_LINE_JOIN,
  COMMAND_PEN_COLOUR,
  COMMAND_PEN_CURVE,
  COMMAND_PEN_LINE,
  COMMAND_PEN_MOVE,
  COMMAND_PEN_WIDTH,
};

struct command_type
{
  draw_type command;
  double width;
  double x1;
  double y1;
  double x2;
  double y2;
  double radius;
  colour_type colour;  
  int arg1;
  int arg2;
  int arg3;
  int arg4;
  bool fill;
};

struct image_type
{
  char name[255];
  cairo_surface_t *image;
};

static void triangle(cairo_t *cr);
static void square(cairo_t *cr);
static void bowtie(cairo_t *cr);
static void win_init(win_t *win, int width, int height);
static void win_deinit(win_t *win);
static void win_draw(win_t *win);
static void win_handle_events(win_t *win);

class rendererCairo : public renderer
{
public:
  rendererCairo (void);
  ~rendererCairo ();
   
  // Pure Virtual functions
  int init (int width, int height);
  void setPixel (int handle, int x, int y);
  void setColour (int handle, int red, int green, int blue);
  void setColourForground (int handle, int red, int green, int blue);
  void setColourBackground (int handle, int red, int green, int blue);
  int drawLine (int handle, int x1, int y1, int x2, int y2);
  int drawLineType (int handle, int opt);
  void drawCircle (int handle, int x, int y, int radius);
  void fillCircle (int handle, int x, int y, int radius);
  int drawColor (int handle, int r, int g, int b);
  int textureRGB (int handle, int x, int y, void *buffer);
  void scale (int handle, float x);
  void present (int handle);
  
  // Cairo specific functions
  void draw (int handle);
  win_t * getWin() { return &m_render_handle[0].win; }; 
private:
  double m_scale;
  handle_type m_render_handle[1];
  pthread_t m_event_thread;
  // Render List
  command_type m_draw_commands[MAX_COMMANDS];
  int m_draw_tail = 0;
  // Image List
  image_type m_image_list[MAX_IMAGES];
  int m_image_tail = 0;
};

#endif
