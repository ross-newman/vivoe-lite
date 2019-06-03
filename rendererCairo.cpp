#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#include "rendererCairo.h"

using namespace std;

// apt-get install libcairo2-dev
// apt-get install libxt-dev

#define SIZE 10

void
rendererCairo::draw (int handle)
{
  int count = 0;

  m_scale = m_render_handle[handle].win.width / (double)640;

  cairo_t *cr = m_render_handle[handle].cr;
  //cairo_close_path (cr);

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
          break;
        case COMMAND_LINE_JOIN:
          cairo_set_line_join (cr, (cairo_line_join_t) currentCmd->arg1);
          break;
        case COMMAND_IMAGE_PNG:
          cairo_set_source_surface (cr, m_image_list[currentCmd->arg1].image,
                                    currentCmd->points[0].x,
                                    currentCmd->points[0].y);
          cairo_paint (cr);
//        cairo_surface_destroy(image);
          break;
        case COMMAND_TEXT:
          cairo_save (cr);
          cairo_set_font_size (cr, currentCmd->arg1);
          cairo_move_to (cr, currentCmd->points[0].x,
                         currentCmd->points[0].y);
//        cairo_rotate (cr, M_PI / 2);
          cairo_show_text (cr, currentCmd->text);
          cairo_restore (cr);
          break;
        default:
          break;
        }
    }
  cairo_scale(cr, m_render_handle[handle].win.width / (double)640, m_render_handle[handle].win.height / (double)480);  
}

rendererCairo::rendererCairo (int width, int height)
{
  m_scale = 1.0;
  m_forground_colour =
  {
  255, 255, 255};
  m_background_colour =
  {
  0, 0, 0};
  m_height = height;
  m_width = width;
}

rendererCairo::~rendererCairo ()
{
  cairo_destroy (m_render_handle[0].cr);

  XCloseDisplay (m_render_handle[0].win.dpy);
}

int
rendererCairo::init (int width, int height)
{
  m_render_handle[0].size.width = width;
  m_render_handle[0].size.height = height;

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

  Visual *visual =
    DefaultVisual (m_render_handle[0].win.dpy,
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
rendererCairo::setLineThickness (int handle, int thickness)
{
  m_draw_commands[m_draw_tail].command = COMMAND_PEN_THICKNESS;
  m_draw_commands[m_draw_tail].arg1 = thickness;
  m_draw_tail++;
}

int
rendererCairo::drawLine (int handle, int x1, int y1, int x2, int y2)
{
  y1 = m_render_handle[handle].size.height - y1;
  y2 = m_render_handle[handle].size.height - y2;
  m_draw_commands[m_draw_tail].command = COMMAND_PEN_LINE;
  m_draw_commands[m_draw_tail].points[0].x = x1 * m_scale;
  m_draw_commands[m_draw_tail].points[0].y = y1 * m_scale;
  m_draw_commands[m_draw_tail].points[1].x = x2 * m_scale;
  m_draw_commands[m_draw_tail].points[1].y = y2 * m_scale;
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

  m_draw_commands[m_draw_tail].command = COMMAND_IMAGE_PNG;
  m_draw_commands[m_draw_tail].points[0].x = x;
  m_draw_commands[m_draw_tail].points[0].y = y;
  m_draw_commands[m_draw_tail].arg1 = m_image_tail;

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











static void
triangle (cairo_t * cr)
{
  cairo_move_to (cr, SIZE, 0);
  cairo_rel_line_to (cr, SIZE, 2 * SIZE);
  cairo_rel_line_to (cr, -2 * SIZE, 0);
  cairo_close_path (cr);
}

static void
square (cairo_t * cr)
{
  cairo_move_to (cr, 0, 0);
  cairo_rel_line_to (cr, 2 * SIZE, 0);
  cairo_rel_line_to (cr, 0, 2 * SIZE);
  cairo_rel_line_to (cr, -2 * SIZE, 0);
  cairo_close_path (cr);
}

static void
bowtie (cairo_t * cr)
{
  cairo_move_to (cr, 0, 0);
  cairo_rel_line_to (cr, 2 * SIZE, 2 * SIZE);
  cairo_rel_line_to (cr, -2 * SIZE, 0);
  cairo_rel_line_to (cr, 2 * SIZE, -2 * SIZE);
  cairo_close_path (cr);
}

static void
inf (cairo_t * cr)
{
  cairo_move_to (cr, 0, SIZE);
  cairo_rel_curve_to (cr, 0, SIZE, SIZE, SIZE, 2 * SIZE, 0);
  cairo_rel_curve_to (cr, SIZE, -SIZE, 2 * SIZE, -SIZE, 2 * SIZE, 0);
  cairo_rel_curve_to (cr, 0, SIZE, -SIZE, SIZE, -2 * SIZE, 0);
  cairo_rel_curve_to (cr, -SIZE, -SIZE, -2 * SIZE, -SIZE, -2 * SIZE, 0);
  cairo_close_path (cr);
}

static void
draw_shapes (cairo_t * cr, int x, int y, int fill)
{
  cairo_save (cr);

  cairo_new_path (cr);
  cairo_translate (cr, x + SIZE, y + SIZE);
  bowtie (cr);
  if (fill)
    cairo_fill (cr);
  else
    cairo_stroke (cr);

  cairo_new_path (cr);
  cairo_translate (cr, 4 * SIZE, 0);
  square (cr);
  if (fill)
    cairo_fill (cr);
  else
    cairo_stroke (cr);

  cairo_new_path (cr);
  cairo_translate (cr, 4 * SIZE, 0);
  triangle (cr);
  if (fill)
    cairo_fill (cr);
  else
    cairo_stroke (cr);

  cairo_new_path (cr);
  cairo_translate (cr, 4 * SIZE, 0);
  inf (cr);
  if (fill)
    cairo_fill (cr);
  else
    cairo_stroke (cr);

  cairo_restore (cr);
}

static void
fill_shapes (cairo_t * cr, int x, int y)
{
  draw_shapes (cr, x, y, 1);
}

static void
stroke_shapes (cairo_t * cr, int x, int y)
{
  draw_shapes (cr, x, y, 0);
}

static void
win_draw (win_t * win)
{
  cout << "Line: " << __LINE__ << "\n";
#define NUM_DASH 2
  static double dash[NUM_DASH] = { SIZE / 4.0, SIZE / 4.0 };
  cairo_surface_t *surface;
  cairo_t *cr;
  Visual *visual = DefaultVisual (win->dpy, DefaultScreen (win->dpy));

  XClearWindow (win->dpy, win->win);

  surface = cairo_xlib_surface_create (win->dpy, win->win, visual,
                                       win->width, win->height);
  cr = cairo_create (surface);

  cairo_set_source_rgb (cr, 1, 1, 1);

  cairo_save (cr);
  cairo_set_font_size (cr, 20);
  cairo_move_to (cr, 10, 10);
  cairo_rotate (cr, M_PI / 2);
  cairo_show_text (cr, "Hello World.");
  cairo_restore (cr);


  /* This is handy for examining problems more closely */
  /*    cairo_scale(cr, 4, 4);  */

  cairo_set_line_width (cr, SIZE / 8);

  cairo_set_tolerance (cr, .1);

  cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);
  cairo_set_dash (cr, dash, NUM_DASH, 0);
  stroke_shapes (cr, 0, 0);

  cairo_translate (cr, 0, 4 * SIZE);

  cairo_set_dash (cr, NULL, 0, 0);
  stroke_shapes (cr, 0, 0);

  cairo_translate (cr, 0, 4 * SIZE);

  cairo_set_line_join (cr, CAIRO_LINE_JOIN_BEVEL);
  stroke_shapes (cr, 0, 0);

  cairo_translate (cr, 0, 4 * SIZE);

  cairo_set_line_join (cr, CAIRO_LINE_JOIN_MITER);
  stroke_shapes (cr, 0, 0);

  cairo_translate (cr, 0, 4 * SIZE);

  fill_shapes (cr, 0, 0);

  cairo_translate (cr, 0, 4 * SIZE);

  cairo_set_line_join (cr, CAIRO_LINE_JOIN_BEVEL);
  fill_shapes (cr, 0, 0);
  cairo_set_source_rgb (cr, 1, 0, 0);
  stroke_shapes (cr, 0, 0);
/*
    draw_broken_shapes(cr);
*/
  if (cairo_status (cr))
    {
      printf ("Cairo is unhappy: %s\n",
              cairo_status_to_string (cairo_status (cr)));
      exit (0);
    }

  cairo_destroy (cr);
  cairo_surface_destroy (surface);
}
