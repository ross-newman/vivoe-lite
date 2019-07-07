#include <iostream>
#include "hmi_gva.h"
#include "view_gva.h"

using namespace gva;
using namespace std;

#define CANVAS { true, SURFACE_NONE, "", 0 }
#define KEYBOARD { false, KEYBOARD_UPPER }
#define ALARMS { false }
#define SCREEN { "Situational Awareness", "/dev/ttyUSB0", SA, m_canvas, &m_top, &m_status, SYS_FUNCTION_KEYS_LEFT, SYS_FUNCTION_KEYS_RIGHT, COMMON_KEYS, COMPASS, m_keyboard, m_alarms }
#define init(var, typ, data) {typ x = data; var = x; }
#define BIT(b,x) (x & 0x1 << b)
#define SET_CANVAS_PNG(file) strcpy (m_screen.canvas.filename, file); m_screen.canvas.bufferType = SURFACE_FILE; m_screen.canvas.buffer = 0;

void
Hmi::reset()
{
  m_screen.canvas.visible = false;
  m_screen.statusBar->visible = true;
  labels(m_screen.labels);
  m_screen.canvas.visible = false;
  m_screen.canvas.bufferType = SURFACE_NONE;
  m_widgets.compass.visible = false;
  m_widgets.compass.x = 165;
  m_widgets.keyboard.visible = (m_widgets.keyboard.visible) ? true : false;
  m_screen.alarms.visible = false;
  m_screen.control->active = 0;
  m_alarmsOn = false;
}

void 
Hmi::labels(labelModeEnum labels) {
  switch(labels) {
  case LABEL_ALL :
    if ( (m_screen.currentFunction == SA) || (m_screen.currentFunction == WPN) || (m_screen.currentFunction == SYS) || (m_screen.currentFunction == DRV) )
      m_widgets.compass.visible = true;
    m_screen.functionLeft.visible = true;
    m_screen.functionRight.visible = true;
    m_screen.control->visible = true;
    m_screen.functionTop->visible = true;
    m_screen.statusBar->visible = true;
    m_screen.statusBar->y = 443;
    m_widgets.alarmIndicator.visible = true;
    m_widgets.alarmIndicator.y = 422;
    break;
  case LABEL_STATUS_ONLY :
    m_screen.functionLeft.visible = false;
    m_screen.functionRight.visible = false;
    m_screen.control->visible = false;
    m_screen.functionTop->visible = false;
    m_screen.statusBar->visible = true;
    m_screen.statusBar->y = 459;
    m_widgets.alarmIndicator.visible = true;
    m_widgets.alarmIndicator.y = 438;
    m_widgets.compass.x = 165-90;
    break;
  case LABEL_MINIMAL :
    m_screen.functionLeft.visible = false;
    m_screen.functionRight.visible = false;
    m_screen.control->visible = false;
    m_screen.functionTop->visible = false;
    m_screen.statusBar->visible = false;
    m_widgets.alarmIndicator.visible = false;
    m_widgets.compass.visible = false;
    m_widgets.compass.x = 165;
    break;
  }
};

void
Hmi::keySide(int key) {
  switch (key){
  case KEY_F1 :
    m_screen.functionLeft.active = 1 << 5;
    break;
  case KEY_F2 :
    m_screen.functionLeft.active = 1 << 4;
    break;
  case KEY_F3 :
    m_screen.functionLeft.active = 1 << 3;
    break;
  case KEY_F4 :
    m_screen.functionLeft.active = 1 << 2;
    break;
  case KEY_F5 :
    m_screen.functionLeft.active = 1 << 1;
    break;
  case KEY_F6 :
    m_screen.functionLeft.active = 1;
    break;
  case KEY_F7 :
    m_screen.functionRight.active = 1 << 5;
    break;
  case KEY_F8 :
    m_screen.functionRight.active = 1 << 4;
    break;
  case KEY_F9 :
    m_screen.functionRight.active = 1 << 3;
    break;
  case KEY_F10 :
    m_screen.functionRight.active = 1 << 2;
    break;
  case KEY_F11 :
    m_screen.functionRight.active = 1 << 1;
    break;
  case KEY_F12 :
    m_screen.functionRight.active = 1;
    break;
  }
}  

void
Hmi::key(int keypress) {
  m_screen.functionLeft.active = 0;
  m_screen.functionRight.active = 0;
  m_screen.control->active = 0;
  keySide(keypress);
  switch (keypress){
  case KEY_F13 :
    m_screen.control->active = 1 << 7;
    break;
  case KEY_F14 :
    m_screen.control->active = 1 << 6;
    break;
  case KEY_F15 :
    m_screen.control->active = 1 << 5;
    break;
  case KEY_F16 :
    m_screen.control->active = 1 << 4;
    break;
  case KEY_F17 :
    m_screen.control->active = 1 << 3;
    break;
  case KEY_F18 :
    m_screen.control->active = 1 << 2;
    break;
  case KEY_F19 :
    m_screen.control->active = 1 << 1;
    switch (m_screen.labels) {
    case LABEL_ALL : 
      m_screen.labels = LABEL_STATUS_ONLY;
      break;
    case LABEL_STATUS_ONLY : 
      m_screen.labels = LABEL_MINIMAL;
      break;
    case LABEL_MINIMAL : 
      m_screen.labels = LABEL_ALL;
      break;
    }
    labels(m_screen.labels);
    break;
  case KEY_F20 :
    m_screen.control->active = 1;
    m_screen.message.visible = false;
    break;
  }
}

void
Hmi::keySA(int keypress) {
  m_screen.functionLeft.active = 0;
  m_screen.functionRight.active = 0;
  
  keySide(keypress);
  key(keypress);

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
    m_screen.message.visible = true;
    m_screen.message.icon = ICON_NONE;
    strcpy(m_screen.message.brief.text, "Function key");
    strcpy(m_screen.message.detail.text, "Operation not implemented!");
    break;
  }
}


void
Hmi::keyWPN(int keypress) {
  m_screen.functionLeft.active = 0;
  m_screen.functionRight.active = 0;
  
  keySide(keypress);
  key(keypress);

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
    m_screen.message.visible = true;
    m_screen.message.icon = ICON_NONE;
    strcpy(m_screen.message.brief.text, "Function key");
    strcpy(m_screen.message.detail.text, "Operation not implemented!");
    break;
  }
}


void
Hmi::keyDEF(int keypress) {
  m_screen.functionLeft.active = 0;
  m_screen.functionRight.active = 0;
  
  keySide(keypress);
  key(keypress);

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
    m_screen.message.visible = true;
    m_screen.message.icon = ICON_NONE;
    strcpy(m_screen.message.brief.text, "Function key");
    strcpy(m_screen.message.detail.text, "Operation not implemented!");
    break;
  }
}

void
Hmi::keySYS(int keypress) {
  m_screen.functionLeft.active = 0;
  m_screen.functionRight.active = 0;
  
  keySide(keypress);
  key(keypress);

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
    m_screen.message.visible = true;
    m_screen.message.icon = ICON_NONE;
    strcpy(m_screen.message.brief.text, "Function key");
    strcpy(m_screen.message.detail.text, "Operation not implemented!");
    break;
  }
}

void
Hmi::keyDRV(int keypress) {
  m_screen.functionLeft.active = 0;
  m_screen.functionRight.active = 0;
  
  keySide(keypress);
  key(keypress);

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
    m_screen.message.visible = true;
    m_screen.message.icon = ICON_NONE;
    strcpy(m_screen.message.brief.text, "Function key");
    strcpy(m_screen.message.detail.text, "Operation not implemented!");
    break;
  }
}

void
Hmi::keySTR(int keypress) {
  m_screen.functionLeft.active = 0;
  m_screen.functionRight.active = 0;
  
  keySide(keypress);
  key(keypress);

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
    m_screen.message.visible = true;
    m_screen.message.icon = ICON_NONE;
    strcpy(m_screen.message.brief.text, "Function key");
    strcpy(m_screen.message.detail.text, "Operation not implemented!");
    break;
  }
}

void
Hmi::keyCOM(int keypress) {
  m_screen.functionLeft.active = 0;
  m_screen.functionRight.active = 0;
  
  keySide(keypress);
  key(keypress);

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
    m_screen.message.visible = true;
    m_screen.message.icon = ICON_NONE;
    strcpy(m_screen.message.brief.text, "Function key");
    strcpy(m_screen.message.detail.text, "Operation not implemented!");
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
Hmi::keyBMS(int key) {
  m_screen.functionLeft.active = 0;
  m_screen.functionRight.active = 0;
  keySide(key);
  switch (key){
  case KEY_F3 :
    // Shift UP
    xml.lat += conv(xml.zoom);
    break;
  case KEY_F4 :
    // Shift DOWN
    xml.lat -= conv(xml.zoom);
    break;
  case KEY_F5 :
    // Zoom +
    xml.zoom += xml.zoom;
    break;
  case KEY_F9 :
    // Shift LEFT
    xml.lon -= conv(xml.zoom);
    break;
  case KEY_F10 :
    // Shift RIGHT
    xml.lon += conv(xml.zoom);
    break;
  case KEY_F11 :
    // Zoom -
    xml.zoom -= xml.zoom / 2;
    break;
  default:
    m_screen.message.visible = true;
    m_screen.message.icon = ICON_NONE;
    strcpy(m_screen.message.brief.text, "Function key");
    strcpy(m_screen.message.detail.text, "Operation not implemented!");
    break;
  }
printf("Zoom level %d lat %f, %f\n", xml.zoom, xml.lat, ((double)xml.zoom / 10000000) * ((double)xml.zoom / 10000));
//  cairo_surface_destroy(m_screen.canvas.surface);
  m_map->project(xml.zoom, xml.lon, xml.lat, &m_screen.canvas.surface);
  m_screen.canvas.bufferType = SURFACE_CAIRO;  
}

struct stateSA : Hmi
{
  void entry() override {
    if (!BIT (7, m_screen.functionTop->hidden))
      {
        m_screen = m_manager->getScreen(SA);
        m_lastState = SA;
        reset();
        m_screen.functionTop->visible = true;
                
        if (m_screen.labels != LABEL_MINIMAL) m_widgets.compass.visible = true;
        m_screen.canvas.visible = true;
        if (!m_screen.canvas.surface) SET_CANVAS_PNG("test2.png");
        m_screen.functionTop->active = 0x1 << 7;
      }
  };
  void react(KeyPowerOn const &) override { transit<stateOff>(); };
  void react(KeyWPN const &) override { transit<stateWPN>(); };
  void react(KeyDEF const &) override { transit<stateDEF>(); };
  void react(KeySYS const &) override { transit<stateSYS>(); };
  void react(KeyDRV const &) override { transit<stateDRV>(); };
  void react(KeySTR const &) override { transit<stateSTR>(); };
  void react(KeyCOM const &) override { transit<stateCOM>(); };
  void react(KeyBMS const &) override { transit<stateBMS>(); };
  void react(KeyAlarms const &) override { transit<stateAlarms>(); };
  void react(KeyFunction const &e) { keySA(e.key); };
};

struct stateWPN : Hmi
{
  void entry() override {
    if (!BIT (6, m_screen.functionTop->hidden))
      {
        m_screen = m_manager->getScreen(WPN);
        m_lastState = WPN;
        reset();

        if (m_screen.labels != LABEL_MINIMAL) m_widgets.compass.visible = true;
        m_screen.canvas.visible = true;
        SET_CANVAS_PNG("test2.png");
        m_screen.functionTop->active = 0x1 << 6;
      }
  };
  void react(KeyPowerOn const &) override { transit<stateOff>(); };
  void react(KeySA const &) override { transit<stateSA>(); };
  void react(KeyDEF const &) override { transit<stateDEF>(); };
  void react(KeySYS const &) override { transit<stateSYS>(); };
  void react(KeyDRV const &) override { transit<stateDRV>(); };
  void react(KeySTR const &) override { transit<stateSTR>(); };
  void react(KeyCOM const &) override { transit<stateCOM>(); };
  void react(KeyBMS const &) override { transit<stateBMS>(); };
  void react(KeyAlarms const &) override { transit<stateAlarms>(); };
  void react(KeyFunction const &e) { keyWPN(e.key); };
};

struct stateDEF : Hmi
{
  void entry() override {
    if (!BIT (5, m_screen.functionTop->hidden))
      {
        m_screen = m_manager->getScreen(DEF);
        m_lastState = DEF;
        reset();

        m_screen.statusBar->visible = true;
        m_screen.functionTop->active = 0x1 << 5;
      }
  };
  void react(KeyPowerOn const &) override { transit<stateOff>(); };
  void react(KeySA const &) override { transit<stateSA>(); };
  void react(KeyWPN const &) override { transit<stateWPN>(); };
  void react(KeySYS const &) override { transit<stateSYS>(); };
  void react(KeyDRV const &) override { transit<stateDRV>(); };
  void react(KeySTR const &) override { transit<stateSTR>(); };
  void react(KeyCOM const &) override { transit<stateCOM>(); };
  void react(KeyBMS const &) override { transit<stateBMS>(); };
  void react(KeyAlarms const &) override { transit<stateAlarms>(); };
  void react(KeyFunction const &e) { keyDEF(e.key); };
};
                
struct stateSYS : Hmi
{
  void entry() override {
    if (!BIT (4, m_screen.functionTop->hidden))
      {
        m_screen = m_manager->getScreen(SYS);
        m_lastState = SYS;
        reset();

        if (m_screen.labels != LABEL_MINIMAL) m_widgets.compass.visible = true;
        m_screen.canvas.visible = true;
        SET_CANVAS_PNG("test2.png");

        m_screen.functionTop->active = 0x1 << 4;
      }
  };
  void react(KeyPowerOn const &) override { transit<stateOff>(); };
  void react(KeySA const &) override { transit<stateSA>(); };
  void react(KeyWPN const &) override { transit<stateWPN>(); };
  void react(KeyDEF const &) override { transit<stateDEF>(); };
  void react(KeyDRV const &) override { transit<stateDRV>(); };
  void react(KeySTR const &) override { transit<stateSTR>(); };
  void react(KeyCOM const &) override { transit<stateCOM>(); };
  void react(KeyBMS const &) override { transit<stateBMS>(); };
  void react(KeyAlarms const &) override { transit<stateAlarms>(); };
  void react(KeyFunction const &e) { keySYS(e.key); };
};

struct stateDRV : Hmi
{
  void entry() override {
    if (!BIT (3, m_screen.functionTop->hidden))
      {
        m_screen = m_manager->getScreen(DRV);
        m_lastState = DRV;
        reset();

        if (m_screen.labels != LABEL_MINIMAL) m_widgets.compass.visible = true;
        m_screen.statusBar->visible = true;
        m_screen.functionTop->active = 0x1 << 3;
      }
  };
  void react(KeyPowerOn const &) override { transit<stateOff>(); };
  void react(KeySA const &) override { transit<stateSA>(); };
  void react(KeyWPN const &) override { transit<stateWPN>(); };
  void react(KeyDEF const &) override { transit<stateDEF>(); };
  void react(KeySYS const &) override { transit<stateSYS>(); };
  void react(KeySTR const &) override { transit<stateSTR>(); };
  void react(KeyCOM const &) override { transit<stateCOM>(); };
  void react(KeyBMS const &) override { transit<stateBMS>(); };
  void react(KeyAlarms const &) override { transit<stateAlarms>(); };
  void react(KeyFunction const &e) { keyDRV(e.key); };
};

struct stateSTR : Hmi
{
  void entry() override {
    if (!BIT (2, m_screen.functionTop->hidden))
      {
        m_screen = m_manager->getScreen(STR);
        m_lastState = STR;
        reset();

        m_screen.functionTop->active = 0x1 << 2;
      }
  };
  void react(KeyPowerOn const &) override { transit<stateOff>(); };
  void react(KeySA const &) override { transit<stateSA>(); };
  void react(KeyWPN const &) override { transit<stateWPN>(); };
  void react(KeyDEF const &) override { transit<stateDEF>(); };
  void react(KeySYS const &) override { transit<stateSYS>(); };
  void react(KeyDRV const &) override { transit<stateDRV>(); };
  void react(KeyCOM const &) override { transit<stateCOM>(); };
  void react(KeyBMS const &) override { transit<stateBMS>(); };
  void react(KeyAlarms const &) override { transit<stateAlarms>(); };
  void react(KeyFunction const &e) { keySTR(e.key); };
};

struct stateCOM : Hmi
{
  void entry() override {
    if (!BIT (1, m_screen.functionTop->hidden))
      {
        m_screen = m_manager->getScreen(COM);
        m_lastState = COM;
        reset();

        m_screen.functionTop->active = 0x1 << 1;
      }
  };
  void react(KeyPowerOn const &) override { transit<stateOff>(); };
  void react(KeySA const &) override { transit<stateSA>(); };
  void react(KeyWPN const &) override { transit<stateWPN>(); };
  void react(KeyDEF const &) override { transit<stateDEF>(); };
  void react(KeySYS const &) override { transit<stateSYS>(); };
  void react(KeyDRV const &) override { transit<stateDRV>(); };
  void react(KeySTR const &) override { transit<stateSTR>(); };
  void react(KeyBMS const &) override { transit<stateBMS>(); };
  void react(KeyAlarms const &) override { transit<stateAlarms>(); };
  void react(KeyFunction const &e) { keyCOM(e.key); };
};

struct stateBMS : Hmi
{
  void entry() override {
    if (!BIT (0, m_screen.functionTop->hidden))
      {
        m_screen = m_manager->getScreen(BMS);
        m_lastState = BMS;
        reset();

        m_screen.canvas.visible = true;

        m_map->project(xml.zoom, xml.lon, xml.lat, &m_screen.canvas.surface);
        m_screen.canvas.bufferType = SURFACE_CAIRO;

        m_screen.functionTop->active = 0x1 << 0;
      }
  };
  void react(KeyPowerOn const &) override { transit<stateOff>(); };
  void react(KeySA const &) override { transit<stateSA>(); };
  void react(KeyWPN const &) override { transit<stateWPN>(); };
  void react(KeyDEF const &) override { transit<stateDEF>(); };
  void react(KeySYS const &) override { transit<stateSYS>(); };
  void react(KeyDRV const &) override { transit<stateDRV>(); };
  void react(KeySTR const &) override { transit<stateSTR>(); };
  void react(KeyCOM const &) override { transit<stateCOM>(); };
  void react(KeyAlarms const &) override { transit<stateAlarms>(); };
  void react(KeyFunction const &e) { keyBMS(e.key); };
};

struct stateAlarms : Hmi
{
  void entry() override {
    if (!BIT (1, m_screen.control->hidden)) {
      if (m_alarmsOn) { 
        m_alarmsOn = false;
        switch (m_lastState) {
        case SA:
          transit<stateSA>(); 
          return; 
        case WPN:
          transit<stateWPN>(); 
          return; 
        case DEF:
          transit<stateDEF>(); 
          return; 
        case SYS:
          transit<stateSYS>(); 
          return; 
        case DRV:
          transit<stateDRV>(); 
          return; 
        case STR:
          transit<stateSTR>(); 
          return; 
        case COM:
          transit<stateCOM>(); 
          return; 
        case BMS:
          transit<stateBMS>(); 
          return; 
        }
      }
      reset();
      m_alarmsOn = true;
      m_screen = m_manager->getScreen(ALARMSX);

      m_screen.alarms.visible = true;
      m_screen.control->active = 0x1 << 6;
    }
  };
  void react(KeySA const &) override { transit<stateSA>(); };
  void react(KeyWPN const &) override { transit<stateWPN>(); };
  void react(KeyDEF const &) override { transit<stateDEF>(); };
  void react(KeySYS const &) override { transit<stateSYS>(); };
  void react(KeyDRV const &) override { transit<stateDRV>(); };
  void react(KeySTR const &) override { transit<stateSTR>(); };
  void react(KeyCOM const &) override { transit<stateCOM>(); };
  void react(KeyBMS const &) override { transit<stateBMS>(); };
  void react(KeyAlarms const &) override { transit<stateAlarms>(); };
  void react(KeyFunction const &e) { key(e.key); };
};

struct stateOn : Hmi
{
  void entry() override {
    /* 4:3 aspect ratio @ lowest resolution */
    m_view = { MIN_WIDTH, MIN_HEIGHT, 24 };
    
    if(!m_manager)
      m_manager = new viewGvaManager(&m_status);
    
    // Render a map for BMS
    m_map = new rendererMap("/opt/osmscout/maps/england-latest/", 
      "/opt/osmscout/stylesheets/standard.oss", MIN_WIDTH, MIN_HEIGHT);
    
    init(m_top, functionSelectType, COMMON_FUNCTION_KEYS_TOP);
    init(m_bottom, commonTaskKeysType, COMMON_KEYS);
    init(m_status, statusBarType, COMMON_STATUS_BAR);
    init(m_canvas, canvasType, CANVAS);
    init(m_keyboard, keyboardType, KEYBOARD);
    init(m_alarms, alarmsType, ALARMS);

    // Setup the main screens
    m_manager->getNewView(SA,     &m_top, &m_bottom, (functionKeys)SA_FUNCTION_KEYS_LEFT,  (functionKeys)SA_FUNCTION_KEYS_RIGHT);
    m_manager->getNewView(WPN,    &m_top, &m_bottom, (functionKeys)WPN_FUNCTION_KEYS_LEFT, (functionKeys)WPN_FUNCTION_KEYS_RIGHT);
    m_manager->getNewView(DEF,    &m_top, &m_bottom, (functionKeys)DEF_FUNCTION_KEYS_LEFT, (functionKeys)DEF_FUNCTION_KEYS_RIGHT);
    m_manager->getNewView(SYS,    &m_top, &m_bottom, (functionKeys)SYS_FUNCTION_KEYS_LEFT, (functionKeys)SYS_FUNCTION_KEYS_RIGHT);
    m_manager->getNewView(DRV,    &m_top, &m_bottom, (functionKeys)DRV_FUNCTION_KEYS_LEFT, (functionKeys)DRV_FUNCTION_KEYS_RIGHT);
    m_manager->getNewView(STR,    &m_top, &m_bottom, (functionKeys)STR_FUNCTION_KEYS_LEFT, (functionKeys)STR_FUNCTION_KEYS_RIGHT);
    m_manager->getNewView(COM,    &m_top, &m_bottom, (functionKeys)COM_FUNCTION_KEYS_LEFT, (functionKeys)COM_FUNCTION_KEYS_RIGHT);
    m_manager->getNewView(BMS,    &m_top, &m_bottom, (functionKeys)BMS_FUNCTION_KEYS_LEFT, (functionKeys)BMS_FUNCTION_KEYS_RIGHT);
    m_manager->getNewView(ALARMSX, &m_top, &m_bottom, (functionKeys)ALARM_KEYS_LEFT,        (functionKeys)ALARM_KEYS_RIGHT);

    m_screen = m_manager->getScreen(SYS);
    m_widgets.compass.bearingSight = 33;
    m_widgets.compass.x = 165; 
    m_widgets.compass.y = 370;
    m_widgets.compass.visible = true;
    m_widgets.alarmIndicator.visible = true;
    m_widgets.alarmIndicator.y = 422;
    strcpy(m_widgets.alarmIndicator.text,"Engine over tempreture");

    m_screen.canvas = m_canvas;
    m_screen.canvas.visible = true;
    m_screen.alarms = m_alarms;
    m_screen.alarms.visible = false;
    m_screen.labels = LABEL_ALL;
    SET_CANVAS_PNG("test2.png");
    
    /* These are comon to all screens */
    m_render = new screenGva (&m_screen, &m_widgets, m_view.width, m_view.height);

    /*
     * Draw the inital screen and start the Real Time Clock
     */
    m_render->update ();
    m_render->startClock (&m_status);

    transit<stateSA>(); 
  };
};

struct stateOff : Hmi
{
  void entry() override { 
    free(m_render);
    free(m_manager);
    m_render = 0;
    m_manager = 0;
  };
  void react(KeyPowerOn const &) override { transit<stateOn>(); };
};

viewGvaManager *Hmi::m_manager;
resolution_type Hmi::m_view;
statusBarType Hmi::m_status;
functionSelectType Hmi::m_top;
commonTaskKeysType Hmi::m_bottom;
canvasType Hmi::m_canvas;
keyboardType Hmi::m_keyboard;
alarmsType Hmi::m_alarms;
screenType Hmi::m_screen;
widgetsType Hmi::m_widgets;
screenGva* Hmi::m_render;
rendererMap* Hmi::m_map;
int  Hmi::m_lastState;
bool Hmi::m_alarmsOn = false;
xmlData Hmi::xml;

// ----------------------------------------------------------------------------
// Initial state definition
//
FSM_INITIAL_STATE(Hmi, stateOff)
