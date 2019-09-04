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

#include <GeographicLib/LambertConformalConic.hpp> 
#include <GeographicLib/UTMUPS.hpp>
#include <GeographicLib/MGRS.hpp>
#include <iostream>
#include <string>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include "screen_gva.h"
#include "log_gva.h"
#include "debug.h"

using namespace std;
using namespace GeographicLib; 

#define MAX_NMEA 1000

float toDegrees(float lon) {
  int d = (int)lon/100;
  float m = lon - d*100;
  return d+m/(float)60;
}

namespace gva
{
  ScreenGva::ScreenGva (ScreenType * screen, WidgetsType * Widgets, int width, int height)
  : RendererGva (width, height)
  {
    screen_ = screen;
    widgets_ = Widgets;

    char tmp[100];
    struct termios settings;

    sprintf(tmp, "GVA screen initalised (%dx%d)", width_, height_);
    logGva::log (tmp, LOG_INFO);

    // Initalise the pasert for NMEA
    nmea_zero_INFO(&info_);
    nmea_parser_init(&parser_);
    
    // Open File Descriptor
    gps_ = open( screen->info.gpsDevice, O_RDWR| O_NONBLOCK | O_NDELAY );
    if (gps_ > 0) {
      sprintf(tmp, "GPS Opened %s", screen->info.gpsDevice);
      logGva::log (tmp, LOG_INFO); 
    } else {
      sprintf(tmp, "GPS Error Opening device %s", screen->info.gpsDevice);
      logGva::log (tmp, LOG_ERROR); 
    }
    tcgetattr(gps_, &settings);

    //
    // Set Baud Rate
    //
    cfsetospeed(&settings, B4800); // baud rate 
    tcsetattr(gps_, TCSANOW, &settings); // apply the settings

    //
    // Start the Real Time Clock
    // 
    StartClock (screen_->StatusBar);
  }

  ScreenGva::~ScreenGva ()
  {
    args_->active = false;
    pthread_join (clock_thread_, 0);
    free (args_);
    nmea_parser_destroy(&parser_);
    close(gps_);
    if (gps_) logGva::log ("GPS closed", LOG_INFO);
    logGva::log ("GVA screen finalized.", LOG_INFO);
    logGva::finish ();
  }

  void *
  ClockUpdate (void *arg)
  {
    args *a = (args *) arg;
    time_t t;
    struct tm *tm;
    char c;
    char buffer[MAX_NMEA] = {0};
    char tmp[MAX_NMEA] = {0};

    while (a->active)
    {
      int i,ii = 0;
      t = time (NULL);
      tm = localtime (&t);
      sprintf (a->clockString, "%02d/%02d/%02d %02d:%02d:%02d", tm->tm_mday,
               tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min,
               tm->tm_sec);
      if (*a->gps > 0) 
      {
        i=0;
        tcflush(*a->gps,TCIOFLUSH);
        read(*a->gps, &buffer[0], 1);

        memset(buffer, 0, MAX_NMEA);
        while (buffer[0]!='$') read(*a->gps, &buffer[0], 1);
        while (buffer[i++]!='\n') {
          ii = read(*a->gps, &c, 1);
          if (ii==1) {
             buffer[i]=c;
//             printf("0x%02x ", c);
           }
          if (i==MAX_NMEA) break;
        }
        buffer[i-1]=0;
        sprintf(tmp, "GPS NMEA %s", buffer);
        logGva::log (tmp, LOG_INFO);

        sprintf(tmp, "%s\r\n", buffer);
        a->info->lon = a->location->lon;
        a->info->lat = a->location->lat;
        nmea_parse(a->parser, tmp, (int)strlen(tmp), a->info);
        a->info->lat = toDegrees(a->info->lat);
        a->info->lon = toDegrees(a->info->lon);
      }
      
      if ( a->info->lon && a->info->lat)
      {
        switch (a->location->locationFormat) {
        case LOCATION_FORMAT_LONG_LAT:
          break;
          sprintf(a->locationFormat, "LONLAT");
          sprintf(a->locationString, "Lat:%05.06f Lon:%05.06f [%d,%d]", a->info->lat,
                a->info->lon, a->info->sig, a->info->fix);
        case LOCATION_FORMAT_MGRS:
          {
            int zone;
            bool northp;
            double x, y;
            UTMUPS::Forward(a->info->lat, a->info->lon, zone, northp, x, y);
            string mgrs;
            MGRS::Forward(zone, northp, x, y, a->info->lat, 5, mgrs);
            sprintf(a->locationFormat, "MGRS");
            sprintf(a->locationString, "%s", mgrs.c_str());
          }
          break;
        }
      }        
      usleep (1000000);
    }
  }

  void
  ScreenGva::StartClock (StatusBarType * barData)
  {
    pthread_t clock_thread_;
    args_ = (args *) malloc (sizeof (args));
    args_->active = true;
    args_->clockString = barData->labels[0];
    args_->locationFormat = barData->labels[1];
    args_->locationString = barData->labels[2];
    args_->parser = &parser_;
    args_->info = &info_;
    args_->gps = &gps_;
    args_->screen = this;
    args_->location = &barData->location;
    args_->info->lon = DUMMY_LON;
    args_->info->lat = DUMMY_LAT;
    
    /* Launch clock thread */
    if (pthread_create (&clock_thread_, NULL, ClockUpdate, (void *) args_))
      {
        logGva::log ("Error creating thread", LOG_ERROR);
        return;
      }
    logGva::log ("Internal clock thread started", LOG_INFO);
  }

  int
  ScreenGva::Update ()
  {
    char *texture = 0;

    // Reset the Drawing context, must be Reset before reDrawing the screen
    Reset();
    GetTouch()->Reset();

    // Draw the background canvas first
    switch (screen_->canvas.bufferType) {
      case SURFACE_CAIRO:
        TextureRGB (0, 0, screen_->canvas.surface);
        break;
      case SURFACE_FILE:
        TextureRGB (0, 0, screen_->canvas.buffer);
        TextureRGB (0, 0, texture, screen_->canvas.filename);
        break;
      default:
      case SURFACE_NONE:
        // Set background green
        SetColourForground (GREEN);
        SetColourBackground (GREEN);
        DrawRectangle (0, 0, width_, height_, true);
        break;
      case SURFACE_BLACKOUT:
        // Set background black
        SetColourForground (BLACK);
        SetColourBackground (BLACK);
        DrawRectangle (0, 0, width_, height_, true);
        break;
    }

    // If BLACKOUT then nothing to render
    if (screen_->info.mode == MODE_BLACKOUT) {
      //
      // Refersh display
      //
      Draw ();
      last_screen_ = *screen_;
      return GVA_SUCCESS;
    }

    // Draw label
    if (screen_->label.visible) {
      DrawLabel (screen_->label.x, screen_->label.y, screen_->label.text, screen_->label.fontSize);
    }

    // Draw the LEFT bezel labels
    if (screen_->functionLeft.visible) {
      DrawFunctionLabels (1, screen_->functionLeft.active,
                        screen_->functionLeft.hidden,
                        screen_->functionLeft.toggleActive,
                        screen_->functionLeft.toggleOn,
                        screen_->functionLeft.labels);
    }

    // Draw the RIGHT bezel labels
    if (screen_->functionRight.visible) {
      DrawFunctionLabels (DEFAULT_WIDTH - 100 - 1,
                        screen_->functionRight.active,
                        screen_->functionRight.hidden,
                        screen_->functionRight.toggleActive,
                        screen_->functionRight.toggleOn,
                        screen_->functionRight.labels);
    }

    // Draw the TOP bezel labels     
    if (screen_->functionTop->visible) {
      DrawTopLabels (DEFAULT_HEIGHT-11, screen_->functionTop->active,
                  screen_->functionTop->hidden);
    }

    // Draw the maintinance mode indicator 
    if (screen_->info.mode == MODE_MAINTINENCE) {
      DrawMode ();
    }
    
    // Draw the onscreen KEYBOARD
    if (widgets_->keyboard.GetVisible()) {
      DrawKeyboard(widgets_->keyboard.mode_);
    }
    
    // Setup and Draw the status bar, one row table
    if (screen_->StatusBar->visible) {
      int i = 0;
      // Setup and Draw the status bar, one row table
      int widths[7] = { 23, 8, 37, 8, 8, 8, 8 };
      GvaTable table(1, screen_->StatusBar->y , 640);
      GvaRow newrow;

      for (i=0;i<7;i++) {
        cellAlignType align = ALIGN_LEFT;
        if (i==1) align = ALIGN_CENTRE;
        GvaCellType cell = {screen_->StatusBar->labels[i], align, { WHITE }, { DARK_GREEN }, { WHITE }, WEIGHT_BOLD };
        newrow.addCell(cell, widths[i]);
      }
      table.AddRow(newrow);
      DrawTable (&table);
    }

    // TODO : Draw the alarms if any (Mock up)
    if (widgets_->alarmIndicator.GetVisible()) {
      GvaTable table(102, widgets_->alarmIndicator.GetY(), 436);
      GvaRow alarmrow;
      GvaCellType cell = {widgets_->alarmIndicator.text_, ALIGN_CENTRE, { WHITE }, { RED }, { WHITE }, WEIGHT_NORMAL };
      
      table.border_ = 0;
      alarmrow.addCell(cell, 100);
      table.AddRow(alarmrow);
      DrawTable (&table);
    }

    // Setup and Draw the alarms
    if (screen_->table.visible_) {
	  GvaTable table(screen_->table.x_, screen_->table.y_, screen_->table.width_);
	  table.border_ = 1;
	  for (int row=0; row<screen_->table.row_count_; row++) {
        GvaRow newrow;
        RgbUnpackedType f, b, o;
        for (int cell=0;cell<screen_->table.rows_[row].cell_count; cell++) {
		  f = UnpackRgb(screen_->table.rows_[row].cells[cell].foreground_colour);
	   	  b = UnpackRgb(screen_->table.rows_[row].cells[cell].background_colour);
	   	  
	   	  // Choose colour for cell border
	   	  if (screen_->table.rows_[row].highlighted == false) {
    	    o = UnpackRgb(screen_->table.rows_[row].cells[cell].outline_colour);
		  } else {
		    o = UnpackRgb(screen_->table.rows_[row].cells[cell].highlight_colour);
		  }

          newrow.addCell({screen_->table.rows_[row].cells[cell].text, screen_->table.rows_[row].cells[cell].alignment, 
			             o.r, o.g, o.b, // Outline
			             b.r, b.g, b.b, // Background
			             f.r, f.g, f.b, // Foreground
			             screen_->table.rows_[row].font_weight }, screen_->table.rows_[row].cells[cell].width);
        }
        table.AddRow(newrow);
	  }

      DrawTable (&table);
    }

    if (widgets_->compass.GetVisible()) {
      DrawPPI (widgets_->compass.GetX(), widgets_->compass.GetY(), widgets_->compass.bearing_, widgets_->compass.bearingSight_);
    }
    
    if (screen_->control->visible) {
		DrawControlLabels (0, screen_->control->active,
                       screen_->control->hidden);
    }

    // Generic message box
    if (screen_->message.visible) {
      char tmp[2][MAX_TEXT];
      GvaTable table(320-150, 260, 300);
      GvaRow newrow;
      GvaRow newrow1;
      
      table.border_ = 2;

      strcpy(tmp[0], screen_->message.brief.text);
      newrow.addCell({tmp[0], ALIGN_CENTRE, { WHITE }, { DARK_GREEN }, { WHITE }, WEIGHT_BOLD }, 100);
      table.AddRow(newrow);      
          
      strcpy(tmp[1], screen_->message.detail.text);
      newrow1.addCell({tmp[1], ALIGN_CENTRE, { WHITE }, { DARK_GREEN }, { WHITE }, WEIGHT_NORMAL }, 100);
      table.AddRow(newrow1);
      DrawTable (&table);

      DrawIcon(screen_->message.icon, 320-150 + 300 -17 , 270 ,11,11);
    }
    
    //
    // Refersh display
    //
    Draw ();
    last_screen_ = *screen_;
  }
  
  char 
  *PosDegrees(float lon, float lat)
  {
    
  }

  Keyboard::Keyboard() {  
    // Initalise keyboard widget, hidden on creation
    mode_ = KEYBOARD_UPPER;
    SetVisible(false);
  }
  
}
