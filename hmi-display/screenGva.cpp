#include <string.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "screenGva.h"
#include "logGva.h"

screenGva::screenGva (screenType * screen, int width, int height):
rendererGva (width, height)
{
  m_screen = screen;
  m_width = width;
  m_height = height;
  m_hndl = init (m_width, m_height);
  update (m_screen);
  logGva::log ("GVA screen initalised.", LOG_INFO);
}

screenGva::~screenGva ()
{
  m_args->active = false;
  pthread_join (m_clock_thread, 0);
  free (m_args);
  logGva::log ("GVA screen finalized.", LOG_INFO);
  logGva::finish ();
}

void *
clockUpdate (void *arg)
{
  args *a = (args *) arg;
  time_t t;
  struct tm *tm;

  while (a->active)
    {
      t = time (NULL);
      tm = localtime (&t);
      sprintf (a->clockString, "%02d/%02d/%02d %02d:%02d:%02d", tm->tm_mday,
               tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min,
               tm->tm_sec);
      a->screen->refresh ();
      usleep (1000000);
    }
}

void
screenGva::startClock (statusBarType * barData)
{
  pthread_t m_clock_thread;
  m_args = (args *) malloc (sizeof (args));
  m_args->active = true;
  m_args->clockString = barData->labels[0];
  m_args->screen = this;

  /* Launch clock thread */
  if (pthread_create (&m_clock_thread, NULL, clockUpdate, (void *) m_args))
    {
      logGva::log ("Error creating thread", LOG_ERROR);
      return;
    }
  logGva::log ("Internal clock thread started", LOG_INFO);
}

int
screenGva::update (screenType * screen)
{
  char *texture = 0;

  /* Reset the drawing context, must be reset before redrawing the screen */
  reset ();

  // Set background green
  setColourForground (m_hndl, GREEN);
  setColourBackground (m_hndl, GREEN);
  drawRectangle (m_hndl, 0, 0, m_width, m_height, true);

#if 1
  if (m_screen->canvas.visible)
    {
      if (m_screen->canvas.buffer) {
        textureRGB (m_hndl, 0, 0, m_screen->canvas.buffer);
      } else {
        textureRGB (m_hndl, 0, 0, texture, m_screen->canvas.filename);
      }
    }
#endif
  if (m_screen->functionLeft.visible)
    {
      drawFunctionKeys (m_hndl, 1, m_screen->functionLeft.active,
                        m_screen->functionLeft.hidden,
                        m_screen->functionLeft.toggleActive,
                        m_screen->functionLeft.toggleOn,
                        m_screen->functionLeft.labels);
    }
  if (m_screen->functionRight.visible)
    {
      drawFunctionKeys (m_hndl, m_width - 100 - 1,
                        m_screen->functionRight.active,
                        m_screen->functionRight.hidden,
                        m_screen->functionRight.toggleActive,
                        m_screen->functionRight.toggleOn,
                        m_screen->functionRight.labels);
    }
#if 1
  drawSaKeys (m_hndl, m_height - 11, m_screen->functionTop->active,
              m_screen->functionTop->hidden);
#endif
  if (m_screen->keyboard.visible) {
    drawKeyboard (m_hndl, m_screen->keyboard.mode);
  }

  drawMode (m_hndl);

  if (m_screen->statusBar->visible)
    {
      drawTable (m_hndl, m_screen->statusBar->labels);
    }

  if (m_screen->compass.visible)
    {
      drawPPI (m_hndl, 165, 380, 0);
    }

  if (m_screen->control.visible)
    {
      drawControlKeys (m_hndl, 0, m_screen->control.active,
                       m_screen->control.hidden);
    }

  /*
   *  Refersh display
   */
  draw (m_hndl);
  m_last_screen = *screen;
}

int
screenGva::refresh ()
{
  XClientMessageEvent dummyEvent;

  /*
   * Send a dumy event to force screen update 
   */
  memset (&dummyEvent, 0, sizeof (XClientMessageEvent));
  dummyEvent.type = Expose;
  dummyEvent.window = *getWindow ();
  dummyEvent.format = 32;

  XLockDisplay (getDisplay ());
  XSendEvent (getDisplay (), *getWindow (), False, StructureNotifyMask,
              (XEvent *) & dummyEvent);
  XFlush (getDisplay ());
  XUnlockDisplay (getDisplay ());
}
