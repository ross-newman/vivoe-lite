//
// MIT License
//
// Copyright (c) 2020 Ross Newman (ross@rossnewman.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#ifndef GVA_H
#define GVA_H
#include "debug.h"
#include "log_gva.h"

#define MAJOR 0
#define MINOR 1
#define PATCH 2948

#define MIN_HEIGHT 480
#define MIN_WIDTH 640

enum GvaFunctionEnum {
  SA = 0,
  WPN,
  DEF,
  SYS,
  DRV,
  STR,
  COM,
  BMS,
  ALARMSX,
};

enum GvaAlarmType { ALARM_WARNING, ALARM_CRITICAL, ALARM_ALERT };

enum GvaFunctionGroupEnum { TOP, BOTTOM, LEFT, RIGHT, ALARM_TABLE, KEYBOARD };

enum GvaKeyEnum {
  /* These are physical GVA keys around the display */
  KEY_NONE = 0,
  KEY_SA,
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

enum GvaStatusTypes {
  GVA_SUCCESS = 0,
  GVA_ERROR,
  GVA_NETWORK_ERROR,
  GVA_DDS_ERROR,
  GVA_XML_ERROR,
  GVA_RTP_ERROR
};

#endif
