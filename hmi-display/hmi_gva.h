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

#ifndef HMI_GVA_H
#define HMI_GVA_H
#include <iostream>
#include "tinyfsm.h"
#include "renderer_map.h"
#include "view_gva.h"
#include "screen_gva.h"
#include "xml_reader.h"

using namespace gva;

#define LABEL_NULL "Unused!"

// 
// These labels should not change
// 

//                              Visible    Active    Hidden
#define COMMON_KEYS              { true, 0b0010000,  0b00010000, "Up", "Alarms", "Threats", "Ack", "↑", "↓", "Labels", "Enter" }
#define COMMON_FUNCTION_KEYS_TOP { true, 0b01000000, 0b0000100 }
#define COMMON_STATUS_BAR        { true, DEFAULT_HEIGHT-11, 0, { LOCATION_FORMAT_MGRS, 51.500655, -0.124240 }, "12:30:00, 03/06/2019", "Lat:51.500655 Lon:-0.124240    [1,3]", "W:0", "A:5", "C:1", "O:2" }
#define COMPASS { true, 165, 370, 0, 55 }
//                              Visible    Active    Hidden 
#define TEST_FUNCTION_KEYS_LEFT  { true, 0b000001, 0b011100, { "F1", "F2", "F3", "F4", "F5", "F6" } }
#define TEST_FUNCTION_KEYS_RIGHT { true, 0b100000, 0b001111, { "F7", "F8", "F9", "F10", "F11", "F12" } }

//                              Visible    Active    Hidden    Toggle  ToggleOn
#define SA_FUNCTION_KEYS_LEFT    { true, 0b100000, 0b110111, 0b000000, 0b000000, { "Wpn Sight", "Quad", LABEL_NULL, "Front left", "Left", "Rear Left" } }
#define SA_FUNCTION_KEYS_RIGHT   { true, 0b000000, 0b000111, 0b000000, 0b000000, { LABEL_NULL, LABEL_NULL, LABEL_NULL, "Front right", "Right", "Rear right" } }

#define WPN_FUNCTION_KEYS_LEFT   { true, 0b000000, 0b110000, 0b000000, 0b000000, { "Wpn Sight", "IR Sight", LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL } }
#define WPN_FUNCTION_KEYS_RIGHT  { true, 0b000000, 0b000000, 0b000000, 0b000000, { LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL } }

#define DEF_FUNCTION_KEYS_LEFT   { true, 0b000000, 0b110000, 0b110000, 0b000000, { "ECM", "Spike", LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL } }
#define DEF_FUNCTION_KEYS_RIGHT  { true, 0b000000, 0b000000, 0b000000, 0b000000, { LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL } }

#define SYS_FUNCTION_KEYS_LEFT   { true, 0b100000, 0b111101, 0b000000, 0b000000, { "Overview", "Automotive", "System", "HUMS", LABEL_NULL, "BIT" } }
#define SYS_FUNCTION_KEYS_RIGHT  { true, 0b000000, 0b111111, 0b000000, 0b000000, { "Commander.Health", "Gunner.Health", "Driver.Health", "Roll.Allocation", "Mission", "Blackout" } }

#define DRV_FUNCTION_KEYS_LEFT   { true, 0b110000, 0b100000, 0b000000, 0b000000, { "Assist", "PPI Mode", LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL } }
#define DRV_FUNCTION_KEYS_RIGHT  { true, 0b000000, 0b000000, 0b000000, 0b000000, { LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL } }

#define STR_FUNCTION_KEYS_LEFT   { true, 0b000000, 0b000000, 0b000000, 0b000000, { LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL } }
#define STR_FUNCTION_KEYS_RIGHT  { true, 0b000000, 0b000000, 0b000000, 0b000000, { LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL } }

#define COM_FUNCTION_KEYS_LEFT   { true, 0b100000, 0b110000, 0b000000, 0b000000, { "Comms", "RF State", LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL } }
#define COM_FUNCTION_KEYS_RIGHT  { true, 0b000000, 0b000000, 0b000000, 0b000000, { LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL, LABEL_NULL } }

#define BMS_FUNCTION_KEYS_LEFT   { true, 0b100000, 0b101111, 0b000000, 0b000000, { "Mission", LABEL_NULL, "Shift.up", "Shift.down", "Zoom +", "Rotate.>>" } }
#define BMS_FUNCTION_KEYS_RIGHT  { true, 0b000000, 0b001111, 0b000000, 0b000000, { LABEL_NULL, LABEL_NULL, "Shift.left", "Shift.right", "Zoom -", "Rotate.<<" } }

#define ALARM_KEYS_LEFT          { true, 0b000000, 0b111100, 0b000000, 0b000000, { "Sort", "Order", "Show.Overridden", "Select.All", LABEL_NULL, LABEL_NULL } }
#define ALARM_KEYS_RIGHT         { true, 0b000000, 0b111011, 0b000000, 0b000000, { "Override", "Clear", "Add Note", LABEL_NULL, "Page.Up", "Page.Down" } }

// 
// forward declarations
// 
struct stateOff;
struct stateSA;
struct stateWPN;
struct stateDEF;
struct stateDRV;
struct stateSYS;
struct stateSTR;
struct stateCOM; 
struct stateBMS;
struct stateAlarms;

// 
// Event Declarations
//
struct KeyPowerOn : tinyfsm::Event { };
struct KeySA : tinyfsm::Event { };
struct KeyWPN : tinyfsm::Event { };
struct KeyDEF : tinyfsm::Event { };
struct KeySYS : tinyfsm::Event { };
struct KeyDRV : tinyfsm::Event { };
struct KeySTR : tinyfsm::Event { };
struct KeyCOM : tinyfsm::Event { };
struct KeyBMS : tinyfsm::Event { };
struct KeyAlarms : tinyfsm::Event { };
struct KeyFunction : tinyfsm::Event { int key; };

//
// 2. State Machine Base Class Declaration
//
struct Hmi : tinyfsm::Fsm<Hmi>
{
  virtual void react(KeyPowerOn const &) { };
  virtual void react(KeySA const &) { };
  virtual void react(KeyWPN const &) { };
  virtual void react(KeyDEF const &) { };
  virtual void react(KeySYS const &) { };
  virtual void react(KeyDRV const &) { };
  virtual void react(KeySTR const &) { };
  virtual void react(KeyCOM const &) { };
  virtual void react(KeyBMS const &) { };
  virtual void react(KeyAlarms const &) { };
  virtual void react(KeyFunction const &) { };

  // alternative: enforce handling of Toggle in all states (pure virtual)
  //virtual void react(Toggle const &) = 0;

  virtual void entry(void) { };  /* entry actions in some states */
  void         exit(void)  { };  /* no exit actions */

  // alternative: enforce entry actions in all states (pure virtual)
  //virtual void entry(void) = 0;
protected:
  static viewGvaManager *m_manager;
  static resolution_type m_view;
  static statusBarType m_status;
  static functionSelectType m_top;
  static commonTaskKeysType m_bottom;
  static canvasType m_canvas;
  static keyboardType m_keyboard;
  static alarmsType m_alarms;
  static screenType m_screen;
  static widgetsType m_widgets;
  static screenGva *m_render;
  static rendererMap *m_map;
  static int m_lastState;
  static bool m_alarmsOn;
public:
  static void keySide(int key);
  static void key(int key);
  static void keySA(int key);
  static void keyWPN(int key);
  static void keyDEF(int key);
  static void keySYS(int key);
  static void keyDRV(int key);
  static void keySTR(int key);
  static void keyCOM(int key);
  static void keyBMS(int key);
  static void reset();
  static xmlData xml;
  static screenGva* getRendrer() { return m_render; }
  static screenType* getScreen() { return &m_screen; }
  static widgetsType* getWidgets() { return &m_widgets; }
  static void labels(labelModeEnum labels);
};

using hmi = Hmi;
#endif
