#ifndef EVENTS_GVA_H
#define EVENTS_GVA_H
#include <X11/Xlib.h>
#include "gva.h"

using namespace gva;

enum gvaKeyEnum 
{
  /* These are physical GVA keys around the display */
  KEY_SA = 0,
  KEY_WPN,
  KEY_DEF,
  KEY_SYS,
  KEY_DRV,
  KEY_STR,
  KEY_COM,
  KEY_BMS,
  KEY_F1,
  KEY_F2,
  KEY_F3,
  KEY_F4,
  KEY_F5,
  KEY_F6,
  KEY_F7,
  KEY_F8,
  KEY_F9,
  KEY_F10,
  KEY_F11,
  KEY_F12,
  KEY_F13,
  KEY_F14,
  KEY_F15,
  KEY_F16,
  KEY_F17,
  KEY_F18,
  KEY_F19,
  KEY_F20,
  KEY_F21,
  KEY_F22,
  KEY_BLACKOUT,
  KEY_POWER,
  KEY_BRIGHTNESS_UP,
  KEY_BRIGHTNESS_DOWN,
  /* These are fake keys that only exist in the software (emulated world) */
  KEY_ESC,
  KEY_FULLSCREEN,
  KEY_PLUS,
  KEY_MINUS,
  KEY_GREATER,
  KEY_LESS,
  KEY_KEYBOARD
};

enum eventEnumType {
  NO_EVENT = 0,
  KEY_EVENT,
  TOUCH_EVENT,
  DDS_EVENT,
  RESIZE_EVENT,
  REDRAW_EVENT
};

struct touchType {
  int x;
  int y;
};

struct resolutionType {
  int width;
  int height;
};

class eventGvaType {
public:
  eventEnumType type;
  gvaKeyEnum key;
  touchType toutch;
  resolutionType resize;
};

class eventsGva {
public:
  eventsGva() { m_display = 0; };
  eventsGva(Display *display, Window *window); 
  int nextGvaEvent(eventGvaType *event); // Use for X11/DDS/Touch events
  void flush();
private:
 Display *m_display = 0;
 Window *m_window = 0;
};

#endif
