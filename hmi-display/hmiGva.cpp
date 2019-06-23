#include <iostream>
#include "hmiGva.hpp"
#include "viewGva.hpp"

using namespace gva;
using namespace std;

#define CANVAS { true, "test2.png", 0 }
#define KEYBOARD { false, KEYBOARD_UPPER }
#define ALARMS { false }
#define SCREEN { "Situational Awareness", "/dev/ttyUSB0", SA, m_canvas, &m_top, &m_status, SYS_FUNCTION_KEYS_LEFT, SYS_FUNCTION_KEYS_RIGHT, COMMON_KEYS, COMPASS, m_keyboard, m_alarms }
#define init(var, typ, data) {typ x = data; var = x; }
#define BIT(b,x) (x & 0x1 << b)
#define SET_CANVAS_PNG(file) strcpy (m_screen.canvas.filename, file); m_screen.canvas.buffer = 0;

void 
Hmi::labelsOff() {
  getScreen()->functionLeft.visible = false;
  getScreen()->functionRight.visible = false;
  getScreen()->control->visible = false;
  getScreen()->statusBar->visible = false;
  getScreen()->compass.visible = false;
  m_labelsOn = false;
};

void 
Hmi::labelsOn() {
  getScreen()->functionLeft.visible = true;
  getScreen()->functionRight.visible = true;
  getScreen()->control->visible = true;
  getScreen()->statusBar->visible = true;
  getScreen()->compass.visible = true;
  m_labelsOn = true;
};

struct stateSA : Hmi
{
  void entry() override {
    if (!BIT (7, m_screen.functionTop->hidden))
      {
        m_screen = m_manager->getScreen(SA);
        
        m_screen.compass.visible = true;
        m_screen.canvas.visible = true;
        m_screen.alarms.visible = false;
        SET_CANVAS_PNG("test2.png");
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
  void react(KeyFunction const &) { m_labelsOn ? Hmi::labelsOff() : Hmi::labelsOn(); };
};

struct stateWPN : Hmi
{
  void entry() override {
    if (!BIT (6, m_screen.functionTop->hidden))
      {
        m_screen = m_manager->getScreen(WPN);

        m_screen.compass.visible = true;
        m_screen.canvas.visible = true;
        m_screen.alarms.visible = false;
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
  void react(KeyFunction const &) { m_labelsOn ? Hmi::labelsOff() : Hmi::labelsOn(); };
};

struct stateDEF : Hmi
{
  void entry() override {
    if (!BIT (5, m_screen.functionTop->hidden))
      {
        m_screen = m_manager->getScreen(DEF);

        m_screen.compass.visible = false;
        m_screen.canvas.visible = false;
        m_screen.alarms.visible = false;
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
  void react(KeyFunction const &) { m_labelsOn ? Hmi::labelsOff() : Hmi::labelsOn(); };
};
                
struct stateSYS : Hmi
{
  void entry() override {
    if (!BIT (4, m_screen.functionTop->hidden))
      {
        m_screen = m_manager->getScreen(SYS);

        m_screen.compass.visible = true;
        m_screen.canvas.visible = true;
        m_screen.alarms.visible = false;
#if 0
        if (rtpBuffer) {
          m_screen.canvas.buffer = rtpBuffer;
        } else {
          SET_CANVAS_PNG("test2.png");
        }
#endif
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
  void react(KeyFunction const &) { m_labelsOn ? Hmi::labelsOff() : Hmi::labelsOn(); };
};

struct stateDRV : Hmi
{
  void entry() override {
    if (!BIT (3, m_screen.functionTop->hidden))
      {
        m_screen = m_manager->getScreen(DRV);

        m_screen.compass.visible = false;
        m_screen.canvas.visible = false;
        m_screen.alarms.visible = false;
        m_screen.functionTop->active = 0x1 << 3;
      }
  };
  void react(KeySA const &) override { transit<stateSA>(); };
  void react(KeyWPN const &) override { transit<stateWPN>(); };
  void react(KeyDEF const &) override { transit<stateDEF>(); };
  void react(KeySYS const &) override { transit<stateSYS>(); };
  void react(KeySTR const &) override { transit<stateSTR>(); };
  void react(KeyCOM const &) override { transit<stateCOM>(); };
  void react(KeyBMS const &) override { transit<stateBMS>(); };
  void react(KeyFunction const &) { m_labelsOn ? Hmi::labelsOff() : Hmi::labelsOn(); };
};

struct stateSTR : Hmi
{
  void entry() override {
    if (!BIT (2, m_screen.functionTop->hidden))
      {
        m_screen = m_manager->getScreen(STR);

        m_screen.compass.visible = false;
        m_screen.canvas.visible = false;
        m_screen.alarms.visible = false;
        m_screen.functionTop->active = 0x1 << 2;
      }
  };
  void react(KeySA const &) override { transit<stateSA>(); };
  void react(KeyWPN const &) override { transit<stateWPN>(); };
  void react(KeyDEF const &) override { transit<stateDEF>(); };
  void react(KeySYS const &) override { transit<stateSYS>(); };
  void react(KeyDRV const &) override { transit<stateDRV>(); };
  void react(KeyCOM const &) override { transit<stateCOM>(); };
  void react(KeyBMS const &) override { transit<stateBMS>(); };
  void react(KeyFunction const &) { m_labelsOn ? Hmi::labelsOff() : Hmi::labelsOn(); };
};

struct stateCOM : Hmi
{
  void entry() override {
    if (!BIT (1, m_screen.functionTop->hidden))
      {
        m_screen = m_manager->getScreen(COM);

        m_screen.compass.visible = false;
        m_screen.compass.visible = false;
        m_screen.alarms.visible = false;
        m_screen.canvas.visible = false;
        m_screen.functionTop->active = 0x1 << 1;
      }
  };
  void react(KeySA const &) override { transit<stateSA>(); };
  void react(KeyWPN const &) override { transit<stateWPN>(); };
  void react(KeyDEF const &) override { transit<stateDEF>(); };
  void react(KeySYS const &) override { transit<stateSYS>(); };
  void react(KeyDRV const &) override { transit<stateDRV>(); };
  void react(KeySTR const &) override { transit<stateSTR>(); };
  void react(KeyBMS const &) override { transit<stateBMS>(); };
  void react(KeyFunction const &) { m_labelsOn ? Hmi::labelsOff() : Hmi::labelsOn(); };
};

struct stateBMS : Hmi
{
  void entry() override {
    if (!BIT (0, m_screen.functionTop->hidden))
      {
        m_screen = m_manager->getScreen(BMS);

        m_screen.compass.visible = false;
        m_screen.canvas.visible = true;
        m_screen.alarms.visible = false;
        SET_CANVAS_PNG("map.png");
        m_screen.functionTop->active = 0x1 << 0;
      }
  };
  void react(KeySA const &) override { transit<stateSA>(); };
  void react(KeyWPN const &) override { transit<stateWPN>(); };
  void react(KeyDEF const &) override { transit<stateDEF>(); };
  void react(KeySYS const &) override { transit<stateSYS>(); };
  void react(KeyDRV const &) override { transit<stateDRV>(); };
  void react(KeySTR const &) override { transit<stateSTR>(); };
  void react(KeyCOM const &) override { transit<stateCOM>(); };
  void react(KeyFunction const &) { m_labelsOn ? Hmi::labelsOff() : Hmi::labelsOn(); };
};

struct stateOn : Hmi
{
  void entry() override {
    /* 4:3 aspect ratio */
    m_view = { 640, 480, 24 };
    
    if(!m_manager)
      m_manager = new viewGvaManager(&m_status);
    viewGva *view;
    m_labelsOn = true;
    
    init(m_top, functionSelectType, COMMON_FUNCTION_KEYS_TOP);
    init(m_bottom, commonTaskKeysType, COMMON_KEYS);
    init(m_status, statusBarType, COMMON_STATUS_BAR);
    init(m_canvas, canvasType, CANVAS);
    init(m_keyboard, keyboardType, KEYBOARD);
    init(m_alarms, alarmsType, ALARMS);

    // Setup the main screens
    view = m_manager->getNewView(SA, &m_top, &m_bottom, (functionKeys)SA_FUNCTION_KEYS_LEFT, (functionKeys)SA_FUNCTION_KEYS_RIGHT);
    view = m_manager->getNewView(WPN, &m_top, &m_bottom, (functionKeys)WPN_FUNCTION_KEYS_LEFT, (functionKeys)WPN_FUNCTION_KEYS_RIGHT);
    view = m_manager->getNewView(DEF, &m_top, &m_bottom, (functionKeys)DEF_FUNCTION_KEYS_LEFT, (functionKeys)DEF_FUNCTION_KEYS_RIGHT);
    view = m_manager->getNewView(SYS, &m_top, &m_bottom, (functionKeys)SYS_FUNCTION_KEYS_LEFT, (functionKeys)SYS_FUNCTION_KEYS_RIGHT);
    view = m_manager->getNewView(DRV, &m_top, &m_bottom, (functionKeys)DRV_FUNCTION_KEYS_LEFT, (functionKeys)DRV_FUNCTION_KEYS_RIGHT);
    view = m_manager->getNewView(STR, &m_top, &m_bottom, (functionKeys)STR_FUNCTION_KEYS_LEFT, (functionKeys)STR_FUNCTION_KEYS_RIGHT);
    view = m_manager->getNewView(COM, &m_top, &m_bottom, (functionKeys)COM_FUNCTION_KEYS_LEFT, (functionKeys)COM_FUNCTION_KEYS_RIGHT);
    view = m_manager->getNewView(BMS, &m_top, &m_bottom, (functionKeys)BMS_FUNCTION_KEYS_LEFT, (functionKeys)BMS_FUNCTION_KEYS_RIGHT);

    m_screen = m_manager->getScreen(SYS);
    m_screen.compass.visible = true;
    m_screen.canvas = m_canvas;
    m_screen.canvas.visible = true;
    m_screen.alarms = m_alarms;
    m_screen.alarms.visible = false;
    SET_CANVAS_PNG("test2.png");
    
    /* These are comon to all screens */
    m_render = new screenGva (&m_screen, m_view.width, m_view.height);

    /*
     * Draw the inital screen and start the Real Time Clock
     */
    m_render->update (&m_screen);
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
screenGva* Hmi::m_render;
bool Hmi::m_labelsOn;

// ----------------------------------------------------------------------------
// Initial state definition
//
FSM_INITIAL_STATE(Hmi, stateOff)
