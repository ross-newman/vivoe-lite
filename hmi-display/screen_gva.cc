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
  screenGva::screenGva (screenType * screen, widgetsType * widgets, int width, int height)
  : rendererGva (width, height)
  {
    m_screen = screen;
    m_widgets = widgets;
    m_width = width;
    m_height = height;
    m_hndl = init (m_width, m_height);
    update ();
    char tmp[100];
    struct termios settings;

    sprintf(tmp, "GVA screen initalised (%dx%d)", m_width, m_height);
    logGva::log (tmp, LOG_INFO);

    // Initalise the pasert for NMEA
    nmea_zero_INFO(&m_info);
    nmea_parser_init(&m_parser);
    
    // Open File Descriptor
    m_gps = open( screen->info.gpsDevice, O_RDWR| O_NONBLOCK | O_NDELAY );
    if (m_gps > 0) {
      sprintf(tmp, "GPS Opened %s", screen->info.gpsDevice);
      logGva::log (tmp, LOG_INFO); 
    } else {
      sprintf(tmp, "GPS Error Opening device %s", screen->info.gpsDevice);
      logGva::log (tmp, LOG_ERROR); 
    }
    tcgetattr(m_gps, &settings);

    // Set Baud Rate
    cfsetospeed(&settings, B4800); // baud rate 
    tcsetattr(m_gps, TCSANOW, &settings); // apply the settings
  }

  screenGva::~screenGva ()
  {
    m_args->active = false;
    pthread_join (m_clock_thread, 0);
    free (m_args);
    nmea_parser_destroy(&m_parser);
    close(m_gps);
    if (m_gps) logGva::log ("GPS closed", LOG_INFO);
    logGva::log ("GVA screen finalized.", LOG_INFO);
    logGva::finish ();
  }

  void *
  clockUpdate (void *arg)
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
      a->screen->refresh ();
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
            sprintf(a->locationString, "MGRS: %s", mgrs.c_str());
          }
          break;
        }
      }        
      usleep (1000000);
    }
  }

  void
  screenGva::startClock (statusBarType * barData)
  {
    pthread_t m_clock_thread;
    m_args = (args *) malloc (sizeof (args));
    m_args->active = true;
    m_args->clockString = barData->labels[0];
    m_args->locationString = barData->labels[1];
    m_args->parser = &m_parser;
    m_args->info = &m_info;
    m_args->gps = &m_gps;
    m_args->screen = this;
    m_args->location = &barData->location;
    m_args->info->lon = DUMMY_LON;
    m_args->info->lat = DUMMY_LAT;
    
    /* Launch clock thread */
    if (pthread_create (&m_clock_thread, NULL, clockUpdate, (void *) m_args))
      {
        logGva::log ("Error creating thread", LOG_ERROR);
        return;
      }
    logGva::log ("Internal clock thread started", LOG_INFO);
  }

  int
  screenGva::update ()
  {
    char *texture = 0;

    // Reset the drawing context, must be reset before redrawing the screen
    reset ();
    getTouch()->reset();

    // Draw the background canvas first
    switch (m_screen->canvas.bufferType) {
      case SURFACE_CAIRO:
        textureRGB (0, 0, m_screen->canvas.surface);
        break;
      case SURFACE_FILE:
        textureRGB (0, 0, m_screen->canvas.buffer);
        textureRGB (0, 0, texture, m_screen->canvas.filename);
        break;
      default:
      case SURFACE_NONE:
        // Set background green
        setColourForground (GREEN);
        setColourBackground (GREEN);
        drawRectangle (0, 0, m_width, m_height, true);
        break;
    }

    // Draw label
    if (m_screen->label.visible) {
      drawLabel (m_screen->label.x, m_screen->label.y, m_screen->label.text, m_screen->label.fontSize);
    }

    // Draw the LEFT bezel labels
    if (m_screen->functionLeft.visible) {
      drawFunctionLabels (1, m_screen->functionLeft.active,
                        m_screen->functionLeft.hidden,
                        m_screen->functionLeft.toggleActive,
                        m_screen->functionLeft.toggleOn,
                        m_screen->functionLeft.labels);
    }

    // Draw the RIGHT bezel labels
    if (m_screen->functionRight.visible) {
      drawFunctionLabels (m_width - 100 - 1,
                        m_screen->functionRight.active,
                        m_screen->functionRight.hidden,
                        m_screen->functionRight.toggleActive,
                        m_screen->functionRight.toggleOn,
                        m_screen->functionRight.labels);
    }

    // Draw the TOP bezel labels     
    if (m_screen->functionTop->visible) {
      drawTopLabels (m_height-11, m_screen->functionTop->active,
                  m_screen->functionTop->hidden);
    }

    // Draw the maintinance mode indicator 
    if (m_screen->info.mode == MODE_MAINTINENCE) {
      drawMode ();
    }
    
    // Generic message box
    if (m_screen->message.visible) {
      char tmp[2][MAX_TEXT];
      gvaTable table(320-150, 260, 300);
      gvaRow newrow;
      gvaRow newrow1;
      
      table.m_border = 2;

      strcpy(tmp[0], m_screen->message.brief.text);
      newrow.addCell({tmp[0], ALIGN_CENTRE, { WHITE }, { DARK_GREEN }, { WHITE }, WEIGHT_BOLD }, 100);
      table.addRow(newrow);      

      strcpy(tmp[1], m_screen->message.detail.text);
      newrow1.addCell({tmp[1], ALIGN_CENTRE, { WHITE }, { DARK_GREEN }, { WHITE }, WEIGHT_NORMAL }, 100);
      table.addRow(newrow1);
      drawTable (&table);
    }
    
    // Draw the onscreen KEYBOARD
    if (m_widgets->keyboard.visible) {
      drawKeyboard(m_widgets->keyboard.mode);
    }
    
    // Setup and draw the status bar, one row table
    if (m_screen->statusBar->visible) {
      int i = 0;
      // Setup and draw the status bar, one row table
      int widths[6] = { 23, 45, 8, 8, 8, 8 };
      gvaTable table(1, m_screen->statusBar->y , 639);
      gvaRow newrow;

      for (i=0;i<6;i++) {
        gvaCellType cell = {m_screen->statusBar->labels[i], ALIGN_LEFT, { WHITE }, { DARK_GREEN }, { WHITE }, WEIGHT_BOLD };
        newrow.addCell(cell, widths[i]);
      }
      table.addRow(newrow);
      drawTable (&table);
    }

    // TODO : Draw the alarms if any (Mock up)
    if (m_widgets->alarmIndicator.visible) {
      gvaTable table(102, m_widgets->alarmIndicator.y, 436);
      gvaRow alarmrow;
      gvaCellType cell = {m_widgets->alarmIndicator.text, ALIGN_CENTRE, { WHITE }, { RED }, { WHITE }, WEIGHT_BOLD };
      
      table.m_border = 0;
      alarmrow.addCell(cell, 100);
      table.addRow(alarmrow);
      drawTable (&table);
    }

    // Setup and draw the alarms
    if (m_screen->alarms.visible) {
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
      gvaTable table(110, 390, 420);
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

    if (m_widgets->compass.visible) {
      drawPPI (m_widgets->compass.x, m_widgets->compass.y, m_widgets->compass.bearing, m_widgets->compass.bearingSight);
    }

    if (m_screen->control->visible) {
      drawControlLabels (0, m_screen->control->active,
                       m_screen->control->hidden);
    }

    //
    // Refersh display
    //
    draw ();
    m_last_screen = *m_screen;
  }
  
  char 
  *posDegrees(float lon, float lat)
  {
    
  }

  int
  screenGva::refresh ()
  {
    XClientMessageEvent dummyEvent;

    //
    // Send a dummy event to force screen update 
    //
    memset (&dummyEvent, 0, sizeof (XClientMessageEvent));
    dummyEvent.type = Expose;
    dummyEvent.window = *getWindow ();
    dummyEvent.format = 32;
    XSendEvent (getDisplay (), *getWindow (), False, ExposureMask,
                (XEvent *) & dummyEvent);
    XFlush (getDisplay ());
  }
}
