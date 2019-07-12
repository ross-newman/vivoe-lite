// 
// MIT License
// 
// Copyright (c) 2019 Ross Newman (ross@rossnewman.com)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// 

#include "gva.h"
#include "renderer_gva.h"
#include "events_gva.h"

namespace gva
{
  EventsGva::EventsGva(gtkType *window, touchGva *touch) { 
    window_ = window;
    touch_ = touch;
  }

  // Handle button press events by either drawing a rectangle
  // or clearing the surface, depending on which button was pressed.
  // The ::button-press signal handler receives a GdkEventButton
  // struct which contains this information.
  //
  gboolean 
  EventsGva::ButtonPressEventCb (GtkWidget      *widget,
                         GdkEventButton *event,
                         gpointer        data)
  {
    printf("[GVA] Mouse event %d\n", event->button);
    if (event->button == GDK_BUTTON_PRIMARY)
      {
        eventqueue_.push_back(EventGvaType((int)event->x, (int)event->y));
      }
    else if (event->button == GDK_BUTTON_SECONDARY)
      {
         // Not much to do here now
      }

    /* We've handled the event, stop processing */
    return TRUE;
  }

  gboolean 
  EventsGva::KeyPressEventCb (GtkWidget *widget, GdkEventKey *event) {
    GdkModifierType modifiers;

    modifiers = gtk_accelerator_get_default_mod_mask ();
    printf("[GVA] Key event %d\n", event->keyval);
    g_print ("send_event=%d, state=%u, keyval=%u, length=%d, string='%s', hardware_keycode=%u, group=%u\n", 
event->send_event, event->state, event->keyval, event->length, event->string, event->hardware_keycode, 
event->group);
    /* The event was handled, and the emission should stop */
    return TRUE;
  }

  int 
  EventsGva::NextGvaEvent(EventGvaType *event) {
#if X11
    XEvent e;
    int binding = 0;

    XNextEvent (window_->dpy, &e);
    
    event->type == NO_EVENT;

    switch (e.type) {
      case ButtonPressMask :
        touch_->check(TOP, &binding, e.xbutton.x, e.xbutton.y);
        if (!binding) touch_->check(BOTTOM, &binding, e.xbutton.x, e.xbutton.y);
        if (!binding) touch_->check(RIGHT, &binding, e.xbutton.x, e.xbutton.y);
        if (!binding) touch_->check(LEFT, &binding, e.xbutton.x, e.xbutton.y);
        if (binding) {
          event->type = KEY_EVENT;
          event->key_ = (gvaKeyEnum)binding;
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
            event->key_ = KEY_ESC;
            break;
          case 0xa:
            /* 1 maps to SA */
            event->key_ = KEY_SA;
            break;
          case 0xb:
            /* 2 maps to WPN */
            event->key_ = KEY_WPN;
            break;
          case 0xc:
            /* 3 maps to DEF */
            event->key_ = KEY_DEF;
            break;
          case 0xd:
            /* 4 maps to SYS */
            event->key_ = KEY_SYS;
            break;
          case 0xe:
            /* 5 maps to DRV */
            event->key_ = KEY_DRV;
            break;
          case 0xf:
            /* 6 maps to STR */
            event->key_ = KEY_STR;
            break;
          case 0x10:
            /* 7 maps to COM */
            event->key_ = KEY_COM;
            break;
          case 0x11:
            /* 8 maps to BMS */
            event->key_ = KEY_BMS;
            break;
          case 0x26:
            /* a maps to ALARMS */
            event->key_ = KEY_F14;
            break;
          case 0x29:
            /* f toggle fullscreen TODO: Does not work */
            event->key_ = KEY_FULLSCREEN;
            break;
          case 0x2d:
            /* k toggle keyboard */
            event->key_ = KEY_KEYBOARD;
            break;
          case 0x42:
            /* caps_lock keyboard */
            event->key_ = KEY_F17;
            break;
          case 0x4D:
            /* num_lock keyboard */
            event->key_ = KEY_F18;
            break;
          case 0x2e:
            /* l show / hide labels */
            event->key_ = KEY_F19;
            break;
          case 0x15:
            /* + keyboard */
            event->key_ = KEY_PLUS;
            break;
          case 0x14:
            /* - show / hide labels */
            event->key_ = KEY_MINUS;
            break;
          case 0x3c:
            /* > keyboard */
            event->key_ = KEY_GREATER;
            break;
          case 0x3b:
            /* < show / hide labels */
            event->key_ = KEY_LESS;
            break;
          default:
            printf ("KeyPress not defined 0x%x\n", e.xkey.keycode);
            break;
          }
          break;
        }      
      case KeyRelease:
        {
          /* Nothing here yet */
        }
        break;
      case 25:
      case ConfigureNotify:
        {
          XConfigureEvent *cev = &e.xconfigure;
          event->type = RESIZE_EVENT;
          event->resize_.width = cev->width;
          event->resize_.height = cev->height;
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
#endif
    return GVA_SUCCESS;
  }

  void
  EventsGva::Flush() {
#if X11
    if (window_->dpy) {
      XClientMessageEvent dummyEvent;

      XSync (window_->dpy, true);

      dummyEvent.type = Expose;
      dummyEvent.window = window_->win;
      dummyEvent.format = 32;

      XSendEvent (window_->dpy, window_->win, False, StructureNotifyMask, (XEvent *) & dummyEvent);
      XFlush (window_->dpy);
    }
#else
#endif
  }
}
