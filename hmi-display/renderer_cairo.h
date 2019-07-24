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

#ifndef RENDERER_CAIRO_H
#define RENDERER_CAIRO_H

#include <iostream>
#include <stdio.h>
#include <cairo.h>
#include "gva.h"
#include "renderer.h"

#include <gtk/gtk.h>
typedef struct  { 
  GtkApplication * app; 
  GtkWidget * win;
  GtkWidget * draw; 
  int height;
  int width;
} gtkType; 

#define MAX_COMMANDS 1000
#define MAX_IMAGES 100
#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480

typedef struct win {
    int scr;
    int width;
    int height;
    int win;
} win_t;

struct handle_type
{
  int handle;
  bool inUse;
  bool fullscreen;
  ResolutionType size;
  gtkType win;
  cairo_surface_t *surface;
  cairo_t * cr;  
};

enum draw_type
{
  COMMAND_CIRCLE = 0,
  COMMAND_COLOUR_BG,
  COMMAND_COLOUR_FG,
  COMMAND_IMAGE_TEXTURE,
  COMMAND_IMAGE_TEXTURE_PERSIST,
  COMMAND_LINE_JOIN,
  COMMAND_PEN_COLOUR,
  COMMAND_PEN_CURVE,
  COMMAND_PEN_LINE,
  COMMAND_PEN_DRAW,
  COMMAND_PEN_MOVE,
  COMMAND_PEN_RECTANGLE,
  COMMAND_PEN_ROUNDED_RECTANGLE,
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
  bool from_cache;
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

static handle_type render_;

typedef void (*CallbackType)(void * io, gpointer user_data);

class RendererCairo : public renderer {
public:
  RendererCairo (int width, int height);
  ~RendererCairo ();
  int init (int width, int height, CallbackType cb, void *arg);   
  // Pure Virtual functions
  void setPixel (int x, int y);
  void setColour (int red, int green, int blue);
  void setColourForground (int red, int green, int blue);
  void setColourBackground (int red, int green, int blue);
  void setLineType (int type);
  void setLineThickness (int thickness, lineType fill);
  int movePen (int x, int y);
  int drawPen (int x, int y, bool close);
  int drawLine (int x1, int y1, int x2, int y2);
  void drawCircle (int x, int y, int radius, bool fill);
  void drawRectangle (int x1, int y1, int x2, int y2, bool fill);
  void drawRoundedRectangle (int x1, int y1, int x2, int y2, int courner, bool fill);
  void drawTriangle (int x1, int y1, int x2, int y2, int x3, int y3, bool fill);
  int drawColor (int r, int g, int b);
  void setTextFont (int slope, int weight, char* fontName);
  int getTextWidth (char* str, int fontSize);
  int getTextHeight (char* str, int fontSize);
  void drawText (int x, int y, char* text, int size);
  void drawLabel (int x, int y, char* text, int size);
  void drawTextCentre (int x, char* text, int size);
  int textureRGB (int x, int y, void *buffer, char *file);
  int textureRGB (int x, int y, void *buffer);
  int textureRGB (int x, int y, cairo_surface_t *surface);
  
  // Cairo specific functions
  void draw ();
  void reset () { draw_tail_ = 0; image_tail_ = 0;}
  gtkType* getWindow() { return &render_.win; }
#if X11
  void setHeight(int height) { cairo_xlib_surface_set_size(render_.surface, render_.win.width, height); 
      cairo_scale(render_.cr, 1.0, (double)height / render_.win.height);  render_.win.height = height; }
  void setWidth(int width) { cairo_xlib_surface_set_size(render_.surface, width, render_.win.height); 
      cairo_scale(render_.cr, (double)width / render_.win.width, 1.0);  render_.win.width = width; }
  int getHeight() { return render_.win.height; }
  int getWidth() { return render_.win.width; }
#else
  void setHeight(int height) { height_ = height; gtk_widget_set_size_request(render_.win.draw, (gint)width_, (gint)height_); }
  void setWidth(int width) { width_ = width; gtk_widget_set_size_request(render_.win.draw, (gint)width_, (gint)height_); }
  int getHeight() { gint h,w; gtk_widget_get_size_request(render_.win.draw, &w, &h); return (int)w; }
  int getWidth() { gint h,w; gtk_widget_get_size_request(render_.win.draw, &w, &h); return (int)h; }
#endif
private:
  unsigned char* texture_;
  int current_handle_;
  double scale_;
  gtkType root_;

  //
  // Helper Functions
  //
  double intToFloat(int c) { return (double)1/255 * c; };

  //
  //Render List
  //
  command_type draw_commands_[MAX_COMMANDS];
  int draw_tail_ = 0;

  //
  // Image List
  //
  image_type image_list_[MAX_IMAGES];
  int image_tail_ = 0;
  
  //
  // Image Cache
  //
  image_type cache_image_list_[MAX_IMAGES];
  int cache_image_tail_ = 0;
  
private:
  static gboolean DrawCb (GtkWidget *Widget, cairo_t *cr, gpointer   data);
  static gboolean ConfigureEventCb (GtkWidget *Widget, 
    GdkEventConfigure *event, gpointer data);
  static void Activate (GtkApplication *app, gpointer user_data);
  static gboolean Callback (gpointer user_data);
  static void CloseWindow (void);
};

#endif
