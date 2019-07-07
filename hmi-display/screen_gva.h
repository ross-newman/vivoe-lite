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
  
  enum screenMode {
    MODE_MAINTINENCE = 0,
    MODE_OPERATIONAL
  };

  enum iconEnum {
    ICON_NONE = 0,
    ICON_WARNING,
    ICON_INFO,
    ICON_ERROR 
  };
  
  enum labelModeEnum {
    LABEL_ALL,
    LABEL_STATUS_ONLY,
    LABEL_MINIMAL
  };
  
  typedef struct  {
    bool visible;
    int x;
    int y;
  } widget;
  
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
    int x;
    int y;
    locationType location;
    char labels[6][80];
  } statusBarType;

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
  
  typedef struct message {
    bool visible;
    int width;
    iconEnum icon;
    struct {
      char text[256];
      int fontSize;
    } brief;
    struct {
      char text[4096];
      int fontSize;
    } detail;
  } messageType;

  typedef struct screen {
    struct {
      char name[100];
      screenMode mode;
      char gpsDevice[100];
    } info;
    gvaFunctionEnum currentFunction;
    canvasType canvas;
    functionSelectType *functionTop;
    commonTaskKeysType *control;
    statusBarType *statusBar;
    functionKeysType functionLeft;
    functionKeysType functionRight;
    alarmsType alarms;
    labelType label;
    messageType message;
    labelModeEnum labels;
  } screenType;
  
  //
  // Widgets
  //
  typedef struct compass {
    bool visible;
    int x;
    int y;
    int bearing;
    int bearingSight;
  } compassType;

  typedef struct keyboard {
    bool visible;
    keyboardModeType mode;
  } keyboardType;

  typedef struct alarmIndicator {
    bool visible;
    int y;
    char text[256];
    gvaAlarmType type;
  } alarmIndicatorType;
  
  typedef struct {
    compassType compass;
    keyboardType keyboard;
    alarmIndicatorType alarmIndicator;
  } widgetsType;

  class screenGva;

  //
  // These are used by the clock thread to update the time and refresh the screen 
  //
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
    screenGva(screenType *screen, widgetsType *widgets, int width, int height);
    ~screenGva();
    int update();
    int refresh();
    void startClock(statusBarType *barData);
  private:
    char *posDegrees(float lon, float lat);
    screenType *m_screen;
    widgetsType *m_widgets;
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
