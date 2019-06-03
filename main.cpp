#include <iostream>
#include <unistd.h>
#include "debug.h"
#include "pngget.h"
#include "screenGva.h"

using namespace std;

#define LABEL_NULL "Unused!"

// These labels should not change
#define COMMON_KEYS { 0b00000100, 0b00000000, "UP", "DOWN", "Alarms", "Ack", "", "", "Labels", "" }

#define TEST_FUNCTION_KEYS_TOP { 0b00010000, 0b01000000 }
#define TEST_FUNCTION_KEYS_LEFT { 0b000001, 0b011100 , { "F1", "F2", "F3", "F4", "F5", "F6" } }
#define TEST_FUNCTION_KEYS_RIGHT {0b100000, 0b001111, { "F7", "F8", "F9", "F10", "F11", "F12" } }

#define SA_FUNCTION_KEYS_TOP { 0b00001000, 0b00000010 }
#define SA_FUNCTION_KEYS_LEFT { 0b110000, 0b010000 , { "PriSight", LABEL_NULL, "Quad", "Front left", "Left", "Rear Left" } }
#define SA_FUNCTION_KEYS_RIGHT {0b000100, 0b111000, { LABEL_NULL, LABEL_NULL, LABEL_NULL, "Front right", "Right", "Rear right" } }

int
main (int argc, char *argv[])
{
  XEvent event;
  int done = 0;
  resolution_type view = { 640, 480, 24 };
  int hndl;
//  screenType screen_test = { "Test Screen", TEST_FUNCTION_KEYS_TOP, TEST_FUNCTION_KEYS_LEFT, TEST_FUNCTION_KEYS_RIGHT, COMMON_KEYS };
  screenType screen_sa =
    { "Situational Awareness", SA_FUNCTION_KEYS_TOP, SA_FUNCTION_KEYS_LEFT,
SA_FUNCTION_KEYS_RIGHT, COMMON_KEYS };
  screenGva *render = new screenGva (screen_sa, view.width, view.height);
//  screenGva *render2 = new screenGva(screen_test, 800, 800);


  Display *d = render->getDisplay ();
  Window *w = render->getWindow ();

  /* select kind of events we are interested in */
  XSelectInput (d, *w, KeyPressMask | KeyReleaseMask | StructureNotifyMask);

  //win->quit_code = XKeysymToKeycode (win->dpy, XStringToKeysym ("Q"));

  render->draw (0);

  while (!done)
    {
      XNextEvent (d, &event);

      switch (event.type)
        {
        case KeyPress:
          {
            printf ("KeyPress: %x\n", event.xkey.keycode);

            /* exit on ESC key press */
            if (event.xkey.keycode == 0x09)
              {
                done = 1;
                break;
              }
          }
          break;
        case KeyRelease:
          {
            printf ("KeyRelease: %x\n", event.xkey.keycode);
          }
          render->draw (0);
          break;
        case ConfigureNotify:
          {
            XConfigureEvent *cev = &event.xconfigure;

            if (cev->width != render->getWidth () ||
                cev->height != render->getHeight ())
              {
                printf ("WindowResize: %d x %d\n", cev->width, cev->height);
                render->setWidth (cev->width);
                render->setHeight (cev->height);
                render->draw (0);
              }
          }
          break;
        case Expose:
          {
              render->draw (0);
          }
          break;
        }


    }

  /*
   * Clean up render/s
   */
  delete (render);
}
