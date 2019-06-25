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
#include "gva.hpp"
#include "eventsGva.hpp"
#include "hmiGva.hpp"

using namespace std;
using namespace gva;

int
main (int argc, char *argv[])
{
  eventGvaType event;
  int done = 0;
  char* rtpBuffer;
  int hndl;
  bool update;

  cout << "hmi_display (v" << MAJOR << "." << MINOR << "." << PATCH <<
    ") author ross@rossnewman.com...\n";

  // instantiate events
  KeyPowerOn on;

  hmi::start();
  
  hmi::dispatch(on);
  
  /*
   * Initalise the display events
   */
  eventsGva io(hmi::getRendrer()->getDisplay(), hmi::getRendrer()->getWindow(), hmi::getRendrer()->getTouch());
  
  /*
   * Setup video sources (default size will be 640 x 480)
   */
  gvaVideoRtpYuv *rtpStream1 = new gvaVideoRtpYuv("127.0.0.1", 5004);
  cout << "Resolution " << rtpStream1->getHeight() << "x" << rtpStream1->getWidth() << "\n";
  rtpBuffer = (char*)malloc(rtpStream1->getHeight() * rtpStream1->getWidth() * 4); 

  while (!done) {
    update = true;
    io.nextGvaEvent(&event);

    /*
     * Get the live video frame if Driver (DRV)
     */  
    if (hmi::getScreen()->currentFunction == DRV) {
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
              {
                KeySA sa;
                hmi::dispatch(sa);
              }
              break;
            case KEY_WPN:
              /* 2 maps to F2 */
              {
                KeyWPN wpn;
                hmi::dispatch(wpn);
              }
              break;
            case KEY_DEF:
              /* 3 maps to F3 */
              {
                KeyDEF def;
                hmi::dispatch(def);
              }
              break;
            case KEY_SYS:
              /* 4 maps to F4 */
              {
                KeySYS sys;
                hmi::dispatch(sys);
              }
              break;
            case KEY_DRV:
              /* 5 maps to F5 */
              {
                KeyDRV drv;
                hmi::dispatch(drv);
              }
              break;
            case KEY_STR:
              /* 6 maps to F6 */
              {
                KeySTR str;
                hmi::dispatch(str);
              }
              break;
            case KEY_COM:
              /* 7 maps to F7 */
              {
                KeyCOM com;
                hmi::dispatch(com);
              }
              break;
            case KEY_BMS:
              /* 8 maps to F8 */
              {
                KeyBMS bms;
                hmi::dispatch(bms);
              }
              break;
            case KEY_F14:
              /* Alarms */
              {
                functionKeysType left = ALARM_KEYS_LEFT;
                functionKeysType right = ALARM_KEYS_RIGHT;

                hmi::getScreen()->compass.visible = false;
                hmi::getScreen()->canvas.visible = false;
                hmi::getScreen()->alarms.visible = true;
                hmi::getScreen()->functionTop->active = 0x1 << 0;
                hmi::getScreen()->functionLeft = left;
                hmi::getScreen()->functionRight = right;
              }
              break;
            case KEY_F19:
              /* Toggle labels */
              KeyFunction input;
              input.key = KEY_F19;
              hmi::dispatch(input);
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
                hmi::getScreen()->keyboard.visible =
                hmi::getScreen()->keyboard.visible ? false : true;
              }
              break;
            case KEY_F17:
              /* Up Arrow keyboard */
              {
                hmi::getScreen()->keyboard.mode = (hmi::getScreen()->keyboard.mode == KEYBOARD_UPPER) ?  KEYBOARD_LOWER :  KEYBOARD_UPPER; 
              }
              break;
            case KEY_F18:
              /* Down Arrow keyboard */
              {
                hmi::getScreen()->keyboard.mode = (hmi::getScreen()->keyboard.mode == KEYBOARD_NUMBERS) ?  KEYBOARD_UPPER :  KEYBOARD_NUMBERS; 
              }
              break;
            case KEY_PLUS:
              hmi::getScreen()->compass.bearing+=2;
              break;
            case KEY_GREATER:
              hmi::getScreen()->compass.bearingSight+=2;
              break;
            case KEY_MINUS: 
              hmi::getScreen()->compass.bearing-=2;
              break;
            case KEY_LESS:
              hmi::getScreen()->compass.bearingSight-=2;
              break;
            default:
              printf ("[GVA] KeyPress not defined 0x%x\n", event.key);
              update = false;
              break;
            }
        }
        if (update) hmi::getRendrer()->update (hmi::getScreen());
        break;
      case RESIZE_EVENT:
        {
              printf ("[GVA] WindowResize: %d x %d\n", event.resize.width, event.resize.height);
          if (event.resize.width != hmi::getRendrer()->getWidth () ||
              event.resize.height != hmi::getRendrer()->getHeight ())
            {
              printf ("[GVA] WindowResize: %d x %d\n", event.resize.width, event.resize.height);
              hmi::getRendrer()->setWidth (event.resize.width);
              hmi::getRendrer()->setHeight (event.resize.height);
              hmi::getRendrer()->update (hmi::getScreen());
            }
        }
        break;
      case REDRAW_EVENT:
        {
          hmi::getRendrer()->update (hmi::getScreen());
        }
        break;
    }
  }

  /*
   * Clean up code goes here
   */
  cout << "Exiting hmi_display...\n";
}
