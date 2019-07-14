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

    // Reset the drawing context, must be reset before redrawing the screen
    reset();
    getTouch()->reset();

    // Draw the background canvas first
    switch (screen_->canvas.bufferType) {
      case SURFACE_CAIRO:
        textureRGB (0, 0, screen_->canvas.surface);
        break;
      case SURFACE_FILE:
        textureRGB (0, 0, screen_->canvas.buffer);
        textureRGB (0, 0, texture, screen_->canvas.filename);
        break;
      default:
      case SURFACE_NONE:
        // Set background green
        setColourForground (GREEN);
        setColourBackground (GREEN);
        drawRectangle (0, 0, width_, height_, true);
        break;
    }

    // Draw label
    if (screen_->label.visible) {
      drawLabel (screen_->label.x, screen_->label.y, screen_->label.text, screen_->label.fontSize);
    }

    // Draw the LEFT bezel labels
    if (screen_->functionLeft.visible) {
      drawFunctionLabels (1, screen_->functionLeft.active,
                        screen_->functionLeft.hidden,
                        screen_->functionLeft.toggleActive,
                        screen_->functionLeft.toggleOn,
                        screen_->functionLeft.labels);
    }

    // Draw the RIGHT bezel labels
    if (screen_->functionRight.visible) {
      drawFunctionLabels (width_ - 100 - 1,
                        screen_->functionRight.active,
                        screen_->functionRight.hidden,
                        screen_->functionRight.toggleActive,
                        screen_->functionRight.toggleOn,
                        screen_->functionRight.labels);
    }

    // Draw the TOP bezel labels     
    if (screen_->functionTop->visible) {
      drawTopLabels (height_-11, screen_->functionTop->active,
                  screen_->functionTop->hidden);
    }

    // Draw the maintinance mode indicator 
    if (screen_->info.mode == MODE_MAINTINENCE) {
      drawMode ();
    }
    
    // Generic message box
    if (screen_->message.visible) {
      char tmp[2][MAX_TEXT];
      GvaTable table(320-150, 260, 300);
      gvaRow newrow;
      gvaRow newrow1;
      
      table.m_border = 2;

      strcpy(tmp[0], screen_->message.brief.text);
      newrow.addCell({tmp[0], ALIGN_CENTRE, { WHITE }, { DARK_GREEN }, { WHITE }, WEIGHT_BOLD }, 100);
      table.addRow(newrow);      

      strcpy(tmp[1], screen_->message.detail.text);
      newrow1.addCell({tmp[1], ALIGN_CENTRE, { WHITE }, { DARK_GREEN }, { WHITE }, WEIGHT_NORMAL }, 100);
      table.addRow(newrow1);
      drawTable (&table);
    }
    
    // Draw the onscreen KEYBOARD
    if (widgets_->keyboard.visible) {
      drawKeyboard(widgets_->keyboard.mode);
    }
    
    // Setup and draw the status bar, one row table
    if (screen_->StatusBar->visible) {
      int i = 0;
      // Setup and draw the status bar, one row table
      int widths[7] = { 23, 8, 37, 8, 8, 8, 8 };
      GvaTable table(1, screen_->StatusBar->y , 640);
      gvaRow newrow;

      for (i=0;i<7;i++) {
        cellAlignType align = ALIGN_LEFT;
        if (i==1) align = ALIGN_CENTRE;
        gvaCellType cell = {screen_->StatusBar->labels[i], align, { WHITE }, { DARK_GREEN }, { WHITE }, WEIGHT_BOLD };
        newrow.addCell(cell, widths[i]);
      }
      table.addRow(newrow);
      drawTable (&table);
    }

    // TODO : Draw the alarms if any (Mock up)
    if (widgets_->alarmIndicator.visible) {
      GvaTable table(102, widgets_->alarmIndicator.y, 436);
      gvaRow alarmrow;
      gvaCellType cell = {widgets_->alarmIndicator.text, ALIGN_CENTRE, { WHITE }, { RED }, { WHITE }, WEIGHT_BOLD };
      
      table.m_border = 0;
      alarmrow.addCell(cell, 100);
      table.addRow(alarmrow);
      drawTable (&table);
    }

    // Setup and draw the alarms
    if (screen_->alarms.visible) {
      int i = 0;
      int widths[6] = { 20, 50, 10, 20 };
      char row1[4][MAX_TEXT] = { "Time", "Alarm Text", "Cat", "Status"};
      char row2[4][MAX_TEXT] = { "15/6 15:06", "Low engine oil pressure", "W", "RES"};
      char row3[4][MAX_TEXT] = { "15/6 15:26", "Engine over tempreture", "W", "UNACK"};
      char row4[4][MAX_TEXT] = { "15/6 15:29", "Engine over tempreture", "W", "RES"};
      char row5[4][MAX_TEXT] = { "15/6 14:00", "Gun fault", "C", "RES"};
      char row6[4][MAX_TEXT] = { "15/6 18:16", "Air con fault", "A", "ACT"};
      char row7[4][MAX_TEXT] = { "15/6 19:03", "Gun barrel over tempreture", "C", "(OVR)ACT"};
      char row8[4][MAX_TEXT] = { "15/6 19:04", "LRU fault", "C", "ACT"};
      GvaTable table(110, 390, 420);
      gvaRow newrow;
      gvaRow newrow1;
      gvaRow newrow2;
      gvaRow newrow3;
      gvaRow newrow4;
      gvaRow newrow5;
      gvaRow newrow6;
      gvaRow newrow7;
      gvaRow newrow8;
      gvaRow newrow9;
      
      table.m_border = 1;

      for (i=0;i<4;i++) {
        newrow.addCell({row1[i], ALIGN_LEFT, { WHITE }, { DARK_GREEN }, { WHITE }, WEIGHT_NORMAL }, widths[i]);
      }
      table.addRow(newrow);

      for (i=0;i<4;i++) {
        newrow1.addCell({row2[i], ALIGN_LEFT, { WHITE }, { RED }, { WHITE }, WEIGHT_NORMAL }, widths[i]);
      }
      table.addRow(newrow1);

      for (i=0;i<4;i++) {
        newrow2.addCell({row3[i], ALIGN_LEFT, { YELLOW }, { DARK_GREEN }, { WHITE }, WEIGHT_NORMAL }, widths[i]);
      }
      table.addRow(newrow2);

      for (i=0;i<4;i++) {
        newrow3.addCell({row4[i], ALIGN_LEFT, { WHITE }, { DARK_GREEN }, { WHITE }, WEIGHT_NORMAL }, widths[i]);
      }
      table.addRow(newrow3);

      for (i=0;i<4;i++) {
        newrow4.addCell({row5[i], ALIGN_LEFT, { WHITE }, { DARK_GREEN }, { WHITE }, WEIGHT_NORMAL }, widths[i]);
      }
      table.addRow(newrow4);

      for (i=0;i<4;i++) {
        newrow5.addCell({row6[i], ALIGN_LEFT, { WHITE }, { DARK_GREEN }, { WHITE }, WEIGHT_NORMAL }, widths[i]);
      }
      table.addRow(newrow5);

      for (i=0;i<4;i++) {
        newrow6.addCell({row7[i], ALIGN_LEFT, { WHITE }, { DARK_GREEN }, { WHITE }, WEIGHT_NORMAL }, widths[i]);
      }
      table.addRow(newrow6);

      for (i=0;i<4;i++) {
        newrow7.addCell({row8[i], ALIGN_LEFT, { WHITE }, { GREY }, { WHITE }, WEIGHT_NORMAL }, widths[i]);
      }
      table.addRow(newrow7);

      for (i=0;i<4;i++) {
        newrow8.addCell({row3[i], ALIGN_LEFT, { WHITE }, { ORANGE }, { WHITE }, WEIGHT_NORMAL }, widths[i]);
      }
      table.addRow(newrow8);

//        newrow9.addCell({"Page_1_of_1", ALIGN_RIGHT, { WHITE }, { DARK_GREEN }, { WHITE }, WEIGHT_NORMAL }, 100);

      newrow9.addCell({"Page 1 of 1", ALIGN_RIGHT, { WHITE }, { DARK_GREEN }, { WHITE }, WEIGHT_NORMAL }, 100);
      table.addRow(newrow9);


      drawTable (&table);
    }

    if (widgets_->compass.visible) {
      drawPPI (widgets_->compass.x, widgets_->compass.y, widgets_->compass.bearing, widgets_->compass.bearingSight);
    }

    if (screen_->control->visible) {
      drawControlLabels (0, screen_->control->active,
                       screen_->control->hidden);
    }

    //
    // Refersh display
    //
    draw ();
    last_screen_ = *screen_;
  }
  
  char 
  *PosDegrees(float lon, float lat)
  {
    
  }
}
