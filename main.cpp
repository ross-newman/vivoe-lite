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
  functionSelectType top = TEST_FUNCTION_KEYS_TOP;
  screenType screen_test = { "Test Screen", top, TEST_FUNCTION_KEYS_LEFT, TEST_FUNCTION_KEYS_RIGHT, COMMON_KEYS };
  screenType screen_sa =
    { "Situational Awareness", SA_FUNCTION_KEYS_TOP, SA_FUNCTION_KEYS_LEFT, SA_FUNCTION_KEYS_RIGHT, COMMON_KEYS }; 
  screenType *screen = &screen_test;

  screenGva *render = new screenGva (screen, view.width, view.height);

  Display *d = render->getDisplay ();
  Window *w = render->getWindow ();

  /* select kind of events we are interested in */
  XSelectInput (d, *w, KeyPressMask | KeyReleaseMask | StructureNotifyMask);

  render->update(screen);
  
  while (!done)
    {
      XNextEvent (d, &event);

      switch (event.type)
        {
        case KeyPress:
          {
            printf ("KeyPress: 0x%x ('%c')\n", event.xkey.keycode);

            switch (event.xkey.keycode) {
				/* exit on ESC key press */
				case 0x09 : 
                    done = 1;
                    break;
				/* 1 maps to F1 */
				case 0xa : 
                    screen->functionTop.active ^= 0x1 << 0;
                    break;
				/* 2 maps to F2 */
				case 0xb : 
                    screen->functionTop.active ^= 0x1 << 1;
                    break;
				/* 3 maps to F3 */
				case 0xc : 
                    screen->functionTop.active ^= 0x1 << 2;
                    break;
				/* 4 maps to F4 */
				case 0xd : 
                    screen->functionTop.active ^= 0x1 << 3;
                    break;
				/* 5 maps to F5 */
				case 0xe : 
                    screen->functionTop.active ^= 0x1 << 4;
                    break;
				/* 6 maps to F6 */
				case 0xf : 
                    screen->functionTop.active ^= 0x1 << 5;
                    break;
				/* 7 maps to F7 */
				case 0x10 : 
                    screen->functionTop.active ^= 0x1 << 6;
                    break;
				/* 8 maps to F8 */
				case 0x11 : 
                    screen->functionTop.active ^= 0x1 << 7;
                    break;
              }
          }
          break;
        case KeyRelease:
          {
            printf ("KeyRelease: 0x%x ('%c')\n", event.xkey.keycode);
          }
          render->update(screen);
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
                render->update(screen);
              }
          }
          break;
        case Expose:
          {
              render->update(screen);
          }
          break;
        }
    }

  /*
   * Clean up render/s
   */
  delete (render);
}
