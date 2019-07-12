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

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "renderer_cairo.h"
#include "events_gva.h"

using namespace std;
using namespace gva;

static CallbackType callback_;
static void *arg_;

rendererCairo::rendererCairo (int width, int height)
{
  scale_ = 1.0;
  forground_colour_ = { 255, 255, 255};
  background_colour_ = {0, 0, 0};
  height_ = height;
  width_ = width;
  texture_ = 0;
  image_tail_ = 0;
}

rendererCairo::~rendererCairo ()
{
  if (texture_) {
    free(texture_);
    texture_ = 0;
  }
  cairo_destroy (render_.cr);

  CloseWindow();
}

void
rendererCairo::draw ()
{
  int count = 0;
  cairo_surface_t *surface;
  double dashed[] = { 1.0 };

  cairo_t *cr = render_.cr;

  /* Push the render to prevent flicker, flush when done */
  cairo_push_group (cr);
  for (count = 0; count < draw_tail_; count++)
    {
      command_type *currentCmd = &draw_commands_[count];
      switch (currentCmd->command)
        {
        case COMMAND_CIRCLE:
          cairo_save (cr);
          cairo_new_path (cr);
          cairo_translate (cr, (int) currentCmd->points[0].x,
                           (int) currentCmd->points[0].y);
          cairo_arc (cr, 0, 0, currentCmd->radius, 0, 2 * M_PI);
          cairo_close_path (cr);
          if (currentCmd->fill)
            {
              cairo_set_source_rgb (cr, intToFloat (background_colour_.red),
                                    intToFloat (background_colour_.green),
                                    intToFloat (background_colour_.blue));
              cairo_fill_preserve (cr);
              cairo_set_source_rgb (cr, intToFloat (forground_colour_.red),
                                    intToFloat (forground_colour_.green),
                                    intToFloat (forground_colour_.blue));
              cairo_stroke (cr);
            }
          else
            {
              cairo_stroke (cr);
            }
          cairo_restore (cr);
          break;
        case COMMAND_COLOUR_BG:
          background_colour_.red = currentCmd->arg1;
          background_colour_.green = currentCmd->arg2;
          background_colour_.blue = currentCmd->arg3;
          break;
        case COMMAND_COLOUR_FG:
          forground_colour_.red = currentCmd->arg1;
          forground_colour_.green = currentCmd->arg2;
          forground_colour_.blue = currentCmd->arg3;
          break;
        case COMMAND_PEN_MOVE:
          cairo_move_to (cr, currentCmd->points[0].x,
                         currentCmd->points[0].y);
          break;
        case COMMAND_PEN_DRAW:
          cairo_line_to (cr, currentCmd->points[0].x,
                         currentCmd->points[0].y);
          if (currentCmd->arg1)
            cairo_stroke (cr);
          break;
        case COMMAND_PEN_LINE:
          cairo_move_to (cr, currentCmd->points[0].x,
                         currentCmd->points[0].y);
          cairo_line_to (cr, currentCmd->points[1].x,
                         currentCmd->points[1].y);
          cairo_stroke (cr);
          break;
        case COMMAND_PEN_RECTANGLE:
          cairo_new_path (cr);
          cairo_move_to (cr, currentCmd->points[0].x,
                         currentCmd->points[0].y);
          cairo_rel_line_to (cr,
                             currentCmd->points[1].x -
                             currentCmd->points[0].x, 0);
          cairo_rel_line_to (cr, 0,
                             currentCmd->points[1].y -
                             currentCmd->points[0].y);
          cairo_rel_line_to (cr,
                             currentCmd->points[0].x -
                             currentCmd->points[1].x, 0);
          cairo_rel_line_to (cr, 0,
                             currentCmd->points[0].y -
                             currentCmd->points[1].y);
          cairo_close_path (cr);
#if 1
          if (currentCmd->fill)
            {
              cairo_set_source_rgb (cr, intToFloat (background_colour_.red),
                                    intToFloat (background_colour_.green),
                                    intToFloat (background_colour_.blue));
              cairo_fill_preserve (cr);
              cairo_set_source_rgb (cr, intToFloat (forground_colour_.red),
                                    intToFloat (forground_colour_.green),
                                    intToFloat (forground_colour_.blue));
              cairo_stroke (cr);
            }
          else
            {
              cairo_stroke (cr);
            }
#endif
          break;
        case COMMAND_PEN_ROUNDED_RECTANGLE:
          {
            double width = currentCmd->arg1;
            double height = currentCmd->arg2;
            double corner_radius = (double) currentCmd->arg3;   /* and corner curvature radius */
            double x = currentCmd->points[0].x;
            double y = currentCmd->points[0].y;
            double radius = corner_radius;
            double degrees = M_PI / 180.0;

            cairo_new_sub_path (cr);
            cairo_arc (cr, x + width - radius, y - radius, radius,
                       0 * degrees, 90 * degrees);
            cairo_arc (cr, x + radius, y - radius, radius, 90 * degrees,
                       180 * degrees);
            cairo_arc (cr, x + radius, y - height + radius, radius,
                       180 * degrees, 270 * degrees);
            cairo_arc (cr, x + width - radius, y - height + radius, radius,
                       270 * degrees, 0 * degrees);
            cairo_close_path (cr);
          }
          if (currentCmd->fill)
            {
              cairo_set_source_rgb (cr, intToFloat (background_colour_.red),
                                    intToFloat (background_colour_.green),
                                    intToFloat (background_colour_.blue));
              cairo_fill_preserve (cr);
              cairo_set_source_rgb (cr, intToFloat (forground_colour_.red),
                                    intToFloat (forground_colour_.green),
                                    intToFloat (forground_colour_.blue));
              cairo_stroke (cr);
            }
          break;
        case COMMAND_PEN_TRIANGLE:
          cairo_new_path (cr);
          cairo_move_to (cr, currentCmd->points[0].x,
                         currentCmd->points[0].y);
          cairo_rel_line_to (cr,
                             currentCmd->points[1].x -
                             currentCmd->points[0].x,
                             currentCmd->points[1].y -
                             currentCmd->points[0].y);
          cairo_rel_line_to (cr,
                             currentCmd->points[2].x -
                             currentCmd->points[1].x,
                             currentCmd->points[2].y -
                             currentCmd->points[1].y);
          cairo_close_path (cr);

          if (currentCmd->fill)
            {
              cairo_set_source_rgb (cr, intToFloat (background_colour_.red),
                                    intToFloat (background_colour_.green),
                                    intToFloat (background_colour_.blue));
              cairo_fill_preserve (cr);
              cairo_set_source_rgb (cr, intToFloat (forground_colour_.red),
                                    intToFloat (forground_colour_.green),
                                    intToFloat (forground_colour_.blue));
              cairo_stroke (cr);
            }
          else
            {
              cairo_stroke (cr);
            }
          break;
        case COMMAND_PEN_COLOUR:
          cairo_set_source_rgb (cr, intToFloat (currentCmd->colour.red),
                                intToFloat (currentCmd->colour.green),
                                intToFloat (currentCmd->colour.blue));
          break;
        case COMMAND_PEN_THICKNESS:
          cairo_set_line_width (cr, (cairo_line_join_t) currentCmd->arg1);
          switch (currentCmd->arg2)
            {
            case LINE_DASHED:
              cairo_set_dash (cr, dashed, 1, 0);
              break;
            case LINE_SOLID:
            default:
              cairo_set_dash (cr, 0, 0, 0);
              break;
            }
          break;
        case COMMAND_LINE_JOIN:
          cairo_set_line_join (cr, (cairo_line_join_t) currentCmd->arg1);
          break;
        case COMMAND_IMAGE_TEXTURE:
          cairo_set_source_surface (cr, image_list_[currentCmd->arg1].image,
                                    currentCmd->points[0].x,
                                    currentCmd->points[0].y);
          cairo_paint (cr);
          cairo_surface_destroy(image_list_[currentCmd->arg1].image);
          break;
        case COMMAND_IMAGE_TEXTURE_PERSIST:
          cairo_set_source_surface (cr, image_list_[currentCmd->arg1].image,
                                    currentCmd->points[0].x,
                                    currentCmd->points[0].y);
          cairo_paint (cr);
          break;
        case COMMAND_TEXT_FONT:
          cairo_select_font_face (cr,
                                  currentCmd->text,
                                  (cairo_font_slant_t) currentCmd->arg1,
                                  (cairo_font_weight_t) currentCmd->arg2);
        case COMMAND_TEXT:
          cairo_save (cr);
          cairo_set_font_size (cr, currentCmd->arg1);
          cairo_move_to (cr, currentCmd->points[0].x,
                         currentCmd->points[0].y);
          cairo_show_text (cr, currentCmd->text);
          cairo_restore (cr);
          break;
        default:
          break;
        }
    }
  /* Pop the group and flush to display on the screen */
  cairo_pop_group_to_source (cr);
  cairo_paint (cr);
}

int
rendererCairo::init (int width, int height, CallbackType cb, void *arg)
{
  render_.size.width = width;
  render_.size.height = height;
  width_ = width;
  height_ = height;
  callback_ = cb;
  arg_ = arg;

  int status;

  render_.surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
                            width, height);

  render_.win.app = gtk_application_new ("org.gtk.vivoe-lite-hmi", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (render_.win.app, "activate", G_CALLBACK (Activate), NULL);
  g_timeout_add (40, Callback, NULL);
  status = g_application_run (G_APPLICATION (render_.win.app), 0, 0);
  g_object_unref (render_.win.app);

  return 0;
}

void
rendererCairo::setPixel (int x, int y)
{
}

void
rendererCairo::setColour (int red, int green, int blue)
{
  setColourForground (red, green, blue);
  setColourBackground (red, green, blue);
}

void
rendererCairo::setColourForground (int red, int green, int blue)
{
  draw_commands_[draw_tail_].command = COMMAND_COLOUR_FG;
  draw_commands_[draw_tail_].arg1 = red;
  draw_commands_[draw_tail_].arg2 = green;
  draw_commands_[draw_tail_].arg3 = blue;
  draw_tail_++;
}

void
rendererCairo::setColourBackground (int red, int green, int blue)
{
  draw_commands_[draw_tail_].command = COMMAND_COLOUR_BG;
  draw_commands_[draw_tail_].arg1 = red;
  draw_commands_[draw_tail_].arg2 = green;
  draw_commands_[draw_tail_].arg3 = blue;
  draw_tail_++;
}

void
rendererCairo::setLineType (int type)
{
  draw_commands_[draw_tail_].command = COMMAND_LINE_JOIN;
  draw_commands_[draw_tail_].arg1 = type;
  draw_tail_++;
}

void
rendererCairo::setLineThickness (int thickness, lineType fill)
{
  draw_commands_[draw_tail_].command = COMMAND_PEN_THICKNESS;
  draw_commands_[draw_tail_].arg1 = thickness;
  draw_commands_[draw_tail_].arg2 = fill;
  draw_tail_++;
}

int
rendererCairo::movePen (int x, int y)
{
  y = render_.size.height - y;

  draw_commands_[draw_tail_].command = COMMAND_PEN_MOVE;
  draw_commands_[draw_tail_].points[0].x = x;
  draw_commands_[draw_tail_].points[0].y = y;
  draw_tail_++;
  return 0;
}

int
rendererCairo::drawPen (int x, int y, bool close)
{
  y = render_.size.height - y;

  draw_commands_[draw_tail_].command = COMMAND_PEN_DRAW;
  draw_commands_[draw_tail_].points[0].x = x;
  draw_commands_[draw_tail_].points[0].y = y;
  draw_commands_[draw_tail_].arg1 = close;
  draw_tail_++;
  return 0;

}

int
rendererCairo::drawLine (int x1, int y1, int x2, int y2)
{
  y1 = render_.size.height - y1;
  y2 = render_.size.height - y2;
  draw_commands_[draw_tail_].command = COMMAND_PEN_LINE;
  draw_commands_[draw_tail_].points[0].x = x1;
  draw_commands_[draw_tail_].points[0].y = y1;
  draw_tail_++;
  return 0;
}

void
rendererCairo::drawCircle (int x, int y, int radius, bool fill)
{
  y = render_.size.height - y;
  draw_commands_[draw_tail_].command = COMMAND_CIRCLE;
  draw_commands_[draw_tail_].points[0].x = x;
  draw_commands_[draw_tail_].points[0].y = y;
  draw_commands_[draw_tail_].radius = radius;
  draw_commands_[draw_tail_].points[1].x = 1;
  draw_commands_[draw_tail_].points[1].y = 0;
  draw_commands_[draw_tail_].fill = fill ? 1 : 0;
  draw_tail_++;
}

void
rendererCairo::drawRectangle (int x1, int y1, int x2, int y2,
                              bool fill)
{
  y1 = render_.size.height - y1;
  y2 = render_.size.height - y2;

  draw_commands_[draw_tail_].command = COMMAND_PEN_RECTANGLE;
  draw_commands_[draw_tail_].points[0].x = x1;
  draw_commands_[draw_tail_].points[0].y = y1;
  draw_commands_[draw_tail_].points[1].x = x2;
  draw_commands_[draw_tail_].points[1].y = y2;
  draw_commands_[draw_tail_].fill = fill ? 1 : 0;
  draw_tail_++;
}

void
rendererCairo::drawRoundedRectangle (int x, int y, int width,
                                     int height, int courner, bool fill)
{
  y = render_.size.height - y;

  draw_commands_[draw_tail_].command = COMMAND_PEN_ROUNDED_RECTANGLE;
  draw_commands_[draw_tail_].points[0].x = x;
  draw_commands_[draw_tail_].points[0].y = y;
  draw_commands_[draw_tail_].arg1 = width;
  draw_commands_[draw_tail_].arg2 = height;
  draw_commands_[draw_tail_].arg3 = courner;
  draw_commands_[draw_tail_].fill = fill ? 1 : 0;
  draw_tail_++;
}

void
rendererCairo::drawTriangle (int x1, int y1, int x2, int y2,
                             int x3, int y3, bool fill)
{
  y1 = render_.size.height - y1;
  y2 = render_.size.height - y2;
  y3 = render_.size.height - y3;

  draw_commands_[draw_tail_].command = COMMAND_PEN_TRIANGLE;
  draw_commands_[draw_tail_].points[0].x = x1;
  draw_commands_[draw_tail_].points[0].y = y1;
  draw_commands_[draw_tail_].points[1].x = x2;
  draw_commands_[draw_tail_].points[1].y = y2;
  draw_commands_[draw_tail_].points[2].x = x3;
  draw_commands_[draw_tail_].points[2].y = y3;
  draw_commands_[draw_tail_].fill = fill ? 1 : 0;
  draw_tail_++;
}

int
rendererCairo::drawColor (int r, int g, int b)
{
  draw_commands_[draw_tail_].command = COMMAND_PEN_COLOUR;
  draw_commands_[draw_tail_].colour.red = r;
  draw_commands_[draw_tail_].colour.green = g;
  draw_commands_[draw_tail_].colour.blue = b;
  draw_tail_++;
  return 0;
}

void
rendererCairo::setTextFont (int slope, int weight, char *fontName)
{
  draw_commands_[draw_tail_].command = COMMAND_TEXT_FONT;
  draw_commands_[draw_tail_].arg1 = slope;
  draw_commands_[draw_tail_].arg2 = weight;
  strcpy (draw_commands_[draw_tail_].text, fontName);
  draw_tail_++;
}

int
rendererCairo::getTextWidth (char *str, int fontSize)
{
  cairo_t *cr = render_.cr;
  cairo_text_extents_t extents;
  cairo_select_font_face(cr, "Courier", CAIRO_FONT_SLANT_NORMAL,
      CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size (cr, fontSize);
  cairo_text_extents (cr, str, &extents);
  return extents.x_advance;
}

int
rendererCairo::getTextHeight (char *str, int fontSize)
{
  cairo_t *cr = render_.cr;
  cairo_text_extents_t extents;

  cairo_set_font_size (cr, fontSize);
  cairo_text_extents (cr, str, &extents);
  return extents.height;
}

void
rendererCairo::drawText (int x, int y, char *text, int size)
{
  y = render_.size.height - y;

  draw_commands_[draw_tail_].command = COMMAND_TEXT;
  draw_commands_[draw_tail_].points[0].x = x;
  draw_commands_[draw_tail_].points[0].y = y;
  draw_commands_[draw_tail_].arg1 = size;
  strcpy (draw_commands_[draw_tail_].text, text);
  draw_tail_++;
}

void
rendererCairo::drawLabel (int x, int y, char *text, int size)
{
  y = render_.size.height - y;

  draw_commands_[draw_tail_].command = COMMAND_TEXT;
  draw_commands_[draw_tail_].points[0].x = x;
  draw_commands_[draw_tail_].points[0].y = y;
  draw_commands_[draw_tail_].arg1 = size;
  strcpy (draw_commands_[draw_tail_].text, text);
  draw_tail_++;
}

void
rendererCairo::drawTextCentre (int x, char *text, int size)
{
  drawText (x, 200, text, size);
}

int
rendererCairo::textureRGB (int x, int y, void *buffer, char *file)
{
  strcpy (image_list_[image_tail_].name, file);

  image_list_[image_tail_].image =
    cairo_image_surface_create_from_png (file);

  draw_commands_[draw_tail_].command = COMMAND_IMAGE_TEXTURE;
  draw_commands_[draw_tail_].points[0].x = x;
  draw_commands_[draw_tail_].points[0].y = y;
  draw_commands_[draw_tail_].arg1 = image_tail_++;

  draw_tail_++;

  return 0;
}

int
rendererCairo::textureRGB (int x, int y, void *buffer)
{
  image_list_[image_tail_].image =
    cairo_image_surface_create_for_data ((unsigned char*)buffer, CAIRO_FORMAT_RGB24, width_, height_,
                                     cairo_format_stride_for_width(CAIRO_FORMAT_RGB24, width_));

  draw_commands_[draw_tail_].command = COMMAND_IMAGE_TEXTURE;
  draw_commands_[draw_tail_].points[0].x = x;
  draw_commands_[draw_tail_].points[0].y = y;
  draw_commands_[draw_tail_].arg1 = image_tail_++;

  draw_tail_++;

  return 0;
}

int 
rendererCairo::textureRGB (int x, int y, cairo_surface_t *surface) {
  image_list_[image_tail_].image = surface;

  draw_commands_[draw_tail_].command = COMMAND_IMAGE_TEXTURE_PERSIST;
  draw_commands_[draw_tail_].points[0].x = x;
  draw_commands_[draw_tail_].points[0].y = y;
  draw_commands_[draw_tail_].arg1 = image_tail_++;

  draw_tail_++;

  return 0;
}


void
rendererCairo::scale (float x)
{
}

void
rendererCairo::present ()
{
}

//
// Redraw the screen from the surface. Note that the ::draw
// signal receives a ready-to-be-used cairo_t that is already
// clipped to only draw the exposed areas of the widget
//
gboolean
rendererCairo::DrawCb (GtkWidget *widget, cairo_t   *cr, gpointer   data) {
  cairo_set_source_surface (cr, render_.surface, 0, 0);
  cairo_paint (cr);

// printf("File %s, Function %s, Line %d\n", __FILE__, __FUNCTION__, __LINE__); 
  gtk_widget_queue_draw(widget) ;
  return FALSE;
}

// Create a new surface of the appropriate size to store our HMI 
gboolean
rendererCairo::ConfigureEventCb (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
  if (render_.surface)
    cairo_surface_destroy (render_.surface);

  render_.surface = gdk_window_create_similar_surface (gtk_widget_get_window (widget),
                                               CAIRO_CONTENT_COLOR,
                                               gtk_widget_get_allocated_width (widget),
                                               gtk_widget_get_allocated_height (widget));
 printf("File %s, Function %s, Line %d\n", __FILE__, __FUNCTION__, __LINE__); 
  render_.cr = cairo_create (render_.surface);
  gtk_widget_queue_draw(widget) ;

#if 0
  cairo_t *cr;

  cr = cairo_create (render_.surface);

  cairo_set_source_rgb (cr, 0, 1, 1);
  cairo_paint (cr);

  cairo_destroy (cr);
#endif

  // We've handled the configure event, no need for further processing. 
  return TRUE;
}

void
rendererCairo::Activate (GtkApplication *app, gpointer user_data)
{
  GtkWidget *frame;

  render_.win.win = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (render_.win.win), "HMI vivoe-lite");

  g_signal_connect (render_.win.win, "destroy", G_CALLBACK (CloseWindow), NULL);

  gtk_container_set_border_width (GTK_CONTAINER (render_.win.win), 0);

  frame = gtk_frame_new (NULL);
//  gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
  gtk_container_add (GTK_CONTAINER (render_.win.win), frame);

  render_.win.draw = gtk_drawing_area_new ();
  // set a minimum size 
  gtk_widget_set_size_request (render_.win.draw, MIN_WIDTH, MIN_HEIGHT );

  gtk_container_add (GTK_CONTAINER (frame), render_.win.draw);

  //
  // Event signals 
  //
  g_signal_connect (render_.win.draw, "button-press-event",
                    G_CALLBACK (EventsGva::ButtonPressEventCb), NULL);
  g_signal_connect (render_.win.win, "key-press-event",
                    G_CALLBACK (EventsGva::KeyPressEventCb), NULL);
//  g_signal_connect (render_.win.win, "key-release-event",
//                    G_CALLBACK (EventsGva::KeyPressEventCb), NULL);

  // Ask to receive events the drawing area doesn't normally
  // subscribe to. In particular, we need to ask for the
  // button press and motion notify events that want to handle.
  //
  gtk_widget_set_events (render_.win.draw, gtk_widget_get_events (render_.win.draw)
                                     | GDK_BUTTON_PRESS_MASK
                                     | GDK_KEY_PRESS_MASK
                                     | GDK_KEY_RELEASE_MASK
                                     | GDK_POINTER_MOTION_MASK);
                                       
  //
  // Signals used to handle the backing surface 
  //
  g_signal_connect (render_.win.draw, "draw",
                    G_CALLBACK (DrawCb), NULL);
  g_signal_connect (render_.win.draw,"configure-event",
                    G_CALLBACK (ConfigureEventCb), NULL);

  gtk_widget_show_all (render_.win.win);  
}

gboolean 
rendererCairo::Callback (gpointer user_data) {
  callback_(arg_);
}

void
rendererCairo::CloseWindow (void)
{
  if (render_.surface)
    cairo_surface_destroy (render_.surface);
}
