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

namespace gva {
  static unsigned int previous_key_;

  EventsGva::EventsGva(gtkType * window, TouchGva * touch) {
    window_ = window;
    touch_ = touch;
    previous_key_ = 0;
  }
  
  // Handle button press events by either Drawing a rectangle
  // or clearing the surface, depending on which button was pressed.
  // The ::button-press signal handler receives a GdkEventButton
  // struct which contains this information.
  gboolean EventsGva::ButtonPressEventCb(GtkWidget * Widget,
                                     GdkEventButton * event,
                                     gpointer data) {
//printf("[GVA] Mouse event %d\n", event->button);

    if (event->button == GDK_BUTTON_PRIMARY) {
      EventGvaType gvaEvent;
      int binding = 0;

      touch_->check(TOP, &binding, event->x, event->y);
      if (!binding)
        touch_->check(BOTTOM, &binding, event->x, event->y);
      if (!binding)
        touch_->check(RIGHT, &binding, event->x, event->y);
      if (!binding)
        touch_->check(LEFT, &binding, event->x, event->y);
      if (binding) {
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = (GvaKeyEnum) binding;
        eventqueue_.push_back(gvaEvent);
      }
    } else if (event->button == GDK_BUTTON_SECONDARY) {
      // Not much to do here now as second button is unused
    }

    // We've handled the event, stop processing 
    return TRUE;
  }

  gboolean EventsGva::KeyPressEventCb(GtkWidget * Widget, GdkEventKey * event) {
    GdkModifierType modifiers;
    EventGvaType gvaEvent;

    modifiers = gtk_accelerator_get_default_mod_mask();
    printf("[GVA] Key event 0x%x (prev 0x%x)\n", event->keyval, previous_key_);
    switch (previous_key_) {
      case 0xffe3:             // Top keys
        switch (event->keyval) {
          case 0xffbe:
            /* 1 maps to SA */
            gvaEvent.type = KEY_EVENT;
            gvaEvent.key_ = KEY_SA;
            break;
          case 0xffbf:
            /* 2 maps to WPN */
            gvaEvent.type = KEY_EVENT;
            gvaEvent.key_ = KEY_WPN;
            break;
          case 0xffc0:
            /* 3 maps to DEF */
            gvaEvent.type = KEY_EVENT;
            gvaEvent.key_ = KEY_DEF;
            break;
          case 0xffc1:
            /* 4 maps to SYS */
            gvaEvent.type = KEY_EVENT;
            gvaEvent.key_ = KEY_SYS;
            break;
          case 0xffc2:
            /* 5 maps to DRV */
            gvaEvent.type = KEY_EVENT;
            gvaEvent.key_ = KEY_DRV;
            break;
          case 0xffc3:
            /* 6 maps to STR */
            gvaEvent.type = KEY_EVENT;
            gvaEvent.key_ = KEY_STR;
            break;
          case 0xffc4:
            /* 7 maps to COM */
            gvaEvent.type = KEY_EVENT;
            gvaEvent.key_ = KEY_COM;
            break;
          case 0xffc5:
            /* 8 maps to BMS */
            gvaEvent.type = KEY_EVENT;
            gvaEvent.key_ = KEY_BMS;
            break;
          default:
            break;
        }
        printf("[GVA] Top event 0x%x\n", event->keyval);
        previous_key_ = event->keyval;
        if (gvaEvent.type != NO_EVENT)
          eventqueue_.push_back(gvaEvent);
        return TRUE;
    }
//    g_print ("send_event=%d, state=%u, keyval=%u, length=%d, string='%s', hardware_keycode=%u, group=%u\n", gvaEvent.send_event, gvaEvent.state, gvaEvent.keyval, gvaEvent.length, gvaEvent.string, gvaEvent.hardware_keycode, gvaEvent.group);
    /* The event was handled, and the emission should stop */
    switch (event->keyval) {
      case 65307:
        /* exit on ESC key press */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_ESC;
        break;
      case 49:
        /* 1 maps to SA */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_SA;
        break;
      case 50:
        /* 2 maps to WPN */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_WPN;
        break;
      case 51:
        /* 3 maps to DEF */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_DEF;
        break;
      case 52:
        /* 4 maps to SYS */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_SYS;
        break;
      case 53:
        /* 5 maps to DRV */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_DRV;
        break;
      case 54:
        /* 6 maps to STR */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_STR;
        break;
      case 55:
        /* 7 maps to COM */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_COM;
        break;
      case 56:
        /* 8 maps to BMS */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_BMS;
        break;
      case 0xffbe:
        /* F1 */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_F1;
        break;
      case 0xffbf:
        /* F2 */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_F2;
        break;
      case 0xffc0:
        /* F3 */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_F3;
        break;
      case 0xffc1:
        /* F4 */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_F4;
        break;
      case 0xffc2:
        /* F5 */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_F5;
        break;
      case 0xffc3:
        /* F6 */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_F6;
        break;
      case 0xffc4:
        /* F7 */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_F7;
        break;
      case 0xffc5:
        /* F8 */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_F8;
        break;
      case 0xffc6:
        /* F9 */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_F9;
        break;
      case 0xffc7:
        /* F10 */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_F10;
        break;
      case 0xffc8:
        /* F11 */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_F11;
        break;
      case 0xffc9:
        /* F12 */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_F12;
        break;
      case 65:
      case 97:
      case 0xffe9:
        /* a maps to ALARMS */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_F14;
        break;
      case 0xff0d:
        /* Enter maps to F20 */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_F20;
        break;
      case 70:
      case 102:
        /* f toggle fullscreen TODO: Does not work */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_FULLSCREEN;
        break;
      case 0x62:
        /* b toggle blackout F11 */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_F11;
        break;
      case 75:
      case 107:
        /* k toggle keyboard */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_KEYBOARD;
        break;
      case 65509:
        /* caps_lock keyboard */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_F17;
        break;
      case 65407:
        /* num_lock keyboard */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_F18;
        break;
      case 76:
      case 108:
        /* l or L show / hide labels */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_F19;
        break;
      case 43:
        /* + keyboard */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_PLUS;
        break;
      case 95:
        /* - show / hide labels */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_MINUS;
        break;
      case 62:
        /* > keyboard */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_GREATER;
        break;
      case 60:
        /* < show / hide labels */
        gvaEvent.type = KEY_EVENT;
        gvaEvent.key_ = KEY_LESS;
        break;
      default:
        printf("[GVA] KeyPress not defined 0x%x\n", event->keyval);
        previous_key_ = event->keyval;
        return TRUE;
    }
    if (gvaEvent.type != NO_EVENT)
      eventqueue_.push_back(gvaEvent);
    return TRUE;
  }

  int
   EventsGva::NextGvaEvent(EventGvaType * event) {
    EventGvaType popEvent;

    popEvent.type == NO_EVENT;
    if (!eventqueue_.empty()) {
      popEvent = eventqueue_.back();
      eventqueue_.pop_back();
    }
    *event = popEvent;
    return GVA_SUCCESS;
  }
}
