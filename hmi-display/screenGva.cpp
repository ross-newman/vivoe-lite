#include <GeographicLib/LambertConformalConic.hpp> 
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
#include "screenGva.h"
#include "logGva.h"

using namespace GeographicLib; 

#define MAX_NMEA 1000

float toDegrees(float lon) {
  int d = (int)lon/100;
  float m = lon - d*100;
  return d+m/(float)60;
}

namespace gva
{
  screenGva::screenGva (screenType * screen, int width, int height)
  : rendererGva (width, height)
  {
    m_screen = screen;
    m_width = width;
    m_height = height;
    m_hndl = init (m_width, m_height);
    update (m_screen);
    char tmp[100];
    struct termios settings;

    sprintf(tmp, "GVA screen initalised (%dx%d)", m_width, m_height);
    logGva::log (tmp, LOG_INFO);

    /* Initalise the pasert for NMEA */
    nmea_zero_INFO(&m_info);
    nmea_parser_init(&m_parser);
    
    /* Open File Descriptor */
    m_gps = open( screen->gpsDevice, O_RDWR| O_NONBLOCK | O_NDELAY );
    if (m_gps > 0) {
      sprintf(tmp, "GPS Opened %s", screen->gpsDevice);
      logGva::log (tmp, LOG_INFO); 
    } else {
      sprintf(tmp, "GPS Error Opening device %s", screen->gpsDevice);
      logGva::log (tmp, LOG_ERROR); 
    }
    tcgetattr(m_gps, &settings);

    /* Set Baud Rate */
    cfsetospeed(&settings, B4800); /* baud rate */
    tcsetattr(m_gps, TCSANOW, &settings); /* apply the settings */
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

        if (*a->gps > 0) {
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
          a->info->lon = 0;
          a->info->lat = 0;
          nmea_parse(a->parser, tmp, (int)strlen(tmp), a->info);
          if ( a->info->lon && a->info->lat)
          {
            a->info->lat = toDegrees(a->info->lat);
            a->info->lon = toDegrees(a->info->lon);

          sprintf(a->locationString, "Lat:%05.06f Lon:%05.06f [%d,%d]", a->info->lat,
                  a->info->lon, a->info->sig, a->info->fix);
          }
        }
        usleep (100000);
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

    /* Launch clock thread */
    if (pthread_create (&m_clock_thread, NULL, clockUpdate, (void *) m_args))
      {
        logGva::log ("Error creating thread", LOG_ERROR);
        return;
      }
    logGva::log ("Internal clock thread started", LOG_INFO);
  }

  int
  screenGva::update (screenType * screen)
  {
    char *texture = 0;

    /* Reset the drawing context, must be reset before redrawing the screen */
    reset ();

    // Set background green
    setColourForground (m_hndl, GREEN);
    setColourBackground (m_hndl, GREEN);
    drawRectangle (m_hndl, 0, 0, m_width, m_height, true);

#if 1
    if (m_screen->canvas.visible)
      {
        if (m_screen->canvas.buffer) {
          textureRGB (m_hndl, 0, 0, m_screen->canvas.buffer);
        } else {
          textureRGB (m_hndl, 0, 0, texture, m_screen->canvas.filename);
        }
      }
#endif

    if (m_screen->functionLeft.visible)
      {
        drawFunctionKeys (m_hndl, 1, m_screen->functionLeft.active,
                          m_screen->functionLeft.hidden,
                          m_screen->functionLeft.toggleActive,
                          m_screen->functionLeft.toggleOn,
                          m_screen->functionLeft.labels);
      }
    if (m_screen->functionRight.visible)
      {
        drawFunctionKeys (m_hndl, m_width - 100 - 1,
                          m_screen->functionRight.active,
                          m_screen->functionRight.hidden,
                          m_screen->functionRight.toggleActive,
                          m_screen->functionRight.toggleOn,
                          m_screen->functionRight.labels);
      }
#if 1
    drawSaKeys (m_hndl, m_height - 11, m_screen->functionTop->active,
                m_screen->functionTop->hidden);
#endif
    if (m_screen->keyboard.visible) {
      drawKeyboard (m_hndl, m_screen->keyboard.mode);
    }

    drawMode (m_hndl);

    if (m_screen->statusBar->visible)
      {
        drawTable (m_hndl, m_screen->statusBar->labels);
      }

    if (m_screen->compass.visible)
      {
        drawPPI (m_hndl, 165, 380, 0);
      }

    if (m_screen->control.visible)
      {
        drawControlKeys (m_hndl, 0, m_screen->control.active,
                         m_screen->control.hidden);
      }

    /*
     *  Refersh display
     */
    draw (m_hndl);
    m_last_screen = *screen;
  }
  
  char 
  *posDegrees(float lon, float lat)
  {
    
  }

  int
  screenGva::refresh ()
  {
    XClientMessageEvent dummyEvent;

    /*
     * Send a dummy event to force screen update 
     */
    memset (&dummyEvent, 0, sizeof (XClientMessageEvent));
    dummyEvent.type = Expose;
    dummyEvent.window = *getWindow ();
    dummyEvent.format = 32;
    XSendEvent (getDisplay (), *getWindow (), False, ExposureMask,
                (XEvent *) & dummyEvent);
    XFlush (getDisplay ());
  }
}
