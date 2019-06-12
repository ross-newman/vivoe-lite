#ifndef SCREEN_GVA_H
#define SCREEN_GVA_H
#include <pthread.h>
#include "rendererGva.h"
#include "logGva.h"
#include "nmea/nmea.h"

#define MAJOR 0
#define MINOR 0
#define PATCH 2

namespace gva
{

  enum gvaFunctionEnum 
  {
    SA,
    WPN,
    DEF,
    SYS,
    DRV,
    STR,
    COM,
    BMS
  };

  struct functionSelectType {
    bool visible;
    int active;
    int hidden;
  };

  struct functionKeysType {
    bool visible;
    int active;
    int hidden;
    int toggleActive;
    int toggleOn;
    char labels[6][40];
  };

  struct commonTaskKeysType {
    bool visible;
    int active;
    int hidden;
    char labels[8][40];
  };

  struct statusBarType {
    bool visible;
    char labels[5][80];
  };

  struct compassType {
    bool visible;
    int bearing;
    int bearingSight;
  };

  struct keyboardType {
    bool visible;
    keyboardModeType mode;
  };

  struct canvasType {
    bool visible;
    char filename[256];
    char* buffer;
  };

  struct screenType {
    char name[100];
    char gpsDevice[100];
    gvaFunctionEnum currentFunction;
    canvasType canvas;
    functionSelectType *functionTop;
    statusBarType *statusBar;
    functionKeysType functionLeft;
    functionKeysType functionRight;
    commonTaskKeysType control;
    compassType compass;
    keyboardType keyboard;
  };

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
