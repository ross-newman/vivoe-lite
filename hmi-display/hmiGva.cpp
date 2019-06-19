#include <iostream>
#include "hmiGva.h"

using namespace gva;
using namespace std;

#define CANVAS { true, "test2.png", 0 }
#define KEYBOARD { false, KEYBOARD_UPPER }
#define ALARMS { false }
#define SCREEN { "Situational Awareness", "/dev/ttyUSB0", SA, m_canvas, &m_top, &m_status, SYS_FUNCTION_KEYS_LEFT, SYS_FUNCTION_KEYS_RIGHT, COMMON_KEYS, COMPASS, m_keyboard, m_alarms }
#define init(var, typ, data) {typ x = data; var = x; }
#define BIT(b,x) (x & 0x1 << b)
#define SET_CANVAS_PNG(file) strcpy (m_screen->canvas.filename, file); m_screen->canvas.buffer = 0;

struct stateSA : Hmi
{
  void entry() override {
    if (!BIT (7, m_screen->functionTop->hidden))
      {
        functionKeysType left = SA_FUNCTION_KEYS_LEFT;
        functionKeysType right = SA_FUNCTION_KEYS_RIGHT;
        m_screen->currentFunction = SA;
        
        m_screen->compass.visible = true;
        m_screen->canvas.visible = true;
        m_screen->alarms.visible = false;
        SET_CANVAS_PNG("test2.png");
        m_screen->functionTop->active = 0x1 << 7;
        m_screen->functionLeft = left;
        m_screen->functionRight = right;
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
};

struct stateWPN : Hmi
{
  void entry() override {
    if (!BIT (6, m_screen->functionTop->hidden))
      {
        functionKeysType left = WPN_FUNCTION_KEYS_LEFT;
        functionKeysType right = WPN_FUNCTION_KEYS_RIGHT;
        m_screen->currentFunction = DEF;

        m_screen->compass.visible = true;
        m_screen->canvas.visible = true;
        m_screen->alarms.visible = false;
        SET_CANVAS_PNG("test2.png");
        m_screen->functionTop->active = 0x1 << 6;
        m_screen->functionLeft = left;
        m_screen->functionRight = right;
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
};

struct stateDEF : Hmi
{
  void entry() override {
    if (!BIT (5, m_screen->functionTop->hidden))
      {
        functionKeysType left = DEF_FUNCTION_KEYS_LEFT;
        functionKeysType right = DEF_FUNCTION_KEYS_RIGHT;
        m_screen->currentFunction = SYS;

        m_screen->compass.visible = false;
        m_screen->canvas.visible = false;
        m_screen->alarms.visible = false;
        m_screen->functionTop->active = 0x1 << 5;
        m_screen->functionLeft = left;
        m_screen->functionRight = right;
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
};
                
struct stateSYS : Hmi
{
  void entry() override {
    if (!BIT (4, m_screen->functionTop->hidden))
      {
        functionKeysType left = SYS_FUNCTION_KEYS_LEFT;
        functionKeysType right = SYS_FUNCTION_KEYS_RIGHT;
        m_screen->currentFunction = DRV;

        m_screen->compass.visible = true;
        m_screen->canvas.visible = true;
        m_screen->alarms.visible = false;
#if 0
        if (rtpBuffer) {
          m_screen->canvas.buffer = rtpBuffer;
        } else {
          SET_CANVAS_PNG("test2.png");
        }
#endif
        m_screen->functionTop->active = 0x1 << 4;
        m_screen->functionLeft = left;
        m_screen->functionRight = right;
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
};

struct stateDRV : Hmi
{
  void entry() override {
    if (!BIT (3, m_screen->functionTop->hidden))
      {
        functionKeysType left = DRV_FUNCTION_KEYS_LEFT;
        functionKeysType right = DRV_FUNCTION_KEYS_RIGHT;
        m_screen->currentFunction = STR;

        m_screen->compass.visible = false;
        m_screen->canvas.visible = false;
        m_screen->alarms.visible = false;
        m_screen->functionTop->active = 0x1 << 3;
        m_screen->functionLeft = left;
        m_screen->functionRight = right;
      }
  };
  void react(KeySA const &) override { transit<stateSA>(); };
  void react(KeyWPN const &) override { transit<stateWPN>(); };
  void react(KeyDEF const &) override { transit<stateDEF>(); };
  void react(KeySYS const &) override { transit<stateSYS>(); };
  void react(KeySTR const &) override { transit<stateSTR>(); };
  void react(KeyCOM const &) override { transit<stateCOM>(); };
  void react(KeyBMS const &) override { transit<stateBMS>(); };
};

struct stateSTR : Hmi
{
  void entry() override {
    if (!BIT (2, m_screen->functionTop->hidden))
      {
        functionKeysType left = STR_FUNCTION_KEYS_LEFT;
        functionKeysType right = STR_FUNCTION_KEYS_RIGHT;
        m_screen->currentFunction = COM;

        m_screen->compass.visible = false;
        m_screen->canvas.visible = false;
        m_screen->alarms.visible = false;
        m_screen->functionTop->active = 0x1 << 2;
        m_screen->functionLeft = left;
        m_screen->functionRight = right;
      }
  };
  void react(KeySA const &) override { transit<stateSA>(); };
  void react(KeyWPN const &) override { transit<stateWPN>(); };
  void react(KeyDEF const &) override { transit<stateDEF>(); };
  void react(KeySYS const &) override { transit<stateSYS>(); };
  void react(KeyDRV const &) override { transit<stateDRV>(); };
  void react(KeyCOM const &) override { transit<stateCOM>(); };
  void react(KeyBMS const &) override { transit<stateBMS>(); };
};


struct stateCOM : Hmi
{
  void entry() override {
    if (!BIT (1, m_screen->functionTop->hidden))
      {
        functionKeysType left = COM_FUNCTION_KEYS_LEFT;
        functionKeysType right = COM_FUNCTION_KEYS_RIGHT;
        m_screen->currentFunction = BMS;

        m_screen->compass.visible = false;
        m_screen->compass.visible = false;
        m_screen->alarms.visible = false;
        m_screen->canvas.visible = false;
        m_screen->functionTop->active = 0x1 << 1;
        m_screen->functionLeft = left;
        m_screen->functionRight = right;
      }
  };
  void react(KeySA const &) override { transit<stateSA>(); };
  void react(KeyWPN const &) override { transit<stateWPN>(); };
  void react(KeyDEF const &) override { transit<stateDEF>(); };
  void react(KeySYS const &) override { transit<stateSYS>(); };
  void react(KeyDRV const &) override { transit<stateDRV>(); };
  void react(KeySTR const &) override { transit<stateSTR>(); };
  void react(KeyBMS const &) override { transit<stateBMS>(); };
};

struct stateBMS : Hmi
{
  void entry() override {
    if (!BIT (0, m_screen->functionTop->hidden))
      {
        functionKeysType left = BMS_FUNCTION_KEYS_LEFT;
        functionKeysType right = BMS_FUNCTION_KEYS_RIGHT;

        m_screen->compass.visible = false;
        m_screen->canvas.visible = true;
        m_screen->alarms.visible = false;
        SET_CANVAS_PNG("map.png");
        m_screen->functionTop->active = 0x1 << 0;
        m_screen->functionLeft = left;
        m_screen->functionRight = right;
      }
  };
  void react(KeySA const &) override { transit<stateSA>(); };
  void react(KeyWPN const &) override { transit<stateWPN>(); };
  void react(KeyDEF const &) override { transit<stateDEF>(); };
  void react(KeySYS const &) override { transit<stateSYS>(); };
  void react(KeyDRV const &) override { transit<stateDRV>(); };
  void react(KeySTR const &) override { transit<stateSTR>(); };
  void react(KeyCOM const &) override { transit<stateCOM>(); };
};


struct stateOn : Hmi
{
  void entry() override {
    /* 4:3 aspect ratio */
    m_view = { 640, 480, 24 };
    init(m_status, statusBarType, COMMON_STATUS_BAR);
    init(m_top, functionSelectType, COMMON_FUNCTION_KEYS_TOP);
    init(m_canvas, canvasType, CANVAS);
    init(m_keyboard, keyboardType, KEYBOARD);
    init(m_alarms, alarmsType, ALARMS);
    init(m_screen_sa, screenType, SCREEN);
    m_screen_sa.statusBar = &m_status;
    /* These are comon to all screens */
    m_screen = &m_screen_sa;
    m_render = new screenGva (m_screen, m_view.width, m_view.height);
    /*
     * Draw the inital screen and start the Real Time Clock
     */
    m_render->update (m_screen);
    m_render->startClock (&m_status);
    cout << "INIT CHANGE\n";
    transit<stateSA>(); 
  };
//  void react(KeyPowerOn const &) override { transit<stateSA>(); };
};

struct stateOff : Hmi
{
  void entry() override { 
    free(m_render);
  };
  void react(KeyPowerOn const &) override { transit<stateOn>(); };
};

resolution_type Hmi::m_view;
statusBarType Hmi::m_status;
functionSelectType Hmi::m_top;
canvasType Hmi::m_canvas;
keyboardType Hmi::m_keyboard;
alarmsType Hmi::m_alarms;
screenType Hmi::m_screen_sa;
screenType* Hmi::m_screen;
screenGva* Hmi::m_render;

// ----------------------------------------------------------------------------
// Initial state definition
//
FSM_INITIAL_STATE(Hmi, stateOff)
