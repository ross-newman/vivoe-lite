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
  enum LocationEnum {
    LOCATION_FORMAT_LONG_LAT = 0,
    LOCATION_FORMAT_MGRS
  };
  
  enum ScreenMode {
    MODE_MAINTINENCE = 0,
    MODE_OPERATIONAL,
    MODE_BLACKOUT
  };

  enum IconEnum {
    ICON_NONE = 0,
    ICON_WARNING,
    ICON_INFO,
    ICON_ERROR 
  };
  
  enum LabelModeEnum {
    LABEL_ALL,
    LABEL_STATUS_ONLY,
    LABEL_MINIMAL
  };
  
  typedef struct  {
    bool visible;
    int x;
    int y;
  } Widget;
  
  typedef struct FunctionSelect {
    bool visible;
    int active;
    int hidden;
  } FunctionSelectType;

  typedef struct FunctionKeys {
    bool visible;
    int active;
    int hidden;
    int toggleActive;
    int toggleOn;
    char labels[6][40];
  } FunctionKeysType;

  typedef struct CommonTaskKeys {
    bool visible;
    int active;
    int hidden;
    char labels[8][40];
  } CommonTaskKeysType;

  typedef struct {
    LocationEnum locationFormat;
    float lat;
    float lon;
  } LocationType;

  typedef struct StatusBar {
    bool visible;
    int x;
    int y;
    LocationType location;
    char labels[7][80];
  } StatusBarType;

  typedef struct {
    int width;
    char text[256];
    long background_colour;
    long foreground_colour;
    long outline_colour;
    long highlight_colour;
    cellAlignType alignment;  
  } CellType;
  
  #define MAX_CELLS 20
  typedef struct {
	int cell_count;
    CellType cells[MAX_CELLS];
    long background_colour;
    long foreground_colour;
    long outline_colour;
    long highlight_colour;
    WeightType font_weight;
    bool highlighted;
    cellAlignType alignment;
  } RowType;
  
  #define MAX_ROWS 100
  class TableWidget {
  public:
    void AddRow(long forground_colour, long background_colour, 
                long outline_colour, long highlight_colour, WeightType font_weight) {
				RowType *row = &rows_[row_count_]; 
		            row->background_colour = background_colour;
		            row->foreground_colour = forground_colour;
		            row->outline_colour = outline_colour;
		            row->highlight_colour = highlight_colour;
		            row->font_weight = font_weight;
		            row->highlighted = false;
		            row_count_+=1; }
    void AddRow() { RowType *row = &rows_[row_count_]; 
		            row->background_colour = renderer::PackRgb(DARK_GREEN);
		            row->foreground_colour = renderer::PackRgb(WHITE);
		            row->outline_colour = renderer::PackRgb(WHITE);
		            row->highlight_colour = renderer::PackRgb(YELLOW);
		            row->font_weight = WEIGHT_NORMAL;
		            row->highlighted = false;
		            row->alignment = ALIGN_LEFT;
		            row_count_+=1; }
	void AddRow(WeightType font_weight)	{ RowType *row = &rows_[row_count_];
		                                  AddRow(); 
		                                  row->font_weight = font_weight; }
    void CurrentRowHighlight() {rows_[row_count_-1].highlighted = true; };
	void AddCell(char* text, int width) { AddCell(text, width, ALIGN_LEFT); }
	void AddCell(char* text, int width, 
	             long background_colour) { CellType *cell = &rows_[row_count_-1].cells[rows_[row_count_-1].cell_count];
					                      AddCell(text, width, ALIGN_LEFT);
		                                  cell->background_colour = background_colour;}
	void AddCell(char* text, int width, cellAlignType align) {
		CellType *cell = &rows_[row_count_-1].cells[rows_[row_count_-1].cell_count];
	    cell->background_colour = rows_[row_count_-1].background_colour;
	    cell->foreground_colour = rows_[row_count_-1].foreground_colour;
	    cell->outline_colour = rows_[row_count_-1].outline_colour;
	    cell->highlight_colour = rows_[row_count_-1].highlight_colour;
	    strcpy(cell->text, text); 
	    cell->width = width;
		cell->alignment = align; 
		rows_[row_count_-1].cell_count+=1; }
    bool visible_;
    int height_;
    int width_;
    int x_;
    int y_;
    int row_count_;
    RowType rows_[MAX_ROWS];
  };
  
  enum SurfaceType {
    SURFACE_NONE = 0,
    SURFACE_FILE,
    SURFACE_BUFFER_RGB24,
    SURFACE_CAIRO,
    SURFACE_BLACKOUT
  };

  typedef struct Canvas {
    bool visible;
    SurfaceType bufferType;
    char filename[256];
    char* buffer;
    cairo_surface_t *surface;
  } CanvasType;

  typedef struct Label {
    bool visible;
    char text[256];
    int x;
    int y;
    int fontSize;
  } LabelType;
  
  typedef struct Message {
    bool visible;
    int width;
    IconEnum icon;
    struct {
      char text[256];
      int fontSize;
    } brief;
    struct {
      char text[4096];
      int fontSize;
    } detail;
  } MessaGetYpe;

  typedef struct Screen {
    struct {
      char name[100];
      ScreenMode mode;
      char gpsDevice[100];
    } info;
    GvaFunctionEnum currentFunction;
    CanvasType canvas;
    FunctionSelectType *functionTop;
    CommonTaskKeysType *control;
    StatusBarType *StatusBar;
    FunctionKeysType functionLeft;
    FunctionKeysType functionRight;
    TableWidget table;
    LabelType label;
    MessaGetYpe message;
    LabelModeEnum labels;
  } ScreenType;
  
  //
  // Widgets
  //
  typedef struct Compass {
    bool visible;
    int x;
    int y;
    int bearing;
    int bearingSight;
  } CompassType;

  typedef struct Keyboard {
    bool visible;
    KeyboardModeType mode;
  } KeyboardType;

  typedef struct AlarmIndicator {
    bool visible;
    int y;
    char text[256];
    GvaAlarmType type;
  } alarmIndicatorType;
  
  typedef struct {
    CompassType compass;
    KeyboardType keyboard;
    alarmIndicatorType alarmIndicator;
  } WidgetsType;

  class ScreenGva;

  //
  // These are used by the clock thread to update the time and refresh the screen 
  //
  typedef struct ArgStruct {
      char* clockString;
      char* locationFormat;
      char* locationString;
      ScreenGva* screen;
      int *gps;
      nmeaINFO *info;
      nmeaPARSER *parser;
      bool active;
      LocationType *location;
  } args;

  class ScreenGva : public RendererGva
  {
  public:
    ScreenGva(ScreenType *screen, WidgetsType *Widgets, int width, int height);
    ~ScreenGva();
    int Update();
    void StartClock(StatusBarType *barData);
  private:
    char *PosDegrees(float lon, float lat);
    ScreenType *screen_;
    WidgetsType *widgets_;
    args *args_;
    int gps_;
    int hndl_;
    ScreenType last_screen_;
    pthread_t clock_thread_;
    nmeaINFO info_;
    nmeaPARSER parser_;
  };
}
#endif
