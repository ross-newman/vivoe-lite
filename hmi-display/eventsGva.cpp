#include <cairo-xlib.h>
#include "gva.hpp"
#include "rendererGva.hpp"
#include "eventsGva.hpp"

namespace gva
{
  eventsGva::eventsGva(Display *display, Window *window, touchGva *touch) { 
    m_display = display;
    m_window = window;
    m_touch = touch;
    
    /* 
     * select kind of events we are interested in 
     */
    XSelectInput (m_display, *m_window,
                  KeyPressMask | KeyReleaseMask | 
                  ButtonPressMask	| ButtonReleaseMask |
                  ExposureMask  | StructureNotifyMask);
  }

  int 
  eventsGva::nextGvaEvent(eventGvaType *event) {
    XEvent e;
    int binding = 0;

    XNextEvent (m_display, &e);
    
    event->type == NO_EVENT;

    switch (e.type) {
      case ButtonPressMask :
        m_touch->check(SA, &binding, e.xbutton.x, e.xbutton.y);
        if (binding) {
          event->type = KEY_EVENT;
          event->key = (gvaKeyEnum)binding;
        }
        break;
      case ButtonReleaseMask :
        break;
      case KeyPress:
        {
          event->type = KEY_EVENT;
          switch (e.xkey.keycode)
            {
            case 0x09:
              /* exit on ESC key press */
              event->key = KEY_ESC;
              break;
            case 0xa:
              /* 1 maps to SA */
              event->key = KEY_SA;
              break;
            case 0xb:
              /* 2 maps to WPN */
              event->key = KEY_WPN;
              break;
            case 0xc:
              /* 3 maps to DEF */
              event->key = KEY_DEF;
              break;
            case 0xd:
              /* 4 maps to SYS */
              event->key = KEY_SYS;
              break;
            case 0xe:
              /* 5 maps to DRV */
              event->key = KEY_DRV;
              break;
            case 0xf:
              /* 6 maps to STR */
              event->key = KEY_STR;
              break;
            case 0x10:
              /* 7 maps to COM */
              event->key = KEY_COM;
              break;
            case 0x11:
              /* 8 maps to BMS */
              event->key = KEY_BMS;
              break;
            case 0x26:
              /* a maps to ALARMS */
              event->key = KEY_F14;
              break;
            case 0x29:
              /* f toggle fullscreen TODO: Does not work */
              event->key = KEY_FULLSCREEN;
              break;
            case 0x2d:
              /* k toggle keyboard */
              event->key = KEY_KEYBOARD;
              break;
            case 0x42:
              /* caps_lock keyboard */
              event->key = KEY_F17;
              break;
            case 0x4D:
              /* num_lock keyboard */
              event->key = KEY_F18;
              break;
            case 0x2e:
              /* l show / hide labels */
              event->key = KEY_F19;
              break;
            case 0x15:
              /* + keyboard */
              event->key = KEY_PLUS;
              break;
            case 0x14:
              /* - show / hide labels */
              event->key = KEY_MINUS;
              break;
            case 0x3c:
              /* > keyboard */
              event->key = KEY_GREATER;
              break;
            case 0x3b:
              /* < show / hide labels */
              event->key = KEY_LESS;
              break;
            default:
              printf ("KeyPress not defined 0x%x\n", e.xkey.keycode);
              break;
            }
        }      
      case KeyRelease:
        {
          /* Nothing here yet */
        }
        break;
      case 25:
          printf("Resize %d\n", 25);
      case ConfigureNotify:
        {
          XConfigureEvent *cev = &e.xconfigure;
          event->type = RESIZE_EVENT;
          event->resize.width = cev->width;
          event->resize.height = cev->height;
          printf("Resize %d %d %d\n", ConfigureNotify,  cev->width,  cev->height);
        }
        break;
      case Expose:
        {
          event->type = REDRAW_EVENT;
        }
        break;
      default:
        {
          /* Unknown X11 key event */
          printf("Unknown X11 event %d\n",e.type);
        }
        break;
    }

  #if 0
    while ( nextGvaEvent(event) == GVA_SUCCESS ) {
      /* Process other events */
    }
  #endif
    return GVA_SUCCESS;
  }

  void
  eventsGva::flush() {
    if (m_display) {
      XClientMessageEvent dummyEvent;

      XSync (m_display, true);

      dummyEvent.type = Expose;
      dummyEvent.window = *m_window;
      dummyEvent.format = 32;

      XSendEvent (m_display, *m_window, False, StructureNotifyMask, (XEvent *) & dummyEvent);
      XFlush (m_display);
    }
  }
}
