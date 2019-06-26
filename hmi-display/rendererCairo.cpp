#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "rendererCairo.hpp"

using namespace std;

void
rendererCairo::draw (int handle)
{
  int count = 0;
  cairo_surface_t *surface;
  double dashed[] = { 1.0 };

  cairo_t *cr = m_render_handle[handle].cr;

  /* Push the render to prevent flicker, flush when done */
  cairo_push_group (cr);
  for (count = 0; count < m_draw_tail; count++)
    {
      command_type *currentCmd = &m_draw_commands[count];
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
              cairo_set_source_rgb (cr, intToFloat (m_background_colour.red),
                                    intToFloat (m_background_colour.green),
                                    intToFloat (m_background_colour.blue));
              cairo_fill_preserve (cr);
              cairo_set_source_rgb (cr, intToFloat (m_forground_colour.red),
                                    intToFloat (m_forground_colour.green),
                                    intToFloat (m_forground_colour.blue));
              cairo_stroke (cr);
            }
          else
            {
              cairo_stroke (cr);
            }
          cairo_restore (cr);
          break;
        case COMMAND_COLOUR_BG:
          m_background_colour.red = currentCmd->arg1;
          m_background_colour.green = currentCmd->arg2;
          m_background_colour.blue = currentCmd->arg3;
          break;
        case COMMAND_COLOUR_FG:
          m_forground_colour.red = currentCmd->arg1;
          m_forground_colour.green = currentCmd->arg2;
          m_forground_colour.blue = currentCmd->arg3;
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
              cairo_set_source_rgb (cr, intToFloat (m_background_colour.red),
                                    intToFloat (m_background_colour.green),
                                    intToFloat (m_background_colour.blue));
              cairo_fill_preserve (cr);
              cairo_set_source_rgb (cr, intToFloat (m_forground_colour.red),
                                    intToFloat (m_forground_colour.green),
                                    intToFloat (m_forground_colour.blue));
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
              cairo_set_source_rgb (cr, intToFloat (m_background_colour.red),
                                    intToFloat (m_background_colour.green),
                                    intToFloat (m_background_colour.blue));
              cairo_fill_preserve (cr);
              cairo_set_source_rgb (cr, intToFloat (m_forground_colour.red),
                                    intToFloat (m_forground_colour.green),
                                    intToFloat (m_forground_colour.blue));
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
              cairo_set_source_rgb (cr, intToFloat (m_background_colour.red),
                                    intToFloat (m_background_colour.green),
                                    intToFloat (m_background_colour.blue));
              cairo_fill_preserve (cr);
              cairo_set_source_rgb (cr, intToFloat (m_forground_colour.red),
                                    intToFloat (m_forground_colour.green),
                                    intToFloat (m_forground_colour.blue));
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
          cairo_set_source_surface (cr, m_image_list[currentCmd->arg1].image,
                                    currentCmd->points[0].x,
                                    currentCmd->points[0].y);
          cairo_paint (cr);
          cairo_surface_destroy(m_image_list[currentCmd->arg1].image);
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

rendererCairo::rendererCairo (int width, int height)
{
  m_scale = 1.0;
  m_forground_colour = { 255, 255, 255};
  m_background_colour = {0, 0, 0};
  m_height = height;
  m_width = width;
  m_texture = 0;
  m_image_tail = 0;
}

rendererCairo::~rendererCairo ()
{
  if (m_texture) {
    free(m_texture);
    m_texture = 0;
  }
  cairo_destroy (m_render_handle[0].cr);

  XCloseDisplay (m_render_handle[0].win.dpy);
}

int
rendererCairo::init (int width, int height)
{
  m_render_handle[0].size.width = width;
  m_render_handle[0].size.height = height;

  if (XInitThreads () == 0)
    {
      printf ("Error setting XInitThreads\n");
      return -1;
    }
    
  /* open connection with the server */
  m_render_handle[0].win.dpy = XOpenDisplay (NULL);
  if (m_render_handle[0].win.dpy == NULL)
    {
      fprintf (stderr, "Cannot open display\n");
      exit (1);
    }

  m_height = height;
  m_width = width;

  win_init (&m_render_handle[0].win, width, height);

  Visual *visual = DefaultVisual (m_render_handle[0].win.dpy,
                                  DefaultScreen (m_render_handle[0].win.dpy));

  XClearWindow (m_render_handle[0].win.dpy, m_render_handle[0].win.win);

  m_render_handle[0].surface =
    cairo_xlib_surface_create (m_render_handle[0].win.dpy,
                               m_render_handle[0].win.win, visual,
                               m_render_handle[0].win.width,
                               m_render_handle[0].win.height);
  m_render_handle[0].cr = cairo_create (m_render_handle[0].surface);

  cairo_set_source_rgb (m_render_handle[0].cr, 1, 1, 1);

  return 0;
}

void
rendererCairo::setPixel (int handle, int x, int y)
{
}

void
rendererCairo::setColour (int handle, int red, int green, int blue)
{
  setColourForground (handle, red, green, blue);
  setColourBackground (handle, red, green, blue);
}

void
rendererCairo::setColourForground (int handle, int red, int green, int blue)
{
  m_draw_commands[m_draw_tail].command = COMMAND_COLOUR_FG;
  m_draw_commands[m_draw_tail].arg1 = red;
  m_draw_commands[m_draw_tail].arg2 = green;
  m_draw_commands[m_draw_tail].arg3 = blue;
  m_draw_tail++;
}

void
rendererCairo::setColourBackground (int handle, int red, int green, int blue)
{
  m_draw_commands[m_draw_tail].command = COMMAND_COLOUR_BG;
  m_draw_commands[m_draw_tail].arg1 = red;
  m_draw_commands[m_draw_tail].arg2 = green;
  m_draw_commands[m_draw_tail].arg3 = blue;
  m_draw_tail++;
}

void
rendererCairo::setLineType (int handle, int type)
{
  m_draw_commands[m_draw_tail].command = COMMAND_LINE_JOIN;
  m_draw_commands[m_draw_tail].arg1 = type;
  m_draw_tail++;
}

void
rendererCairo::setLineThickness (int handle, int thickness, lineType fill)
{
  m_draw_commands[m_draw_tail].command = COMMAND_PEN_THICKNESS;
  m_draw_commands[m_draw_tail].arg1 = thickness;
  m_draw_commands[m_draw_tail].arg2 = fill;
  m_draw_tail++;
}

int
rendererCairo::movePen (int handle, int x, int y)
{
  y = m_render_handle[handle].size.height - y;

  m_draw_commands[m_draw_tail].command = COMMAND_PEN_MOVE;
  m_draw_commands[m_draw_tail].points[0].x = x;
  m_draw_commands[m_draw_tail].points[0].y = y;
  m_draw_tail++;
  return 0;
}

int
rendererCairo::drawPen (int handle, int x, int y, bool close)
{
  y = m_render_handle[handle].size.height - y;

  m_draw_commands[m_draw_tail].command = COMMAND_PEN_DRAW;
  m_draw_commands[m_draw_tail].points[0].x = x;
  m_draw_commands[m_draw_tail].points[0].y = y;
  m_draw_commands[m_draw_tail].arg1 = close;
  m_draw_tail++;
  return 0;

}

int
rendererCairo::drawLine (int handle, int x1, int y1, int x2, int y2)
{
  y1 = m_render_handle[handle].size.height - y1;
  y2 = m_render_handle[handle].size.height - y2;
  m_draw_commands[m_draw_tail].command = COMMAND_PEN_LINE;
  m_draw_commands[m_draw_tail].points[0].x = x1;
  m_draw_commands[m_draw_tail].points[0].y = y1;
  m_draw_tail++;
  return 0;
}

void
rendererCairo::drawCircle (int handle, int x, int y, int radius, bool fill)
{
  y = m_render_handle[handle].size.height - y;
  m_draw_commands[m_draw_tail].command = COMMAND_CIRCLE;
  m_draw_commands[m_draw_tail].points[0].x = x;
  m_draw_commands[m_draw_tail].points[0].y = y;
  m_draw_commands[m_draw_tail].radius = radius;
  m_draw_commands[m_draw_tail].points[1].x = 1;
  m_draw_commands[m_draw_tail].points[1].y = 0;
  m_draw_commands[m_draw_tail].fill = fill ? 1 : 0;
  m_draw_tail++;
}

void
rendererCairo::drawRectangle (int handle, int x1, int y1, int x2, int y2,
                              bool fill)
{
  y1 = m_render_handle[handle].size.height - y1;
  y2 = m_render_handle[handle].size.height - y2;

  m_draw_commands[m_draw_tail].command = COMMAND_PEN_RECTANGLE;
  m_draw_commands[m_draw_tail].points[0].x = x1;
  m_draw_commands[m_draw_tail].points[0].y = y1;
  m_draw_commands[m_draw_tail].points[1].x = x2;
  m_draw_commands[m_draw_tail].points[1].y = y2;
  m_draw_commands[m_draw_tail].fill = fill ? 1 : 0;
  m_draw_tail++;
}

void
rendererCairo::drawRoundedRectangle (int handle, int x, int y, int width,
                                     int height, int courner, bool fill)
{
  y = m_render_handle[handle].size.height - y;

  m_draw_commands[m_draw_tail].command = COMMAND_PEN_ROUNDED_RECTANGLE;
  m_draw_commands[m_draw_tail].points[0].x = x;
  m_draw_commands[m_draw_tail].points[0].y = y;
  m_draw_commands[m_draw_tail].arg1 = width;
  m_draw_commands[m_draw_tail].arg2 = height;
  m_draw_commands[m_draw_tail].arg3 = courner;
  m_draw_commands[m_draw_tail].fill = fill ? 1 : 0;
  m_draw_tail++;
}

void
rendererCairo::drawTriangle (int handle, int x1, int y1, int x2, int y2,
                             int x3, int y3, bool fill)
{
  y1 = m_render_handle[handle].size.height - y1;
  y2 = m_render_handle[handle].size.height - y2;
  y3 = m_render_handle[handle].size.height - y3;

  m_draw_commands[m_draw_tail].command = COMMAND_PEN_TRIANGLE;
  m_draw_commands[m_draw_tail].points[0].x = x1;
  m_draw_commands[m_draw_tail].points[0].y = y1;
  m_draw_commands[m_draw_tail].points[1].x = x2;
  m_draw_commands[m_draw_tail].points[1].y = y2;
  m_draw_commands[m_draw_tail].points[2].x = x3;
  m_draw_commands[m_draw_tail].points[2].y = y3;
  m_draw_commands[m_draw_tail].fill = fill ? 1 : 0;
  m_draw_tail++;
}

int
rendererCairo::drawColor (int handle, int r, int g, int b)
{
  m_draw_commands[m_draw_tail].command = COMMAND_PEN_COLOUR;
  m_draw_commands[m_draw_tail].colour.red = r;
  m_draw_commands[m_draw_tail].colour.green = g;
  m_draw_commands[m_draw_tail].colour.blue = b;
  m_draw_tail++;
  return 0;
}

void
rendererCairo::setTextFont (int handle, int slope, int weight, char *fontName)
{
  m_draw_commands[m_draw_tail].command = COMMAND_TEXT_FONT;
  m_draw_commands[m_draw_tail].arg1 = slope;
  m_draw_commands[m_draw_tail].arg2 = weight;
  strcpy (m_draw_commands[m_draw_tail].text, fontName);
  m_draw_tail++;
}

int
rendererCairo::getTextWidth (int handle, char *str, int fontSize)
{
  cairo_t *cr = m_render_handle[handle].cr;
  cairo_text_extents_t extents;

  cairo_set_font_size (cr, fontSize);
  cairo_text_extents (cr, str, &extents);
  return extents.x_advance;
}

int
rendererCairo::getTextHeight (int handle, char *str, int fontSize)
{
  cairo_t *cr = m_render_handle[handle].cr;
  cairo_text_extents_t extents;

  cairo_set_font_size (cr, fontSize);
  cairo_text_extents (cr, str, &extents);
  return extents.height;
}

void
rendererCairo::drawText (int handle, int x, int y, char *text, int size)
{
  y = m_render_handle[handle].size.height - y;

  m_draw_commands[m_draw_tail].command = COMMAND_TEXT;
  m_draw_commands[m_draw_tail].points[0].x = x;
  m_draw_commands[m_draw_tail].points[0].y = y;
  m_draw_commands[m_draw_tail].arg1 = size;
  strcpy (m_draw_commands[m_draw_tail].text, text);
  m_draw_tail++;
}

void
rendererCairo::drawTextCentre (int handle, int x, char *text, int size)
{
  drawText (handle, x, 200, text, size);
}

int
rendererCairo::textureRGB (int handle, int x, int y, void *buffer, char *file)
{
  strcpy (m_image_list[m_image_tail].name, file);

  m_image_list[m_image_tail].image =
    cairo_image_surface_create_from_png (file);

  m_draw_commands[m_draw_tail].command = COMMAND_IMAGE_TEXTURE;
  m_draw_commands[m_draw_tail].points[0].x = x;
  m_draw_commands[m_draw_tail].points[0].y = y;
  m_draw_commands[m_draw_tail].arg1 = m_image_tail++;

  m_draw_tail++;

  return 0;
}

int
rendererCairo::textureRGB (int handle, int x, int y, void *buffer)
{
  m_image_list[m_image_tail].image =
    cairo_image_surface_create_for_data ((unsigned char*)buffer, CAIRO_FORMAT_RGB24, m_width, m_height,
                                     cairo_format_stride_for_width(CAIRO_FORMAT_RGB24, m_width));

  m_draw_commands[m_draw_tail].command = COMMAND_IMAGE_TEXTURE;
  m_draw_commands[m_draw_tail].points[0].x = x;
  m_draw_commands[m_draw_tail].points[0].y = y;
  m_draw_commands[m_draw_tail].arg1 = m_image_tail++;

  m_draw_tail++;

  return 0;
}

void
rendererCairo::scale (int handle, float x)
{
}

void
rendererCairo::present (int handle)
{
}

void
rendererCairo::win_init (win_t * win, int width, int height)
{
  win->width = width;
  win->height = height;

  m_root = DefaultRootWindow (win->dpy);
  win->scr = DefaultScreen (win->dpy);

  win->win = XCreateSimpleWindow (win->dpy, m_root, 0, 0,
                                  win->width, win->height, 0,
                                  BlackPixel (win->dpy, win->scr),
                                  BlackPixel (win->dpy, win->scr));

  win->quit_code = XKeysymToKeycode (win->dpy, XStringToKeysym ("Q"));

  XSelectInput (win->dpy, win->win,
                KeyPressMask | StructureNotifyMask | ExposureMask);

  XMapWindow (win->dpy, win->win);
}
