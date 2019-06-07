/**
 * @file main.c
 * @author ross@rossnewman.com
 * @date 04 June 2019
 * @brief The example HMI application.
 */
#include <iostream>
#include <unistd.h>
#include "debug.h"
#include "pngget.h"
#include "screenGva.h"

using namespace std;

#define LABEL_NULL "Unused!"

// These labels should not change
#define COMMON_KEYS { true, 0b0010000, 0b00000000, "Up", "Alarms", "Threats", "Ack", "↑", "↓", "Labels", "Enter" }
#define COMMON_FUNCTION_KEYS_TOP { true, 0b00010000, 0b00001000 }
#define COMMON_STATUS_BAR { true,  "12:30:00, 03/06/2019", "BNGF: 216600, 771200", "W0", "A0", "C0" }
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

int
main (int argc, char *argv[])
{
  XEvent event;
  int done = 0;
  /* 4:3 aspect ratio */
  resolution_type view = { 640, 480, 24 };
  /* 16:9 aspect ration */
//  resolution_type view = { 1080, 720, 24 };
  int hndl;
  /* These are comon to all screens */
  statusBarType status = COMMON_STATUS_BAR;
  functionSelectType top = COMMON_FUNCTION_KEYS_TOP;
//  screenType screen_bms = { "Battle Management System", &top, &status, TEST_FUNCTION_KEYS_LEFT, TEST_FUNCTION_KEYS_RIGHT, COMMON_KEYS, COMPASS };
  canvasType canvas = { true, "test2.png" };
  screenType screen_sa =
    { "Situational Awareness", canvas, &top, &status, SYS_FUNCTION_KEYS_LEFT, SYS_FUNCTION_KEYS_RIGHT, COMMON_KEYS, COMPASS }; 
  screenType *screen = &screen_sa;
  screenGva *render = new screenGva (screen, view.width, view.height);

  cout << "hmi_display (v" << MAJOR << "." << MINOR << "." << PATCH << ") author ross@rossnewman.com...\n";

  if (XInitThreads() == 0) {
	  printf("Error setting XInitThreads\n");
	  return -1;
  }

  Display *d = render->getDisplay ();
  Window *w = render->getWindow ();

  /* select kind of events we are interested in */
  XSelectInput (d, *w, KeyPressMask | KeyReleaseMask | StructureNotifyMask);
  
  render->update(screen);
  render->startClock(&status);
  
  while (!done)
    {
      XLockDisplay(d); 
      XNextEvent (d, &event);

      switch (event.type)
        {
        case KeyPress:
          {
            switch (event.xkey.keycode) {
				/* exit on ESC key press */
				case 0x09 : 
                    done = 1;
                    break;
				/* 1 maps to F1 */
				case 0xa : 
                    if (!BIT(7, screen->functionTop->hidden)) 
                    {
                      functionKeysType left = SA_FUNCTION_KEYS_LEFT; 
                      functionKeysType right = SA_FUNCTION_KEYS_RIGHT;
                      
                      screen->compass.visible = true;
                      screen->canvas.visible = true;
                      strcpy(screen->canvas.filename, "test2.png");
                      screen->functionTop->active = 0x1 << 7;
                      screen->functionLeft = left;
                      screen->functionRight = right;
                    }
                    break;
				/* 2 maps to F2 */
				case 0xb : 
                    if (!BIT(6, screen->functionTop->hidden)) 
                    {
                      functionKeysType left = WPN_FUNCTION_KEYS_LEFT; 
                      functionKeysType right = WPN_FUNCTION_KEYS_RIGHT;
                      
                      screen->compass.visible = true;
                      screen->canvas.visible = true;
                      strcpy(screen->canvas.filename, "test2.png");
                      screen->functionTop->active = 0x1 << 6;
                      screen->functionLeft = left;
                      screen->functionRight = right;
                    }
                    break;
				/* 3 maps to F3 */
				case 0xc : 
                    if (!BIT(5, screen->functionTop->hidden)) 
                    {
                      functionKeysType left = DEF_FUNCTION_KEYS_LEFT; 
                      functionKeysType right = DEF_FUNCTION_KEYS_RIGHT;
                      
                      screen->compass.visible = false;
                      screen->canvas.visible = false;
                      screen->functionTop->active = 0x1 << 5;
                      screen->functionLeft = left;
                      screen->functionRight = right;
                    }
                    break;
				/* 4 maps to F4 */
				case 0xd : 
                    if (!BIT(4, screen->functionTop->hidden))
                    {
                      functionKeysType left = SYS_FUNCTION_KEYS_LEFT; 
                      functionKeysType right = SYS_FUNCTION_KEYS_RIGHT;
                      
                      screen->compass.visible = true;
                      screen->canvas.visible = true;
                      strcpy(screen->canvas.filename, "test2.png");
                      screen->functionTop->active = 0x1 << 4;
                      screen->functionLeft = left;
                      screen->functionRight = right;
                    }
                    break;
				/* 5 maps to F5 */
				case 0xe : 
                    if (!BIT(3, screen->functionTop->hidden)) 
                    {
                      functionKeysType left = DRV_FUNCTION_KEYS_LEFT; 
                      functionKeysType right = DRV_FUNCTION_KEYS_RIGHT;
                      
                      screen->compass.visible = false;
                      screen->canvas.visible = false;
                      screen->functionTop->active = 0x1 << 3;
                      screen->functionLeft = left;
                      screen->functionRight = right;
                    }
                    break;
				/* 6 maps to F6 */
				case 0xf : 
                    if (!BIT(2, screen->functionTop->hidden))
                    {
                      functionKeysType left = STR_FUNCTION_KEYS_LEFT; 
                      functionKeysType right = STR_FUNCTION_KEYS_RIGHT;
                      
                      screen->compass.visible = false;
                      screen->canvas.visible = false;
                      screen->functionTop->active = 0x1 << 2;
                      screen->functionLeft = left;
                      screen->functionRight = right;
                    }
                    break;
				/* 7 maps to F7 */
				case 0x10 : 
                    if (!BIT(1, screen->functionTop->hidden)) 
                    {
                      functionKeysType left = COM_FUNCTION_KEYS_LEFT; 
                      functionKeysType right = COM_FUNCTION_KEYS_RIGHT;
                      
                      screen->compass.visible = false;
                      screen->compass.visible = false;
                      screen->canvas.visible = false;
                      screen->functionTop->active = 0x1 << 1;
                      screen->functionLeft = left;
                      screen->functionRight = right;
                    }
                    break;
				/* 8 maps to F8 */
				case 0x11 : 
                    if (!BIT(0, screen->functionTop->hidden)) 
                    {
                      functionKeysType left = BMS_FUNCTION_KEYS_LEFT; 
                      functionKeysType right = BMS_FUNCTION_KEYS_RIGHT;
                      
                      screen->compass.visible = false;
                      screen->canvas.visible = true;
                      strcpy(screen->canvas.filename, "map.png");
                      screen->functionTop->active = 0x1 << 0;
                      screen->functionLeft = left;
                      screen->functionRight = right;
                    }
                    break;
				/* l toggle labels */
				case 0x2e : 
                    screen->functionLeft.visible = screen->functionLeft.visible ? false : true; 
                    screen->functionRight.visible = screen->functionRight.visible ? false : true; 
                    screen->control.visible = screen->control.visible ? false : true; 
                    screen->statusBar->visible = screen->statusBar->visible ? false : true; 
                    screen->compass.visible = screen->compass.visible ? false : true; 
                    break;
              }
          }
          render->update(screen);
          break;
        case KeyRelease:
          {
            /* Nothing here yet */
          }
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
        XUnlockDisplay(d); 
    }

  /*
   * Clean up code goes here
   */
  cout << "Exiting hmi_display...\n";
}
