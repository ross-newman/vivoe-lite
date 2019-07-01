#ifndef SCREEN_GVA_H
#define SCREEN_GVA_H
#include <pthread.h>
#include "cairo.h"
#include "gva.h"
#include "renderer_gva.h"
#include "log_gva.h"
#include "nmea/nmea.h"

namespace gva
{
  enum locationEnum {
    LOCATION_FORMAT_LONG_LAT = 0,
    LOCATION_FORMAT_MGRS
  };
  
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

  typedef struct {
    locationEnum locationFormat;
    float lat;
    float lon;
  } locationType;

  typedef struct statusBar {
    bool visible;
    locationType location;
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

  typedef struct {
    bool visible;
  } alarmsType;
  
  enum surfaceType {
    SURFACE_NONE = 0,
    SURFACE_FILE,
    SURFACE_BUFFER_RGB24,
    SURFACE_CAIRO
  };

  typedef struct canvas {
    bool visible;
    surfaceType bufferType;
    char filename[256];
    char* buffer;
    cairo_surface_t *surface;
  } canvasType;

  typedef struct label {
    bool visible;
    char text[256];
    int x;
    int y;
    int fontSize;
  } labelType;

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
    labelType label;
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
      locationType *location;
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
