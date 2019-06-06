#ifndef RENDERER_SDL_H
#define RENDERER_SDL_H

#include <iostream>
#include <stdio.h>
#include <cairo.h>
#include <cairo-xlib.h>
#include "renderer.h"

#define MAX_COMMANDS 1000
#define MAX_IMAGES 100
#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480

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
  COMMAND_PEN_DRAW,
  COMMAND_PEN_MOVE,
  COMMAND_PEN_RECTANGLE,
  COMMAND_PEN_THICKNESS,
  COMMAND_PEN_TRIANGLE,
  COMMAND_TEXT_FONT,
  COMMAND_TEXT,
};

struct command_type
{
  draw_type command;
  double width;
  point_type points[3];
  double radius;
  colour_type colour;  
  int arg1;
  int arg2;
  int arg3;
  int arg4;
  char text[80];
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

typedef enum {
  LINE_SOLID,
  LINE_DASHED,
  LINE_DASHED_MEDIUM,
  LINE_DASHED_LARGE,
} lineType;

class rendererCairo : public renderer
{
public:
  rendererCairo (int width, int height);
  ~rendererCairo ();
  int init (int width, int height);   
  // Pure Virtual functions
  void setPixel (int handle, int x, int y);
  void setColour (int handle, int red, int green, int blue);
  void setColourForground (int handle, int red, int green, int blue);
  void setColourBackground (int handle, int red, int green, int blue);
  void setLineType (int handle, int type);
  void setLineThickness (int handle, int thickness, lineType fill);
  int movePen (int handle, int x, int y);
  int drawPen (int handle, int x, int y, bool close);
  int drawLine (int handle, int x1, int y1, int x2, int y2);
  void drawCircle (int handle, int x, int y, int radius, bool fill);
  void drawRectangle (int handle, int x1, int y1, int x2, int y2, bool fill);
  void drawTriangle (int handle, int x1, int y1, int x2, int y2, int x3, int y3, bool fill);
  int drawColor (int handle, int r, int g, int b);
  void setTextFont (int handle, int slope, int weight, char* fontName);
  int getTextWidth (int handle, char* str);
  int getTextHeight (int handle, char* str);
  void drawText (int handle, int x, int y, char* text, int size);
  void drawTextCentre (int handle, int x, char* text, int size);
  int textureRGB (int handle, int x, int y, void *buffer, char *file);
  void scale (int handle, float x);
  void present (int handle);
  
  // Cairo specific functions
  void draw (int handle);
  void reset () { m_draw_tail = 0; }
  win_t* getWin() { return &m_render_handle[0].win; }
  Window* getWindow() { return &m_render_handle[0].win.win; }
  Display* getDisplay() { return m_render_handle[0].win.dpy; }
  void setHeight(int height) { cairo_xlib_surface_set_size(m_render_handle[0].surface, m_render_handle[0].win.width, height); 
      cairo_scale(m_render_handle[0].cr, 1.0, (double)height / m_render_handle[0].win.height);  m_render_handle[0].win.height = height; }
  void setWidth(int width) { cairo_xlib_surface_set_size(m_render_handle[0].surface, width, m_render_handle[0].win.height); 
      cairo_scale(m_render_handle[0].cr, (double)width / m_render_handle[0].win.width, 1.0);  m_render_handle[0].win.width = width; }
  int getHeight() { return m_render_handle[0].win.height; }
  int getWidth() { return m_render_handle[0].win.width; }
private:

  int m_current_handle;
  double m_scale;
  handle_type m_render_handle[1];
  Window m_root;

  /*
   *  Helper Functions
   */
  double intToFloat(int c) { return (double)1/255 * c; };
  void win_init (win_t * win, int width, int height);

  /*
   * Render List
   */
  command_type m_draw_commands[MAX_COMMANDS];
  int m_draw_tail = 0;

  /*
   *  Image List
   */
  image_type m_image_list[MAX_IMAGES];
  int m_image_tail = 0;
};

#endif
