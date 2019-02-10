#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "rendererSdl.h"
#include "debug.h"

using namespace std;

void *
renderer_thread (void *arg)
{
  SDL_Renderer *renderer_handle = (SDL_Renderer *) arg;
  bool done = SDL_FALSE;
  while (!done)
    {
      SDL_Event event;

      SDL_SetRenderDrawColor (renderer_handle, 0, 0, 0, SDL_ALPHA_OPAQUE);
      SDL_RenderClear (renderer_handle);

      while (SDL_PollEvent (&event))
        {
          if (event.type == SDL_QUIT)
            {
              done = SDL_TRUE;
            }
        }
    }
}

rendererSdl::rendererSdl ()
{
  if (SDL_Init (SDL_INIT_VIDEO) == 0)
    {

    }
}

int
rendererSdl::getFreeHandle ()
{
  int counter = 0;
  for (counter; counter < MAX_HANDLES; counter++);
  {
    if (!m_render_handle[counter].inUse)
      {
        m_render_handle[counter].inUse = true;
        m_render_handle[counter].handle = counter;
        return counter;
      }
  }
}

rendererSdl::~rendererSdl ()
{
  int h;
  h = getFreeHandle ();
  if (m_render_handle[h].renderer)
    {
      SDL_DestroyRenderer (m_render_handle[h].renderer);
    }
  if (m_render_handle[h].window)
    {
      SDL_DestroyWindow (m_render_handle[h].window);
    }

  pthread_create (&m_event_thread, NULL, renderer_thread,
                  (void *) m_render_handle[h].renderer);
  pthread_join (m_event_thread, NULL);

  SDL_Quit ();
}

int
rendererSdl::init (int width, int height)
{
  m_render_handle[0].size.height = height;
  m_render_handle[0].size.width = width;
  if (SDL_CreateWindowAndRenderer
      (width, height, 0, &m_render_handle[0].window,
       &m_render_handle[0].renderer) == 0)
    {
      SDL_bool done = SDL_FALSE;
    }
  return 0;
}


void
rendererSdl::setPixel (int handle, int x, int y)
{
  SDL_Renderer *surface = m_render_handle[handle].renderer;
  SDL_RenderDrawPoint (surface, x, y);
}

int
rendererSdl::textureRGB (int handle, int x, int y, void *buff)
{
  char *buffer_ptr = NULL;
  int pitch = 0;
  SDL_Texture *buffer = SDL_CreateTexture (m_render_handle[handle].renderer,
                                           SDL_PIXELFORMAT_RGBA8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           x,
                                           y);

  if (buffer)                   // Buffer OK
    {
      if (SDL_LockTexture (buffer, NULL,        // NULL means the *whole texture* here.
                           (void **) &buffer_ptr, &pitch))
        {
          cout << "Error failed SDL_LockTexture()\n";
        }

      cout << "Pitch : " << pitch << "\n";

      int c = 0;
      char *tmp = (char *) buff;
      for (int i = 0; i < pitch * y; i += 4)
        {
          //               Alpha Blue  Green Red
//              char test[4] = { 0xff, 0xff, 0xff, 0xff };
//              memcpy(&buffer_ptr[i], test);
          buffer_ptr[i] = 0xff;
          buffer_ptr[i + 1] = tmp[c + 2];
          buffer_ptr[i + 2] = tmp[c + 1];
          buffer_ptr[i + 3] = tmp[c];
          c += 3;
        }
      DumpHex (buffer_ptr, 200);

      SDL_UnlockTexture (buffer);

      if (SDL_SetTextureBlendMode (buffer, SDL_BLENDMODE_BLEND))
        {
          cout << "Error could set blendmode SDL_SetTextureBlendMode()\n";
        }

      if (SDL_RenderCopy
          (m_render_handle[handle].renderer, buffer, NULL, NULL))
        {
          cout << "Error could not render texture SDL_RenderCopy()\n";
        }
    }
  else
    {
      return -1;
    }
  return 0;
}

int
rendererSdl::drawLine (int handle, int x1, int y1, int x2, int y2)
{
  y1 = m_render_handle[0].size.height - y1;
  y2 = m_render_handle[0].size.height - y2;
  cout << "line : " << x1 << "," << y1 << "," << x2 << "," << y2 << "\n";
  SDL_RenderDrawLine (m_render_handle[handle].renderer, x1, y1, x2, y2);
  return 0;
}

int
rendererSdl::drawColor (int handle, int r, int g, int b)
{
  SDL_SetRenderDrawColor (m_render_handle[handle].renderer, r, g, b,
                          SDL_ALPHA_OPAQUE);
  return 0;
}

void
rendererSdl::drawCircle (int handle, int x, int y, int radius)
{
  y = m_render_handle[0].size.height - y;

  SDL_Renderer *surface = m_render_handle[handle].renderer;
  // if the first pixel in the screen is represented by (0,0) (which is in sdl)
  // remember that the beginning of the circle is not in the middle of the pixel
  // but to the left-top from it:

  double error = (double) -radius;
  double xx = (double) radius - 0.5;
  double yy = (double) 0.5;
  double cx = x - 0.5;
  double cy = y - 0.5;

  while (xx >= yy)
    {
      setPixel (handle, (int) (cx + xx), (int) (cy + yy));
      setPixel (handle, (int) (cx + yy), (int) (cy + xx));

      if (x != 0)
        {
          setPixel (handle, (int) (cx - xx), (int) (cy + yy));
          setPixel (handle, (int) (cx + yy), (int) (cy - xx));
        }

      if (y != 0)
        {
          setPixel (handle, (int) (cx + xx), (int) (cy - yy));
          setPixel (handle, (int) (cx - yy), (int) (cy + xx));
        }

      if (xx != 0 && yy != 0)
        {
          setPixel (handle, (int) (cx - xx), (int) (cy - yy));
          setPixel (handle, (int) (cx - yy), (int) (cy - xx));
        }

      error += yy;
      ++yy;
      error += yy;

      if (error >= 0)
        {
          --xx;
          error -= xx;
          error -= xx;
        }
    }
}

void
rendererSdl::fillCircle (int handle, int x, int y, int radius)
{
  SDL_Renderer *surface = m_render_handle[handle].renderer;
  // Note that there is more to altering the bitrate of this 
  // method than just changing this value.  See how pixels are
  // altered at the following web page for tips:
  //   http://www.libsdl.org/intro.en/usingvideo.html
  static const int BPP = 4;

  y = m_render_handle[0].size.height - y;

  //double ra = (double)radius;

  for (double dy = 1; dy <= radius; dy += 1.0)
    {
      // This loop is unrolled a bit, only iterating through half of the
      // height of the circle.  The result is used to draw a scan line and
      // its mirror image below it.

      // The following formula has been simplified from our original.  We
      // are using half of the width of the circle because we are provided
      // with a center and we need left/right coordinates.

      double dx = floor (sqrt ((2.0 * radius * dy) - (dy * dy)));
      SDL_RenderDrawLine (surface, x - dx, y + dy - radius, x + dx,
                          y + dy - radius);
      SDL_RenderDrawLine (surface, x - dx, y - dy + radius, x + dx,
                          y - dy + radius);
    }
}

void
rendererSdl::scale (int handle, float x)
{
  SDL_RenderSetScale (m_render_handle[handle].renderer, x, x);
}

void
rendererSdl::present (int handle)
{
  SDL_RenderPresent (m_render_handle[handle].renderer);
}
