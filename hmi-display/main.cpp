/**
 * @file main.c
 * @author ross@rossnewman.com
 * @date 04 June 2019
 * @brief The example HMI application.
 */
#include <iostream>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include "gva.h"
#include "eventsGva.h"
#include "debug.h"
#include "screenGva.h"
#include "gvaVideoRtpYuv.h"

using namespace std;
using namespace gva;

#define LABEL_NULL "Unused!"

// These labels should not change
#define COMMON_KEYS { true, 0b0001000, 0b00000000, "Up", "Alarms", "Threats", "Ack", "↑", "↓", "Labels", "Enter" }
#define COMMON_FUNCTION_KEYS_TOP { true, 0b00100000, 0b00000100 }
#define COMMON_STATUS_BAR { true,  "12:30:00, 03/06/2019", "Lat:51.500655 Lon:-0.124240    [1,3]", "W:0", "A:0", "C:0" }
#define COMPASS { true, 0, 0 }
#define CANVAS { true, 0, 0 }
/*                              Visible    Active    Hidden */
#define TEST_FUNCTION_KEYS_LEFT  { true, 0b000001, 0b011100, { "F1", "F2", "F3", "F4", "F5", "F6" } }
#define TEST_FUNCTION_KEYS_RIGHT { true, 0b100000, 0b001111, { "F7", "F8", "F9", "F10", "F11", "F12" } }

/*                              Visible    Active    Hidden    Toggle  ToggleOn */
#define SA_FUNCTION_KEYS_LEFT    { true, 0b100000, 0b110111, 0b000000, 0b000000, { "Wpn Sight", "Quad", LABEL_NULL, "Front left", "Left", "Rear Left" } }
#define SA_FUNCTION_KEYS_RIGHT   { true, 0b000000, 0b000111, 0b000000, 0b000000, { LABEL_NULL, LABEL_NULL, LABEL_NULL, "Front right", "Right", "Rear right" } }

#define WPN_FUNCTION_KEYS_LEFT   { true, 0b000000, 0b110000, 0b000000, 0b000000, { "Wpn Sight", "IR Sight", LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL } }
#define WPN_FUNCTION_KEYS_RIGHT  { true, 0b000000, 0b000000, 0b000000, 0b000000, { LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL } }

#define DEF_FUNCTION_KEYS_LEFT   { true, 0b000000, 0b110000, 0b110000, 0b000000, { "ECM", "Spike", LABEL_NULL, LABEL_NULL, LABEL_NULL } }
#define DEF_FUNCTION_KEYS_RIGHT  { true, 0b000000, 0b000000, 0b000000, 0b000000, { LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, } }

#define SYS_FUNCTION_KEYS_LEFT   { true, 0b100000, 0b111101, 0b000000, 0b000000, { "Overview", "Automotive", "System", "HUMS", LABEL_NULL, "BIT" } }
#define SYS_FUNCTION_KEYS_RIGHT  { true, 0b000000, 0b111111, 0b000000, 0b000000, { "Commander.Health", "Gunner.Health", "Driver.Health", "Roll.Allocation", "Mission", "Blackout", } }

#define DRV_FUNCTION_KEYS_LEFT   { true, 0b110000, 0b100000, 0b000000, 0b000000, { "Assist", "PPI Mode", LABEL_NULL,LABEL_NULL, LABEL_NULL } }
#define DRV_FUNCTION_KEYS_RIGHT  { true, 0b000000, 0b000000, 0b000000, 0b000000, { LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, } }

#define STR_FUNCTION_KEYS_LEFT   { true, 0b000000, 0b000000, 0b000000, 0b000000, { LABEL_NULL, LABEL_NULL, LABEL_NULL,LABEL_NULL, LABEL_NULL } }
#define STR_FUNCTION_KEYS_RIGHT  { true, 0b000000, 0b000000, 0b000000, 0b000000, { LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, } }

#define COM_FUNCTION_KEYS_LEFT   { true, 0b100000, 0b110000, 0b000000, 0b000000, { "Comms", "RF State", LABEL_NULL, LABEL_NULL, LABEL_NULL } }
#define COM_FUNCTION_KEYS_RIGHT  { true, 0b000000, 0b000000, 0b000000, 0b000000, { LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, } }

#define BMS_FUNCTION_KEYS_LEFT   { true, 0b100000, 0b100000, 0b000000, 0b000000, { "Mission", LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL } }
#define BMS_FUNCTION_KEYS_RIGHT  { true, 0b000000, 0b000000, 0b000000, 0b000000, { LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, } }

#define BIT(b,x) (x & 0x1 << b)
#define SET_CANVAS_PNG(file) strcpy (screen->canvas.filename, file); screen->canvas.buffer = 0;

int
main (int argc, char *argv[])
{
  eventGvaType event;
  int done = 0;
  char* rtpBuffer;
  /* 4:3 aspect ratio */
  resolution_type view = { 640, 480, 24 };
  /* 16:9 aspect ration */
//  resolution_type view = { 1080, 720, 24 };
  int hndl;
  /* These are comon to all screens */
  statusBarType status = COMMON_STATUS_BAR;
  functionSelectType top = COMMON_FUNCTION_KEYS_TOP;
  canvasType canvas = { true, "test2.png", 0 };
  keyboardType keyboard = { false, KEYBOARD_UPPER };
  screenType screen_sa =
    { "Situational Awareness", "/dev/ttyUSB0", SA, canvas, &top, &status, 
      SYS_FUNCTION_KEYS_LEFT, SYS_FUNCTION_KEYS_RIGHT, COMMON_KEYS, COMPASS, keyboard };
  screenType *screen = &screen_sa;
  screenGva *render = new screenGva (screen, view.width, view.height);

  cout << "hmi_display (v" << MAJOR << "." << MINOR << "." << PATCH <<
    ") author ross@rossnewman.com...\n";

  /*
   * Initalise the display events
   */
  eventsGva io(render->getDisplay (), render->getWindow ());
  
  /*
   * Setup video sources (default size will be 640 x 480)
   */
  gvaVideoRtpYuv *rtpStream1 = new gvaVideoRtpYuv("127.0.0.1", 5004);
  cout << "Resolution " << rtpStream1->getHeight() << "x" << rtpStream1->getWidth() << "\n";
  rtpBuffer = (char*)malloc(rtpStream1->getHeight() * rtpStream1->getWidth() * 4); 

  /*
   * Draw the inital screen and start the Real Time Clock
   */
  render->update (screen);
  render->startClock (&status);

  while (!done) {
    io.nextGvaEvent(&event);

    /*
     * Get the live video frame if Driver (DRV)
     */  
    if (screen->currentFunction == DRV) {
      rtpStream1->gvaRecieveFrame(rtpBuffer, RGBA_COLOUR);
      io.flush();
    }
      
    switch (event.type) {
      case KEY_EVENT :
        {
          switch (event.key)
            {
            case KEY_ESC:
              /* exit on ESC key press */
              done = 1;
              break;
            case KEY_SA:
              /* 1 maps to F1 */
              if (!BIT (7, screen->functionTop->hidden))
                {
                  functionKeysType left = SA_FUNCTION_KEYS_LEFT;
                  functionKeysType right = SA_FUNCTION_KEYS_RIGHT;
                  screen->currentFunction = SA;
                  
                  screen->compass.visible = true;
                  screen->canvas.visible = true;
                  SET_CANVAS_PNG("test2.png");
                  screen->functionTop->active = 0x1 << 7;
                  screen->functionLeft = left;
                  screen->functionRight = right;
                }
              break;
            case KEY_WPN:
              /* 2 maps to F2 */
              if (!BIT (6, screen->functionTop->hidden))
                {
                  functionKeysType left = WPN_FUNCTION_KEYS_LEFT;
                  functionKeysType right = WPN_FUNCTION_KEYS_RIGHT;
                  screen->currentFunction = DEF;

                  screen->compass.visible = true;
                  screen->canvas.visible = true;
                  SET_CANVAS_PNG("test2.png");
                  screen->functionTop->active = 0x1 << 6;
                  screen->functionLeft = left;
                  screen->functionRight = right;
                }
              break;
            case KEY_DEF:
              /* 3 maps to F3 */
              if (!BIT (5, screen->functionTop->hidden))
                {
                  functionKeysType left = DEF_FUNCTION_KEYS_LEFT;
                  functionKeysType right = DEF_FUNCTION_KEYS_RIGHT;
                  screen->currentFunction = SYS;

                  screen->compass.visible = false;
                  screen->canvas.visible = false;
                  screen->functionTop->active = 0x1 << 5;
                  screen->functionLeft = left;
                  screen->functionRight = right;
                }
              break;
            case KEY_SYS:
              /* 4 maps to F4 */
              if (!BIT (4, screen->functionTop->hidden))
                {
                  functionKeysType left = SYS_FUNCTION_KEYS_LEFT;
                  functionKeysType right = SYS_FUNCTION_KEYS_RIGHT;
                  screen->currentFunction = DRV;

                  screen->compass.visible = true;
                  screen->canvas.visible = true;
                  if (rtpBuffer) {
                    screen->canvas.buffer = rtpBuffer;
                  } else {
                    SET_CANVAS_PNG("test2.png");
                  }
                  screen->functionTop->active = 0x1 << 4;
                  screen->functionLeft = left;
                  screen->functionRight = right;
                }
              break;
            case KEY_DRV:
              /* 5 maps to F5 */
              if (!BIT (3, screen->functionTop->hidden))
                {
                  functionKeysType left = DRV_FUNCTION_KEYS_LEFT;
                  functionKeysType right = DRV_FUNCTION_KEYS_RIGHT;
                  screen->currentFunction = STR;

                  screen->compass.visible = false;
                  screen->canvas.visible = false;
                  screen->functionTop->active = 0x1 << 3;
                  screen->functionLeft = left;
                  screen->functionRight = right;
                }
              break;
            case KEY_STR:
              /* 6 maps to F6 */
              if (!BIT (2, screen->functionTop->hidden))
                {
                  functionKeysType left = STR_FUNCTION_KEYS_LEFT;
                  functionKeysType right = STR_FUNCTION_KEYS_RIGHT;
                  screen->currentFunction = COM;

                  screen->compass.visible = false;
                  screen->canvas.visible = false;
                  screen->functionTop->active = 0x1 << 2;
                  screen->functionLeft = left;
                  screen->functionRight = right;
                }
              break;
            case KEY_COM:
              /* 7 maps to F7 */
              if (!BIT (1, screen->functionTop->hidden))
                {
                  functionKeysType left = COM_FUNCTION_KEYS_LEFT;
                  functionKeysType right = COM_FUNCTION_KEYS_RIGHT;
                  screen->currentFunction = BMS;

                  screen->compass.visible = false;
                  screen->compass.visible = false;
                  screen->canvas.visible = false;
                  screen->functionTop->active = 0x1 << 1;
                  screen->functionLeft = left;
                  screen->functionRight = right;
                }
              break;
            case KEY_BMS:
              /* 8 maps to F8 */
              if (!BIT (0, screen->functionTop->hidden))
                {
                  functionKeysType left = BMS_FUNCTION_KEYS_LEFT;
                  functionKeysType right = BMS_FUNCTION_KEYS_RIGHT;

                  screen->compass.visible = false;
                  screen->canvas.visible = true;
                  SET_CANVAS_PNG("map.png");
                  screen->functionTop->active = 0x1 << 0;
                  screen->functionLeft = left;
                  screen->functionRight = right;
                }
              break;
            case KEY_F19:
              /* Toggle labels */
              screen->functionLeft.visible =
                screen->functionLeft.visible ? false : true;
              screen->functionRight.visible =
                screen->functionRight.visible ? false : true;
              screen->control.visible =
                screen->control.visible ? false : true;
              screen->statusBar->visible =
                screen->statusBar->visible ? false : true;
              screen->compass.visible =
                screen->compass.visible ? false : true;
              break;
            case KEY_FULLSCREEN:
              /* f toggle fullscreen TODO: Does not work */
#if 0
              {
                Atom wm_state = XInternAtom (d, "_NET_WM_STATE", true);
                Atom wm_fullscreen =
                  XInternAtom (d, "_NET_WM_STATE_FULLSCREEN", true);

                XChangeProperty (d, *w, wm_state, XA_ATOM, 32,
                                 PropModeReplace,
                                 (unsigned char *) &wm_fullscreen, 1);
              }
#endif
              break;
            case KEY_KEYBOARD:
              /* k toggle keyboard */
              {
                screen->keyboard.visible =
                screen->keyboard.visible ? false : true;
              }
              break;
            case KEY_F17:
              /* Up Arrow keyboard */
              {
                screen->keyboard.mode = (screen->keyboard.mode == KEYBOARD_UPPER) ?  KEYBOARD_LOWER :  KEYBOARD_UPPER; 
              }
              break;
            case KEY_F18:
              /* Down Arrow keyboard */
              {
                screen->keyboard.mode = (screen->keyboard.mode == KEYBOARD_NUMBERS) ?  KEYBOARD_UPPER :  KEYBOARD_NUMBERS; 
              }
              break;
            default:
              printf ("[GVA] KeyPress not defined 0x%x\n", event.key);
              break;
            }
        }
        render->update (screen);
        break;
      case RESIZE_EVENT:
        {
              printf ("[GVA] WindowResize: %d x %d\n", event.resize.width, event.resize.height);
          if (event.resize.width != render->getWidth () ||
              event.resize.height != render->getHeight ())
            {
              printf ("[GVA] WindowResize: %d x %d\n", event.resize.width, event.resize.height);
              render->setWidth (event.resize.width);
              render->setHeight (event.resize.height);
              render->update (screen);
            }
        }
        break;
      case REDRAW_EVENT:
        {
          render->update (screen);
        }
        break;
    }
  }

  /*
   * Clean up code goes here
   */
  cout << "Exiting hmi_display...\n";
}
