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

#ifndef EVENTS_GVA_H
#define EVENTS_GVA_H
#include <X11/Xlib.h>
#include "renderer_gva.h"
#include "gva.h"

namespace gva
{
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
    eventsGva(Display *display, Window *window, touchGva *touch); 
    int nextGvaEvent(eventGvaType *event); // Use for X11/DDS/Touch events
    void flush();
  private:
   Display *m_display;
   Window *m_window;
   touchGva *m_touch;
  };
};

#endif
