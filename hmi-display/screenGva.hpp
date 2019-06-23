#ifndef SCREEN_GVA_HPP
#define SCREEN_GVA_HPP
#include <pthread.h>
#include "gva.hpp"
#include "rendererGva.hpp"
#include "logGva.hpp"
#include "nmea/nmea.h"

namespace gva
{
  typedef struct functionSelect {
    bool visible;
    int active;
    int hidden;
  } functionSelectType;

  typedef struct functionKeys {
    bool visible;
    int active;
    int hidden;
    int toggleActive;
    int toggleOn;
    char labels[6][40];
  } functionKeysType;

  typedef struct commonTaskKeys {
    bool visible;
    int active;
    int hidden;
    char labels[8][40];
  } commonTaskKeysType;

  typedef struct statusBar {
    bool visible;
    char labels[6][80];
  } statusBarType;

  typedef struct compass {
    bool visible;
    int bearing;
    int bearingSight;
  } compassType;

  typedef struct keyboard {
    bool visible;
    keyboardModeType mode;
  } keyboardType;

  typedef struct alarmsType {
    bool visible;
  };

  typedef struct canvas {
    bool visible;
    char filename[256];
    char* buffer;
  } canvasType;

  typedef struct screen {
    char name[100];
    char gpsDevice[100];
    gvaFunctionEnum currentFunction;
    canvasType canvas;
    functionSelectType *functionTop;
    commonTaskKeysType *control;
    statusBarType *statusBar;
    functionKeysType functionLeft;
    functionKeysType functionRight;
    compassType compass;
    keyboardType keyboard;
    alarmsType alarms;
  } screenType;

  class screenGva;

  /*
   * These are used by the clock thread to update the time and refresh the screen 
   */
  typedef struct arg_struct {
      char* clockString;
      char* locationString;
      screenGva* screen;
      int *gps;
      nmeaINFO *info;
      nmeaPARSER *parser;
      bool active;
  } args;

  class screenGva : public rendererGva
  {
  public:
    screenGva(screenType *screen, int width, int height);
    ~screenGva();
    int update(screenType *screen);
    int refresh();
    void startClock(statusBarType *barData);
  private:
    char *posDegrees(float lon, float lat);
    screenType *m_screen;
    args *m_args;
    int m_gps;
    int m_hndl;
    int m_width;
    int m_height;
    screenType m_last_screen;
    pthread_t m_clock_thread;
    nmeaINFO m_info;
    nmeaPARSER m_parser;
  };
}
#endif
