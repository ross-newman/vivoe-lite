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

enum Draw_type
{
  COMMAND_CIRCLE = 0,
  COMMAND_ARC,
  COMMAND_COLOUR_BG,
  COMMAND_COLOUR_FG,
  COMMAND_IMAGE_TEXTURE,
  COMMAND_IMAGE_TEXTURE_PERSIST,
  COMMAND_LINE_JOIN,
  COMMAND_PEN_COLOUR,
  COMMAND_PEN_CURVE,
  COMMAND_PEN_LINE,
  COMMAND_PEN_draw,
  COMMAND_PEN_MOVE,
  COMMAND_PEN_RECTANGLE,
  COMMAND_PEN_ROUNDED_RECTANGLE,
  COMMAND_PEN_THICKNESS,
  COMMAND_PEN_TRIANGLE,
  COMMAND_SAVE,
  COMMAND_RESTORE,
  COMMAND_SCALE,
  COMMAND_TRANSLATE,
  COMMAND_ROTATE,
  COMMAND_CLOSE_PATH,
  COMMAND_TEXT_FONT,
  COMMAND_TEXT
};

struct command_type
{
  Draw_type command;
  double height;
  double width;
  point_type points[3];
  double radius;
  double angle1;
  double angle2;
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

typedef enum {
  LINE_SOLID,
  LINE_DASHED,
  LINE_DASHED_MEDIUM,
  LINE_DASHED_LARGE,
} LineType;

static handle_type render_;

typedef void (*CallbackType)(void * io, gpointer data);

class RendererCairo : public renderer {
public:
  RendererCairo (int width, int height);
  ~RendererCairo ();
  int init (int width, int height, CallbackType cb, void *arg);   
  // Pure Virtual functions
  void SetPixel (int x, int y);
  void SetColour (int red, int green, int blue);
  void SetColourForground (int red, int green, int blue);
  void SetColourBackground (int red, int green, int blue);
  void setLineType (int type);
  void SetLineThickness (int thickness, LineType fill);
  int MovePen (int x, int y);
  int DrawPen (int x, int y, bool close);
  int DrawPen (int x, int y) { return DrawPen(x,y,false); };
  int MovePenRaw (int x, int y);
  int DrawPenRaw (int x, int y);
  void DrawArcRaw(int x, int y, int radius, int angle1, int angle2);
  int DrawLine (int x1, int y1, int x2, int y2);
  void Save ();
  void Restore ();
  void Scale (double x, double y);
  void Translate (int x, int y);
  void Rotate (double radians);
  int ClosePath(bool fill);
  void DrawCircle (int x, int y, int radius, bool fill);
  void DrawRectangle (int x1, int y1, int x2, int y2, bool fill);
  void DrawRoundedRectangle (int x1, int y1, int x2, int y2, int courner, bool fill);
  void DrawTriangle (int x1, int y1, int x2, int y2, int x3, int y3, bool fill);
  int DrawColor (int r, int g, int b);
  void SetTextFont (int slope, int weight, char* fontName);
  int GetTextWidth (char* str, int fontSize);
  int GetTextHeight (char* str, int fontSize);
  void DrawText (int x, int y, char* text, int size);
  void DrawLabel (int x, int y, char* text, int size);
  void DrawTextCentre (int x, char* text, int size);
  int TextureRGB (int x, int y, void *buffer, char *file);
  int TextureRGB (int x, int y, void *buffer);
  int TextureRGB (int x, int y, cairo_surface_t *surface);
  
  // Cairo specific functions
  void Draw ();
  void Reset () { draw_tail_ = 0; image_tail_ = 0;}
  gtkType* GetWindow() { return &render_.win; }

  void SetHeight(int height) { height_ = height; gtk_widget_set_size_request(render_.win.draw, (gint)width_, (gint)height_); }
  void SetWidth(int width) { width_ = width; gtk_widget_set_size_request(render_.win.draw, (gint)width_, (gint)height_); }
  int GetHeight() { gint h,w; gtk_widget_get_size_request(render_.win.draw, &w, &h); return (int)w; }
  int GetWidth() { gint h,w; gtk_widget_get_size_request(render_.win.draw, &w, &h); return (int)h; }
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
  command_type Draw_commands_[MAX_COMMANDS];
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
  static gboolean DrawCb (GtkWidget *Widget, cairo_t *cr, gpointer data);
  static gboolean ConfigureEventCb (GtkWidget *Widget, 
    GdkEventConfigure *event, gpointer data);
  static void Activate (GtkApplication *app, gpointer user_data);
  static gboolean Callback (gpointer user_data);
  static void CloseWindow (void);
};

#endif
