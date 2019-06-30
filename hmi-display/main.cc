/**
 * @file main.c
 * @author ross@rossnewman.com
 * @date 04 June 2019
 * @brief The example HMI application.
 */
#include <iostream>
#include <string>
#include <unistd.h>
//#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include "gva.h"
#include "renderer_map.h"
#include "events_gva.h"
#include "hmi_gva.h"
#include "log_gva.h"

using namespace std;
using namespace gva;

struct opts {
  bool videoEnabled;
  bool windowEnabled;
  char config[256];
};

opts opt = {0};

void dispatch(int key) {
  KeyFunction input;
  input.key = key;
  hmi::dispatch(input);
};

int
getopt(int argc, char *argv[])
{
  int c = 0;
  while ((c = getopt (argc, argv, "hvwcl:")) != -1) 
  switch (c) {
    case 'v':
      cout << "Version " << MAJOR << "." << MINOR << "." << PATCH << endl;
      return 0;
    case 'w':
      opt.windowEnabled = true;
      return -1;
    case 'c':
      strcpy(opt.config,optarg);
      return -1;
    case 'x':
      opt.videoEnabled = true;
      return -1;
    case 'h':
      cout << "  -c : XML config file" << endl;
      cout << "  -h : help" << endl;
      cout << "  -w : Show HMI window, for debug when no GVA display present" << endl;
      cout << "  -v : Version" << endl;
      cout << "  -l : Live video" << endl;
      return 0;
    case '?':
      if (optopt == 'c')
        cerr << "Option -" << optopt << " requires an argument.\n" << endl;
      else if (isprint (optopt))
        cerr << "Unknown option `-%c'.\n" << endl;
      else
        cerr << std::hex << "Unknown option character, -h for help'" << optopt << "'." << endl;
      return 1;
    default:
      abort ();
  }
};

int
main (int argc, char *argv[])
{
  eventGvaType event;
  int done = 0;
  char* rtpBuffer;
  int hndl;
  bool update;
  char ipaddr[] = "127.0.0.1";
  int port = 5004;
  char tmp[256];
  
  cout << "hmi_display (author: ross@rossnewman.com)..." << endl;
  
  int ret = getopt(argc, argv);
  if (ret >= 0) return ret;
  
  // instantiate events
  KeyPowerOn on;

  hmi::start();
  
  hmi::dispatch(on);
  
  //
  // Initalise the display events
  //
  eventsGva io(hmi::getRendrer()->getDisplay(), hmi::getRendrer()->getWindow(), hmi::getRendrer()->getTouch());
  
  //
  // Setup video sources (default size will be 640 x 480 unless specified)
  //
  gvaVideoRtpYuv *rtpStream1 = new gvaVideoRtpYuv(ipaddr, port);
  cout << "Resolution " << rtpStream1->getHeight() << "x" << rtpStream1->getWidth() << "\n";
  rtpBuffer = (char*)malloc(rtpStream1->getHeight() * rtpStream1->getWidth() * 4); 
  sprintf(tmp, "GVA Incomming RTP stream initalised %s:%d", ipaddr, port);
  logGva::log (tmp, LOG_INFO);
  
  while (!done) {
    update = true;
    io.nextGvaEvent(&event);

    if (opt.videoEnabled)
    {
      // Get the live video frame if Driver (DRV)
      if (hmi::getScreen()->currentFunction == DRV) {
        rtpStream1->gvaRecieveFrame(rtpBuffer, RGBA_COLOUR);
        io.flush();
      }
    }
    
    switch (event.type) {
      case KEY_EVENT :
        {
          switch (event.key)
            {
            case KEY_ESC:
              // exit on ESC key press 
              done = 1;
              break;
            case KEY_SA:
              // 1 maps to F1 
              {
                KeySA sa;
                hmi::dispatch(sa);
              }
              break;
            case KEY_WPN:
              // 2 maps to F2
              {
                KeyWPN wpn;
                hmi::dispatch(wpn);
              }
              break;
            case KEY_DEF:
              // 3 maps to F3
              {
                KeyDEF def;
                hmi::dispatch(def);
              }
              break;
            case KEY_SYS:
              // 4 maps to F4
              {
                KeySYS sys;
                hmi::dispatch(sys);
              }
              break;
            case KEY_DRV:
              // 5 maps to F5
              {
                KeyDRV drv;
                hmi::dispatch(drv);
              }
              break;
            case KEY_STR:
              // 6 maps to F6
              {
                KeySTR str;
                hmi::dispatch(str);
              }
              break;
            case KEY_COM:
              // 7 maps to F7
              {
                KeyCOM com;
                hmi::dispatch(com);
              }
              break;
            case KEY_BMS:
              // 8 maps to F8 
              {
                KeyBMS bms;
                hmi::dispatch(bms);
              }
              break;
            case KEY_F1:
              dispatch(KEY_F1);
              break;
            case KEY_F2:
              dispatch(KEY_F2);
              break;
            case KEY_F3:
              dispatch(KEY_F3);
              break;
            case KEY_F4:
              dispatch(KEY_F4);
              break;
            case KEY_F5:
              dispatch(KEY_F5);
              break;
            case KEY_F6:
              dispatch(KEY_F6);
              break;
            case KEY_F7:
              dispatch(KEY_F7);
              break;
            case KEY_F8:
              dispatch(KEY_F8);
              break;
            case KEY_F9:
              dispatch(KEY_F9);
              break;
            case KEY_F10:
              dispatch(KEY_F10);
              break;
            case KEY_F11:
              dispatch(KEY_F11);
              break;
            case KEY_F12:
              dispatch(KEY_F12);
              break;
            case KEY_F13:
              // Control UP 
              {
                dispatch(KEY_F13);
              }
              break;
            case KEY_F14:
              // Control Alarms
              {
                KeyAlarms alarms;
                KeyFunction input;
                hmi::dispatch(alarms);
                input.key = KEY_F14;
                hmi::dispatch(input);
              }
              break;
            case KEY_F15:
              // F15
              {
                KeyFunction input;
                input.key = KEY_F15;
                hmi::dispatch(input);
              }
              break;
            case KEY_F16:
              // F16
              {
                KeyFunction input;
                input.key = KEY_F16;
                hmi::dispatch(input);
              }
              break;
            case KEY_F17:
              // F17 Control Arrow Up
              {
                KeyFunction input;
                input.key = KEY_F17;
                hmi::getScreen()->keyboard.mode = (hmi::getScreen()->keyboard.mode == KEYBOARD_UPPER) ?  KEYBOARD_LOWER :  KEYBOARD_UPPER; 
                hmi::dispatch(input);
              }
              break;
            case KEY_F18:
              // F18 Control Arrow Down
              {
                KeyFunction input;
                input.key = KEY_F18;
                hmi::getScreen()->keyboard.mode = (hmi::getScreen()->keyboard.mode == KEYBOARD_NUMBERS) ?  KEYBOARD_UPPER :  KEYBOARD_NUMBERS; 
                hmi::dispatch(input);
              }
              break;
            case KEY_F19:
              // F19 Control labels 
              {
                KeyFunction input;
                input.key = KEY_F19;
                hmi::dispatch(input);
              }
              break;
            case KEY_F20:
              // F20
              {
                KeyFunction input;
                input.key = KEY_F20;
                hmi::dispatch(input);
              }
              break;
            case KEY_FULLSCREEN:
              // f toggle fullscreen TODO: Does not work 
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
              // k toggle keyboard 
              {
                hmi::getScreen()->keyboard.visible = hmi::getScreen()->keyboard.visible ? false : true;
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

  //
  // Clean up code goes here
  //
  free(rtpStream1);
  cout << "Exiting hmi_display...\n";
}
