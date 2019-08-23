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

#include <iostream>
#include "hmi_gva.h"
#include "view_gva.h"

using namespace gva;
using namespace std;

#define CANVAS { true, SURFACE_NONE, "", 0 }
#define KEYBOARD { false, KEYBOARD_UPPER }
#define ALARMS { false }
#define SCREEN { "Situational Awareness", "/dev/ttyUSB0", SA, canvas_, &top_, &status_, SYS_FUNCTION_KEYS_LEFT, SYS_FUNCTION_KEYS_RIGHT, COMMON_KEYS, COMPASS, keyboard_, alarms_ }
#define init(var, typ, data) {typ x = data; var = x; }
#define BIT(b,x) (x & 0x1 << b)
#define SET_CANVAS_PNG(file) strcpy (screen_.canvas.filename, file); screen_.canvas.bufferType = SURFACE_FILE; screen_.canvas.buffer = 0;

void
Hmi::Reset()
{
  screen_.StatusBar->visible = true;
  Labels(screen_.labels);
  screen_.canvas.visible = false;
  screen_.canvas.bufferType = SURFACE_NONE;
  widgets_.compass.visible = false;
  widgets_.compass.x = 165;
  widgets_.keyboard.visible = (widgets_.keyboard.visible) ? true : false;
  screen_.table.visible_ = false;
  screen_.control->active = 0x0;
  screen_.message.visible = false;
  alarmson_ = false;
}

void Hmi::Labels(LabelModeEnum labels) {
  switch(labels) {
  case LABEL_ALL :
    if ( (screen_.currentFunction == SA) || (screen_.currentFunction == WPN) || (screen_.currentFunction == DRV) )
      widgets_.compass.visible = true;
    screen_.functionLeft.visible = true;
    screen_.functionRight.visible = true;
    screen_.control->visible = true;
    screen_.functionTop->visible = true;
    screen_.StatusBar->visible = true;
    screen_.StatusBar->y = 447;
    widgets_.alarmIndicator.visible = true;
    widgets_.alarmIndicator.y = 426;
    break;
  case LABEL_STATUS_ONLY :
    screen_.functionLeft.visible = false;
    screen_.functionRight.visible = false;
    screen_.control->visible = false;
    screen_.functionTop->visible = false;
    screen_.StatusBar->visible = true;
    screen_.StatusBar->y = 459;
    widgets_.alarmIndicator.visible = true;
    widgets_.alarmIndicator.y = 438;
    widgets_.compass.x = 165-90;
    break;
  case LABEL_MINIMAL :
    screen_.functionLeft.visible = false;
    screen_.functionRight.visible = false;
    screen_.control->visible = false;
    screen_.functionTop->visible = false;
    screen_.StatusBar->visible = false;
    widgets_.alarmIndicator.visible = false;
    widgets_.compass.visible = false;
    widgets_.compass.x = 165;
    break;
  }
};

void
Hmi::KeySide(int key) {
  switch (key){
  case KEY_F1 :
    screen_.functionLeft.active = 1 << 5;
    break;
  case KEY_F2 :
    screen_.functionLeft.active = 1 << 4;
    break;
  case KEY_F3 :
    screen_.functionLeft.active = 1 << 3;
    break;
  case KEY_F4 :
    screen_.functionLeft.active = 1 << 2;
    break;
  case KEY_F5 :
    screen_.functionLeft.active = 1 << 1;
    break;
  case KEY_F6 :
    screen_.functionLeft.active = 1;
    break;
  case KEY_F7 :
    screen_.functionRight.active = 1 << 5;
    break;
  case KEY_F8 :
    screen_.functionRight.active = 1 << 4;
    break;
  case KEY_F9 :
    screen_.functionRight.active = 1 << 3;
    break;
  case KEY_F10 :
    screen_.functionRight.active = 1 << 2;
    break;
  case KEY_F11 :
    screen_.functionRight.active = 1 << 1;
    break;
  case KEY_F12 :
    screen_.functionRight.active = 1;
    break;
  }
}  

void
Hmi::Key(int keypress) {
  screen_.functionLeft.active = 0;
  screen_.functionRight.active = 0;
  screen_.control->active = 0;
  KeySide(keypress);
  switch (keypress){
  case KEY_F13 :
    screen_.control->active = 1 << 7;
    break;
  case KEY_F14 :
    if (screen_.currentFunction == ALARMSX) screen_.control->active = 1 << 6;
    break;
  case KEY_F15 :
    screen_.control->active = 1 << 5;
    break;
  case KEY_F16 :
    screen_.control->active = 1 << 4;
    break;
  case KEY_F17 :
    screen_.control->active = 1 << 3;
    break;
  case KEY_F18 :
    screen_.control->active = 1 << 2;
    break;
  case KEY_F19 :
    screen_.control->active = 1 << 1;
    switch (screen_.labels) {
    case LABEL_ALL : 
      screen_.labels = LABEL_STATUS_ONLY;
      break;
    case LABEL_STATUS_ONLY : 
      screen_.labels = LABEL_MINIMAL;
      break;
    case LABEL_MINIMAL : 
      screen_.labels = LABEL_ALL;
      break;
    }
    Labels(screen_.labels);
    break;
  case KEY_F20 :
    screen_.control->active = 1;
    screen_.message.visible = false;
    break;
  default:
    screen_.control->active = 0;
    break;
  }
}

void
Hmi::KeySA(int keypress) {
  screen_.functionLeft.active = 0;
  screen_.functionRight.active = 0;
  
  KeySide(keypress);
  Key(keypress);

  switch (keypress) {
  case KEY_F1:
  case KEY_F2:
  case KEY_F3:
  case KEY_F4:
  case KEY_F5:
  case KEY_F6:
  case KEY_F7:
  case KEY_F8:
  case KEY_F9:
  case KEY_F10:
  case KEY_F11:
  case KEY_F12:
    screen_.message.visible = true;
    screen_.message.icon = ICON_INFO;
    strcpy(screen_.message.brief.text, "Function key");
    strcpy(screen_.message.detail.text, "Operation not implemented!");
    break;
  }
}

void
Hmi::KeyWPN(int keypress) {
  screen_.functionLeft.active = 0;
  screen_.functionRight.active = 0;
  
  KeySide(keypress);
  Key(keypress);

  switch (keypress) {
  case KEY_F1:
  case KEY_F2:
  case KEY_F3:
  case KEY_F4:
  case KEY_F5:
  case KEY_F6:
  case KEY_F7:
  case KEY_F8:
  case KEY_F9:
  case KEY_F10:
  case KEY_F11:
  case KEY_F12:
    screen_.message.visible = true;
    screen_.message.icon = ICON_INFO;
    strcpy(screen_.message.brief.text, "Function key");
    strcpy(screen_.message.detail.text, "Operation not implemented!");
    break;
  }
}

void
Hmi::KeyDEF(int keypress) {
  screen_.functionLeft.active = 0;
  screen_.functionRight.active = 0;
  
  KeySide(keypress);
  Key(keypress);

  switch (keypress) {
  case KEY_F1:
  case KEY_F2:
  case KEY_F3:
  case KEY_F4:
  case KEY_F5:
  case KEY_F6:
  case KEY_F7:
  case KEY_F8:
  case KEY_F9:
  case KEY_F10:
  case KEY_F11:
  case KEY_F12:
    screen_.message.visible = true;
    screen_.message.icon = ICON_ERROR;
    strcpy(screen_.message.brief.text, "Function key");
    strcpy(screen_.message.detail.text, "Operation not implemented!");
    break;
  }
}

void
Hmi::KeySYS(int keypress) {
  screen_.functionLeft.active = 0;
  screen_.functionRight.active = 0;
  
  KeySide(keypress);
  Key(keypress);

  switch (keypress) {
  case KEY_F1:
  case KEY_F2:
  case KEY_F3:
  case KEY_F4:
  case KEY_F5:
  case KEY_F6:
  case KEY_F7:
  case KEY_F8:
  case KEY_F9:
  case KEY_F10:
    screen_.message.visible = true;
    screen_.message.icon = ICON_INFO;
    strcpy(screen_.message.brief.text, "Function key");
    strcpy(screen_.message.detail.text, "Operation not implemented!");
    break;
  case KEY_F11:
    // Blackout
    screen_.info.mode = (screen_.info.mode == MODE_BLACKOUT) ? MODE_MAINTINENCE : MODE_BLACKOUT;
    screen_.canvas.visible = true;
    if (screen_.info.mode == MODE_BLACKOUT) 
      screen_.canvas.bufferType = SURFACE_BLACKOUT;
    else
      screen_.canvas.bufferType = SURFACE_FILE;      
    break;
  case KEY_F12:
    // Exit
    if (RendererCairo::render_.surface)
      cairo_surface_destroy (RendererCairo::render_.surface);
    g_application_quit(G_APPLICATION (RendererCairo::render_.win.app));
    break;
  }
}

void
Hmi::KeyDRV(int keypress) {
  screen_.functionLeft.active = 0;
  screen_.functionRight.active = 0;
  
  KeySide(keypress);
  Key(keypress);

  switch (keypress) {
  case KEY_F1:
  case KEY_F2:
  case KEY_F3:
  case KEY_F4:
  case KEY_F5:
  case KEY_F6:
  case KEY_F7:
  case KEY_F8:
  case KEY_F9:
  case KEY_F10:
  case KEY_F11:
  case KEY_F12:
    screen_.message.visible = true;
    screen_.message.icon = ICON_INFO;
    strcpy(screen_.message.brief.text, "Function key");
    strcpy(screen_.message.detail.text, "Operation not implemented!");
    break;
  }
}

void
Hmi::KeySTR(int keypress) {
  screen_.functionLeft.active = 0;
  screen_.functionRight.active = 0;
  
  KeySide(keypress);
  Key(keypress);

  switch (keypress) {
  case KEY_F1:
  case KEY_F2:
  case KEY_F3:
  case KEY_F4:
  case KEY_F5:
  case KEY_F6:
  case KEY_F7:
  case KEY_F8:
  case KEY_F9:
  case KEY_F10:
  case KEY_F11:
  case KEY_F12:
    screen_.message.visible = true;
    screen_.message.icon = ICON_INFO;
    strcpy(screen_.message.brief.text, "Function key");
    strcpy(screen_.message.detail.text, "Operation not implemented!");
    break;
  }
}

void
Hmi::KeyCOM(int keypress) {
  screen_.functionLeft.active = 0;
  screen_.functionRight.active = 0;
  
  KeySide(keypress);
  Key(keypress);

  switch (keypress) {
  case KEY_F1:
  case KEY_F2:
  case KEY_F3:
  case KEY_F4:
  case KEY_F5:
  case KEY_F6:
  case KEY_F7:
  case KEY_F8:
  case KEY_F9:
  case KEY_F10:
  case KEY_F11:
  case KEY_F12:
    screen_.message.visible = true;
    screen_.message.icon = ICON_INFO;
    strcpy(screen_.message.brief.text, "Function key");
    strcpy(screen_.message.detail.text, "Operation not implemented!");
    break;
  }
}

float 
conv(int zoom) {
  switch (zoom) {
    case 1620000 : return 0.00006;
    case 640000 : return 0.00012;
    case 320000 : return 0.00025;
    case 160000 : return 0.0005;
    case 80000 : return 0.001;
    case 40000 : return 0.002;
    case 20000 : return 0.004;
    case 10000 : return 0.008;
    case 5000 : return 0.016;
    case 2500 : return 0.048;
    case 1250 : return 0.112;
    case 625: return 0.2;
  }
}

void
Hmi::KeyBMS(int key) {
  screen_.functionLeft.active = 0;
  screen_.functionRight.active = 0;
  KeySide(key);
  switch (key){
  case KEY_F3 :
    // Shift UP
    config_.SetTestLat(config_.GetZoom() + conv(config_.GetZoom()));
    break;
  case KEY_F4 :
    // Shift DOWN
    config_.SetTestLat(config_.GetZoom() - conv(config_.GetZoom()));
    break;
  case KEY_F5 :
    // Zoom +
    config_.SetZoom(config_.GetZoom() + conv(config_.GetZoom()));
    break;
  case KEY_F9 :
    // Shift LEFT
    config_.SetTestLon(config_.GetZoom() - conv(config_.GetZoom()));
    break;
  case KEY_F10 :
    // Shift RIGHT
    config_.SetTestLon(config_.GetZoom() + conv(config_.GetZoom()));
    break;
  case KEY_F11 :
    // Zoom -
    config_.SetZoom(config_.GetZoom() - conv(config_.GetZoom()) / 2);
    break;
  default:
    screen_.message.visible = true;
    screen_.message.icon = ICON_INFO;
    strcpy(screen_.message.brief.text, "Function key");
    strcpy(screen_.message.detail.text, "Operation not implemented!");
    return;
  }
printf("[BMS] Zoom level %d lat %f, %f\n", config_.GetZoom(), config_.GetTestLat(), ((double)config_.GetZoom() / 10000000) * ((double)config_.GetZoom() / 10000));
  map_->SetWidth((double)screen_render_->GetWidth() / DEFAULT_WIDTH);
  map_->SetHeight((double)screen_render_->GetHeight() / DEFAULT_HEIGHT);
printf("[BMS] res %d x %d\n", screen_render_->GetWidth(), screen_render_->GetHeight());
  map_->Project(config_.GetZoom(), config_.GetTestLon(), config_.GetTestLat(), &screen_.canvas.surface);
  screen_.canvas.bufferType = SURFACE_CAIRO;  
}

struct StateSA : Hmi
{
  void entry() override {
    if (!BIT (7, screen_.functionTop->hidden))
      {
        screen_ = manager_->GetScreen(SA);
        lastState_ = SA;
        Reset();
        screen_.functionTop->visible = true;
                
        if (screen_.labels != LABEL_MINIMAL) widgets_.compass.visible = true;
        screen_.canvas.visible = true;
        if (!screen_.canvas.surface) SET_CANVAS_PNG("test2.png");
        screen_.functionTop->active = 0x1 << 7;
      }
  };
  void react(EventKeyPowerOn const &) override { transit<StateOff>(); };
  void react(EventKeyWPN const &) override { transit<StateWPN>(); };
  void react(EventKeyDEF const &) override { transit<StateDEF>(); };
  void react(EventKeySYS const &) override { transit<StateSYS>(); };
  void react(EventKeyDRV const &) override { transit<StateDRV>(); };
  void react(EventKeySTR const &) override { transit<StateSTR>(); };
  void react(EventKeyCOM const &) override { transit<StateCOM>(); };
  void react(EventKeyBMS const &) override { transit<StateBMS>(); };
  void react(EventKeyAlarms const &) override { transit<StateAlarms>(); };
  void react(EventKeyFunction const &e) { KeySA(e.key); };
};

struct StateWPN : Hmi
{
  void entry() override {
    if (!BIT (6, screen_.functionTop->hidden))
      {
        screen_ = manager_->GetScreen(WPN);
        lastState_ = WPN;
        Reset();

        if (screen_.labels != LABEL_MINIMAL) widgets_.compass.visible = true;
        screen_.canvas.visible = true;
        SET_CANVAS_PNG("test2.png");
        screen_.functionTop->active = 0x1 << 6;
      }
  };
  void react(EventKeyPowerOn const &) override { transit<StateOff>(); };
  void react(EventKeySA const &) override { transit<StateSA>(); };
  void react(EventKeyDEF const &) override { transit<StateDEF>(); };
  void react(EventKeySYS const &) override { transit<StateSYS>(); };
  void react(EventKeyDRV const &) override { transit<StateDRV>(); };
  void react(EventKeySTR const &) override { transit<StateSTR>(); };
  void react(EventKeyCOM const &) override { transit<StateCOM>(); };
  void react(EventKeyBMS const &) override { transit<StateBMS>(); };
  void react(EventKeyAlarms const &) override { transit<StateAlarms>(); };
  void react(EventKeyFunction const &e) { KeyWPN(e.key); };
};

struct StateDEF : Hmi
{
  void entry() override {
    if (!BIT (5, screen_.functionTop->hidden))
      {
        screen_ = manager_->GetScreen(DEF);
        lastState_ = DEF;
        Reset();

        screen_.StatusBar->visible = true;
        screen_.functionTop->active = 0x1 << 5;
      }
  };
  void react(EventKeyPowerOn const &) override { transit<StateOff>(); };
  void react(EventKeySA const &) override { transit<StateSA>(); };
  void react(EventKeyWPN const &) override { transit<StateWPN>(); };
  void react(EventKeySYS const &) override { transit<StateSYS>(); };
  void react(EventKeyDRV const &) override { transit<StateDRV>(); };
  void react(EventKeySTR const &) override { transit<StateSTR>(); };
  void react(EventKeyCOM const &) override { transit<StateCOM>(); };
  void react(EventKeyBMS const &) override { transit<StateBMS>(); };
  void react(EventKeyAlarms const &) override { transit<StateAlarms>(); };
  void react(EventKeyFunction const &e) { KeyDEF(e.key); };
};
                
struct StateSYS : Hmi
{
  void entry() override {
    if (!BIT (4, screen_.functionTop->hidden))
      {
        screen_ = manager_->GetScreen(SYS);
        lastState_ = SYS;
        Reset();
        
        screen_.StatusBar->visible = true;
        screen_.functionTop->visible = true;
        screen_.canvas.visible = true;
        SET_CANVAS_PNG("test2.png");

		screen_.table.visible_ = true;
		screen_.table.x_ = 110;
		screen_.table.y_ = 390;
		screen_.table.width_ = 420;

		screen_.table.AddRow(WEIGHT_BOLD);
		screen_.table.AddCell("Fuction", 80);
		screen_.table.AddCell("Status", 20);

		screen_.table.AddRow();
		char tmp[100];
		sprintf(tmp, "HMI Version %d.%d.%d", MAJOR, MINOR, PATCH);
		screen_.table.AddCell(tmp, 80);
		screen_.table.AddCell("Ok", 20);

		screen_.table.AddRow();
		screen_.table.AddCell("GPS Source, /dev/ttyUSB0", 80);
		screen_.table.AddCell("Error", 20, renderer::PackRgb(RED));

		screen_.table.AddRow();
		screen_.table.AddCell("ODB Source, /dev/ttyUSB1", 80);
		screen_.table.AddCell("Error", 20, renderer::PackRgb(RED));

		screen_.table.AddRow();
		screen_.table.AddCell("UK Maps, /opt/osmscout/maps/england", 80);
		screen_.table.AddCell("Ok", 20);

		screen_.table.AddRow();
		screen_.table.AddCell("Timesource GPS Lock", 80);
		screen_.table.AddCell("Off", 20, renderer::PackRgb(ORANGE));

        screen_.functionTop->active = 0x1 << 4;
      }
  };
  void react(EventKeyPowerOn const &) override { transit<StateOff>(); };
  void react(EventKeySA const &) override { transit<StateSA>(); };
  void react(EventKeyWPN const &) override { transit<StateWPN>(); };
  void react(EventKeyDEF const &) override { transit<StateDEF>(); };
  void react(EventKeyDRV const &) override { transit<StateDRV>(); };
  void react(EventKeySTR const &) override { transit<StateSTR>(); };
  void react(EventKeyCOM const &) override { transit<StateCOM>(); };
  void react(EventKeyBMS const &) override { transit<StateBMS>(); };
  void react(EventKeyAlarms const &) override { transit<StateAlarms>(); };
  void react(EventKeyFunction const &e) { KeySYS(e.key); };
};

struct StateDRV : Hmi
{
  void entry() override {
    if (!BIT (3, screen_.functionTop->hidden))
      {
        screen_ = manager_->GetScreen(DRV);
        lastState_ = DRV;
        Reset();

        if (screen_.labels != LABEL_MINIMAL) widgets_.compass.visible = true;
        screen_.StatusBar->visible = true;
        screen_.functionTop->active = 0x1 << 3;
      }
  };
  void react(EventKeyPowerOn const &) override { transit<StateOff>(); };
  void react(EventKeySA const &) override { transit<StateSA>(); };
  void react(EventKeyWPN const &) override { transit<StateWPN>(); };
  void react(EventKeyDEF const &) override { transit<StateDEF>(); };
  void react(EventKeySYS const &) override { transit<StateSYS>(); };
  void react(EventKeySTR const &) override { transit<StateSTR>(); };
  void react(EventKeyCOM const &) override { transit<StateCOM>(); };
  void react(EventKeyBMS const &) override { transit<StateBMS>(); };
  void react(EventKeyAlarms const &) override { transit<StateAlarms>(); };
  void react(EventKeyFunction const &e) { KeyDRV(e.key); };
};

struct StateSTR : Hmi
{
  void entry() override {
    if (!BIT (2, screen_.functionTop->hidden))
      {
        screen_ = manager_->GetScreen(STR);
        lastState_ = STR;
        Reset();

        screen_.functionTop->active = 0x1 << 2;
      }
  };
  void react(EventKeyPowerOn const &) override { transit<StateOff>(); };
  void react(EventKeySA const &) override { transit<StateSA>(); };
  void react(EventKeyWPN const &) override { transit<StateWPN>(); };
  void react(EventKeyDEF const &) override { transit<StateDEF>(); };
  void react(EventKeySYS const &) override { transit<StateSYS>(); };
  void react(EventKeyDRV const &) override { transit<StateDRV>(); };
  void react(EventKeyCOM const &) override { transit<StateCOM>(); };
  void react(EventKeyBMS const &) override { transit<StateBMS>(); };
  void react(EventKeyAlarms const &) override { transit<StateAlarms>(); };
  void react(EventKeyFunction const &e) { KeySTR(e.key); };
};

struct StateCOM : Hmi
{
  void entry() override {
    if (!BIT (1, screen_.functionTop->hidden))
      {
        screen_ = manager_->GetScreen(COM);
        lastState_ = COM;
        Reset();

        screen_.functionTop->active = 0x1 << 1;
      }
  };
  void react(EventKeyPowerOn const &) override { transit<StateOff>(); };
  void react(EventKeySA const &) override { transit<StateSA>(); };
  void react(EventKeyWPN const &) override { transit<StateWPN>(); };
  void react(EventKeyDEF const &) override { transit<StateDEF>(); };
  void react(EventKeySYS const &) override { transit<StateSYS>(); };
  void react(EventKeyDRV const &) override { transit<StateDRV>(); };
  void react(EventKeySTR const &) override { transit<StateSTR>(); };
  void react(EventKeyBMS const &) override { transit<StateBMS>(); };
  void react(EventKeyAlarms const &) override { transit<StateAlarms>(); };
  void react(EventKeyFunction const &e) { KeyCOM(e.key); };
};

struct StateBMS : Hmi
{
  void entry() override {
    if (!BIT (0, screen_.functionTop->hidden))
      {
        screen_ = manager_->GetScreen(BMS);
        lastState_ = BMS;
        Reset();

        screen_.canvas.visible = true;

        map_->SetWidth((double)screen_render_->GetWidth() / DEFAULT_WIDTH);
        map_->SetHeight((double)screen_render_->GetHeight() / DEFAULT_HEIGHT);
        map_->Project(config_.GetZoom(), config_.GetTestLon(), config_.GetTestLat(), &screen_.canvas.surface);
        screen_.canvas.bufferType = SURFACE_CAIRO;

        screen_.functionTop->active = 0x1 << 0;
      }
  };
  void react(EventKeyPowerOn const &) override { transit<StateOff>(); };
  void react(EventKeySA const &) override { transit<StateSA>(); };
  void react(EventKeyWPN const &) override { transit<StateWPN>(); };
  void react(EventKeyDEF const &) override { transit<StateDEF>(); };
  void react(EventKeySYS const &) override { transit<StateSYS>(); };
  void react(EventKeyDRV const &) override { transit<StateDRV>(); };
  void react(EventKeySTR const &) override { transit<StateSTR>(); };
  void react(EventKeyCOM const &) override { transit<StateCOM>(); };
  void react(EventKeyAlarms const &) override { transit<StateAlarms>(); };
  void react(EventKeyFunction const &e) { KeyBMS(e.key); };
};

struct StateAlarms : Hmi
{
  void entry() override {
    if (!BIT (1, screen_.control->hidden)) {
      if (alarmson_) { 
        alarmson_ = false;
        switch (lastState_) {
        case SA:
          transit<StateSA>(); 
          return; 
        case WPN:
          transit<StateWPN>(); 
          return; 
        case DEF:
          transit<StateDEF>(); 
          return; 
        case SYS:
          transit<StateSYS>(); 
          return; 
        case DRV:
          transit<StateDRV>(); 
          return; 
        case STR:
          transit<StateSTR>(); 
          return; 
        case COM:
          transit<StateCOM>(); 
          return; 
        case BMS:
          transit<StateBMS>(); 
          return; 
        }
      }
      Reset();
      alarmson_ = true;
      screen_ = manager_->GetScreen(ALARMSX);

      screen_.table.visible_ = true;
      screen_.table.x_ = 110;
      screen_.table.y_ = 390;
      screen_.table.width_ = 420;

      screen_.table.AddRow(WEIGHT_BOLD);
      screen_.table.AddCell("Time", 20);
      screen_.table.AddCell("Alarm Text", 50);
      screen_.table.AddCell("Cat", 10);
      screen_.table.AddCell("Status", 20);
      
      screen_.table.AddRow();
      screen_.table.AddCell("15/6 15:06", 20);
      screen_.table.AddCell("Low engine oil pressure", 50);
      screen_.table.AddCell("W", 10);
      screen_.table.AddCell("RES", 20);
      
      screen_.table.AddRow(screen_render_->PackRgb(WHITE), 
                     screen_render_->PackRgb(RED), 
                     screen_render_->PackRgb(WHITE), 
                     screen_render_->PackRgb(YELLOW), WEIGHT_NORMAL);
      screen_.table.AddCell("15/6 15:26", 20);
      screen_.table.AddCell("Engine over tempreture", 50);
      screen_.table.AddCell("W", 10);
      screen_.table.AddCell("UNACK", 20);
      
      screen_.table.AddRow();
      screen_.table.CurrentRowHighlight();
      screen_.table.AddCell("15/6 15:29", 20);
      screen_.table.AddCell("Engine over tempreture", 50);
      screen_.table.AddCell("W", 10);
      screen_.table.AddCell("RES", 20);
      
      screen_.table.AddRow();
      screen_.table.AddCell("15/6 14:00", 20);
      screen_.table.AddCell("Gun fault", 50);
      screen_.table.AddCell("C", 10);
      screen_.table.AddCell("RES", 20);
      
      screen_.table.AddRow();
      screen_.table.AddCell("15/6 18:16", 20);
      screen_.table.AddCell("Air con fault", 50);
      screen_.table.AddCell("A", 10);
      screen_.table.AddCell("ACT", 20);
      
      screen_.table.AddRow(screen_render_->PackRgb(WHITE), 
                            screen_render_->PackRgb(GREY), 
                            screen_render_->PackRgb(WHITE), 
                            screen_render_->PackRgb(YELLOW), WEIGHT_NORMAL);
      screen_.table.AddCell("15/6 19:03", 20);
      screen_.table.AddCell("Gun barrel over tempreture", 50);
      screen_.table.AddCell("C", 10);
      screen_.table.AddCell("ACT(OVR)", 20);
      
      screen_.table.AddRow(screen_render_->PackRgb(WHITE), 
                            screen_render_->PackRgb(ORANGE), 
                            screen_render_->PackRgb(WHITE), 
                            screen_render_->PackRgb(YELLOW), WEIGHT_NORMAL);
      screen_.table.AddCell("15/6 19:", 20);
      screen_.table.AddCell("LRU xx fault", 50);
      screen_.table.AddCell("C", 10);
      screen_.table.AddCell("ACT", 20);

      screen_.table.AddRow();
      screen_.table.AddCell("Page 1 of 1", 100, ALIGN_RIGHT);
    }
  };
  void react(EventKeySA const &) override { transit<StateSA>(); };
  void react(EventKeyWPN const &) override { transit<StateWPN>(); };
  void react(EventKeyDEF const &) override { transit<StateDEF>(); };
  void react(EventKeySYS const &) override { transit<StateSYS>(); };
  void react(EventKeyDRV const &) override { transit<StateDRV>(); };
  void react(EventKeySTR const &) override { transit<StateSTR>(); };
  void react(EventKeyCOM const &) override { transit<StateCOM>(); };
  void react(EventKeyBMS const &) override { transit<StateBMS>(); };
  void react(EventKeyAlarms const &) override { transit<StateAlarms>(); };
  void react(EventKeyFunction const &e) { Key(e.key); };
};

struct StateOn : Hmi
{
  void entry() override {
    /* 4:3 aspect ratio @ lowest resolution */
    view_ = { MIN_WIDTH, MIN_HEIGHT, 24 };
    
    if(!manager_)
      manager_ = new ViewGvaManager(&status_);
    
    // Render a map for BMS
    map_ = new rendererMap("/opt/osmscout/maps/england-latest/", 
      "/opt/osmscout/stylesheets/standard.oss", MIN_WIDTH, MIN_HEIGHT);
    
    init(top_, FunctionSelectType, COMMON_FUNCTION_KEYS_TOP);
    init(bottom_, CommonTaskKeysType, COMMON_KEYS);
    init(status_, StatusBarType, COMMON_STATUS_BAR);
    init(canvas_, CanvasType, CANVAS);
    init(keyboard_, KeyboardType, KEYBOARD);
    init(alarms_, TableWidget, ALARMS);

    // Setup the main screens
    manager_->GetNewView(SA,     &top_, &bottom_, (FunctionKeys)SA_FUNCTION_KEYS_LEFT,  (FunctionKeys)SA_FUNCTION_KEYS_RIGHT);
    manager_->GetNewView(WPN,    &top_, &bottom_, (FunctionKeys)WPN_FUNCTION_KEYS_LEFT, (FunctionKeys)WPN_FUNCTION_KEYS_RIGHT);
    manager_->GetNewView(DEF,    &top_, &bottom_, (FunctionKeys)DEF_FUNCTION_KEYS_LEFT, (FunctionKeys)DEF_FUNCTION_KEYS_RIGHT);
    manager_->GetNewView(SYS,    &top_, &bottom_, (FunctionKeys)SYS_FUNCTION_KEYS_LEFT, (FunctionKeys)SYS_FUNCTION_KEYS_RIGHT);
    manager_->GetNewView(DRV,    &top_, &bottom_, (FunctionKeys)DRV_FUNCTION_KEYS_LEFT, (FunctionKeys)DRV_FUNCTION_KEYS_RIGHT);
    manager_->GetNewView(STR,    &top_, &bottom_, (FunctionKeys)STR_FUNCTION_KEYS_LEFT, (FunctionKeys)STR_FUNCTION_KEYS_RIGHT);
    manager_->GetNewView(COM,    &top_, &bottom_, (FunctionKeys)COM_FUNCTION_KEYS_LEFT, (FunctionKeys)COM_FUNCTION_KEYS_RIGHT);
    manager_->GetNewView(BMS,    &top_, &bottom_, (FunctionKeys)BMS_FUNCTION_KEYS_LEFT, (FunctionKeys)BMS_FUNCTION_KEYS_RIGHT);
    manager_->GetNewView(ALARMSX, &top_, &bottom_, (FunctionKeys)ALARM_KEYS_LEFT,        (FunctionKeys)ALARM_KEYS_RIGHT);

    screen_ = manager_->GetScreen(SYS);
    widgets_.compass.bearingSight = 33;
    widgets_.compass.x = 165; 
    widgets_.compass.y = 370;
    widgets_.compass.visible = true;
    widgets_.alarmIndicator.visible = true;
    widgets_.alarmIndicator.y = 422;
    strcpy(widgets_.alarmIndicator.text,"Engine over tempreture");

    screen_.canvas = canvas_;
    screen_.canvas.visible = true;
    screen_.table = alarms_;
    screen_.table.visible_ = false;
    screen_.labels = LABEL_ALL;
    SET_CANVAS_PNG("test2.png");
    
    /* These are comon to all screens */
    screen_render_ = new ScreenGva (&screen_, &widgets_, view_.width, view_.height);

    transit<StateSYS>(); 
  };
};

struct StateOff : Hmi
{
  void entry() override { 
    free(screen_render_);
    free(manager_);
    screen_render_ = 0;
    manager_ = 0;
  };
  void react(EventKeyPowerOn const &) override { transit<StateOn>(); };
};

ViewGvaManager *Hmi::manager_;
ResolutionType Hmi::view_;
StatusBarType Hmi::status_;
FunctionSelectType Hmi::top_;
CommonTaskKeysType Hmi::bottom_;
CanvasType Hmi::canvas_;
KeyboardType Hmi::keyboard_;
TableWidget Hmi::alarms_;
ScreenType Hmi::screen_;
WidgetsType Hmi::widgets_;
ScreenGva* Hmi::screen_render_;
rendererMap* Hmi::map_;
int  Hmi::lastState_;
bool Hmi::alarmson_ = false;
ConfigData Hmi::config_;

// ----------------------------------------------------------------------------
// Initial state definition
//
FSM_INITIAL_STATE(Hmi, StateOff)
