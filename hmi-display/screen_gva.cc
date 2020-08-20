//
// MIT License
//
// Copyright (c) 2020 Ross Newman (ross@rossnewman.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include <termios.h>
#include <time.h>
#include <unistd.h>

#include <GeographicLib/LambertConformalConic.hpp>
#include <GeographicLib/MGRS.hpp>
#include <GeographicLib/UTMUPS.hpp>
#include <iostream>
#include <string>
//#include <X11/Xlib.h>
//#include <X11/Xutil.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>

#include "debug.h"
#include "log_gva.h"
#include "screen_gva.h"

using namespace std;
using namespace GeographicLib;

#define MAX_NMEA 1000

float toDegrees(float lon) {
  int d = (int)lon / 100;
  float m = lon - d * 100;
  return d + m / (float)60;
}

namespace gva {
void Compass::Draw() {
  if (GetVisible()) {
    screen_->DrawPPI(GetX(), GetY(), bearing_, bearingSight_);
  }
}

Keyboard::Keyboard(ScreenGva *screen) : WidgetX(screen, WIDGET_TYPE_KEYBOARD) {
  // Initalise keyboard widget, hidden on creation
  mode_ = KEYBOARD_UPPER;
  SetVisible(false);
}

void Keyboard::Draw() {
  if (GetVisible()) {
    screen_->DrawKeyboard(mode_);
  }
}

void AlarmIndicator::Draw() {
  if (GetVisible()) {
    GvaTable table(104, GetY(), 432);
    table.SetFontName(gva::configuration.GetThemeFont());
    GvaRow alarmrow;
    GvaCellType cell = {text_,     ALIGN_CENTRE, {HMI_WHITE},
                        {HMI_RED}, {HMI_WHITE},  WEIGHT_NORMAL};

    table.border_ = 0;
    alarmrow.addCell(cell, 100);
    table.AddRow(alarmrow);
    screen_->DrawTable(&table);
  }
}

ScreenGva::ScreenGva(ScreenType *screen, int width, int height)
    : RendererGva(width, height) {
  screen_ = screen;

  char tmp[100];
  struct termios settings;

  sprintf(tmp, "GVA screen initalised (%dx%d)", width_, height_);
  logGva::log(tmp, LOG_INFO);

  // Initalise the pasert for NMEA
  nmea_zero_INFO(&info_);
  nmea_parser_init(&parser_);

  // Open File Descriptor
  gps_ = open(screen->info.gpsDevice, O_RDWR | O_NONBLOCK | O_NDELAY);
  if (gps_ > 0) {
    sprintf(tmp, "GPS Opened %s", screen->info.gpsDevice);
    logGva::log(tmp, LOG_INFO);
  } else {
    sprintf(tmp, "GPS Error Opening device %s", screen->info.gpsDevice);
    logGva::log(tmp, LOG_ERROR);
  }
  tcgetattr(gps_, &settings);

  //
  // Set Baud Rate
  //
  cfsetospeed(&settings, B4800);        // baud rate
  tcsetattr(gps_, TCSANOW, &settings);  // apply the settings

  //
  // Start the Real Time Clock
  //
  StartClock(screen_->status_bar);

  //
  // Setup the required widgets
  //
  widget_list_.push_back(new Compass(this));
  widget_list_.push_back(new Keyboard(this));
  widget_list_.push_back(new AlarmIndicator(this));
}

ScreenGva::~ScreenGva() {
  args_->active = false;
  pthread_join(clock_thread_, 0);
  free(args_);
  nmea_parser_destroy(&parser_);
  close(gps_);
  if (gps_) logGva::log("GPS closed", LOG_INFO);
  logGva::log("GVA screen finalized.", LOG_INFO);
  logGva::finish();
}

void *ClockUpdate(void *arg) {
  args *a = (args *)arg;
  time_t t;
  struct tm *tm;
  char c;
  char buffer[MAX_NMEA] = {0};
  char tmp[MAX_NMEA] = {0};

  while (a->active) {
    int i, ii = 0;
    t = time(NULL);
    tm = localtime(&t);
    sprintf(a->clockString, "%02d/%02d/%02d %02d:%02d:%02d", tm->tm_mday,
            tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min,
            tm->tm_sec);
    if (*a->gps > 0) {
      i = 0;
      tcflush(*a->gps, TCIOFLUSH);
      read(*a->gps, &buffer[0], 1);

      memset(buffer, 0, MAX_NMEA);
      while (buffer[0] != '$') read(*a->gps, &buffer[0], 1);
      while (buffer[i++] != '\n') {
        ii = read(*a->gps, &c, 1);
        if (ii == 1) {
          buffer[i] = c;
          //             printf("0x%02x ", c);
        }
        if (i == MAX_NMEA) break;
      }
      buffer[i - 1] = 0;
      sprintf(tmp, "GPS NMEA %s", buffer);
      logGva::log(tmp, LOG_INFO);

      sprintf(tmp, "%s\r\n", buffer);
      a->info->lon = a->location->lon;
      a->info->lat = a->location->lat;
      nmea_parse(a->parser, tmp, (int)strlen(tmp), a->info);
      a->info->lat = toDegrees(a->info->lat);
      a->info->lon = toDegrees(a->info->lon);
    }

    if (a->info->lon && a->info->lat) {
      switch (a->location->locationFormat) {
        case LOCATION_FORMAT_LONG_LAT:
          break;
          sprintf(a->locationFormat, "LONLAT");
          sprintf(a->locationString, "Lat:%05.06f Lon:%05.06f [%d,%d]",
                  a->info->lat, a->info->lon, a->info->sig, a->info->fix);
        case LOCATION_FORMAT_MGRS: {
          int zone;
          bool northp;
          double x, y;
          UTMUPS::Forward(a->info->lat, a->info->lon, zone, northp, x, y);
          string mgrs;
          MGRS::Forward(zone, northp, x, y, a->info->lat, 5, mgrs);
          sprintf(a->locationFormat, "MGRS");
          sprintf(a->locationString, "%s", mgrs.c_str());
        } break;
      }
    }
    usleep(1000000);
  }
}

void ScreenGva::StartClock(StatusBarType *barData) {
  pthread_t clock_thread_;
  args_ = (args *)malloc(sizeof(args));
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
  if (pthread_create(&clock_thread_, NULL, ClockUpdate, (void *)args_)) {
    logGva::log("Error creating thread", LOG_ERROR);
    return;
  }
  logGva::log("Internal clock thread started", LOG_INFO);
}

int ScreenGva::Update() {
  char *texture = 0;

  // Reset the Drawing context, must be Reset before reDrawing the screen
  Reset();
  GetTouch()->Reset();

  // Draw the background canvas first
  switch (screen_->canvas.bufferType) {
    case SURFACE_CAIRO:
      TextureRGB(0, 0, screen_->canvas.surface);
      break;
    case SURFACE_FILE:
      TextureRGB(0, 0, screen_->canvas.buffer);
      TextureRGB(0, 0, texture, screen_->canvas.filename);
      break;
    default:
    case SURFACE_NONE:
      // Set background green
      SetColourForground(gva::configuration.GetThemeBackground());
      SetColourBackground(gva::configuration.GetThemeBackground());
      DrawRectangle(0, 0, width_, height_, true);
      break;
    case SURFACE_BLACKOUT:
      // Set background black
      SetColourForground(HMI_BLACK);
      SetColourBackground(HMI_BLACK);
      DrawRectangle(0, 0, width_, height_, true);
      break;
  }

  // If BLACKOUT then nothing to render
  if (screen_->info.mode == MODE_BLACKOUT) {
    //
    // Refersh display
    //
    Draw();
    last_screen_ = *screen_;
    return GVA_SUCCESS;
  }

  // Draw label
  if (screen_->label.visible) {
    DrawLabel(screen_->label.x, screen_->label.y, screen_->label.text,
              screen_->label.fontSize);
  }

  // Draw the LEFT bezel labels
  if (screen_->function_left.visible) {
    DrawFunctionLabels(
        1, screen_->function_left.active, screen_->function_left.hidden,
        screen_->function_left.toggleActive, screen_->function_left.toggleOn,
        screen_->function_left.labels);
  }

  // Draw the RIGHT bezel labels
  if (screen_->function_right.visible) {
    DrawFunctionLabels(
        DEFAULT_WIDTH - 100 - 1, screen_->function_right.active,
        screen_->function_right.hidden, screen_->function_right.toggleActive,
        screen_->function_right.toggleOn, screen_->function_right.labels);
  }

  // Draw the TOP bezel labels
  if (screen_->function_top->visible) {
    DrawTopLabels(DEFAULT_HEIGHT - 11, screen_->function_top->active,
                  screen_->function_top->hidden);
  }

  // Draw the maintinance mode indicator
  if (screen_->info.mode == MODE_MAINTINENCE) {
    DrawMode();
  }

  // Draw the onscreen KEYBOARD
  widget_list_[1]->Draw();

  // Setup and Draw the status bar, one row table
  if (screen_->status_bar->visible) {
    int i = 0;
    // Setup and Draw the status bar, one row table
    int widths[7] = {23, 8, 37, 8, 8, 8, 8};
    GvaTable table(1, screen_->status_bar->y, 640);
    table.SetFontName(gva::configuration.GetThemeFont());
    GvaRow newrow;

    // Use theme colours
    GvaColourType border = {
        UnpackRed(gva::configuration.GetThemeStatusBorder()),
        UnpackGreen(gva::configuration.GetThemeStatusBorder()),
        UnpackBlue(gva::configuration.GetThemeStatusBorder())};
    GvaColourType background = {
        UnpackRed(gva::configuration.GetThemeStatusBackground()),
        UnpackGreen(gva::configuration.GetThemeStatusBackground()),
        UnpackBlue(gva::configuration.GetThemeStatusBackground())};
    GvaColourType text = {UnpackRed(gva::configuration.GetThemeStatusText()),
                          UnpackGreen(gva::configuration.GetThemeStatusText()),
                          UnpackBlue(gva::configuration.GetThemeStatusText())};

    for (i = 0; i < 7; i++) {
      cellAlignType align = ALIGN_LEFT;
      if (i == 1) align = ALIGN_CENTRE;
      GvaCellType cell = {screen_->status_bar->labels[i],
                          align,
                          border,
                          background,
                          text,
                          WEIGHT_BOLD};
      newrow.addCell(cell, widths[i]);
    }
    table.AddRow(newrow);
    DrawTable(&table);
  }

  // TODO : Draw the alarms if any (Mock up)
  widget_list_[2]->Draw();

  // Setup and Draw the alarms
  if (screen_->table.visible_) {
    GvaTable table(screen_->table.x_, screen_->table.y_, screen_->table.width_);
    table.SetFontName(gva::configuration.GetThemeFont());
    table.border_ = 1;
    for (int row = 0; row < screen_->table.row_count_; row++) {
      GvaRow newrow;
      RgbUnpackedType f, b, o;
      for (int cell = 0; cell < screen_->table.rows_[row].cell_count; cell++) {
        f = UnpackRgb(screen_->table.rows_[row].cells[cell].foreground_colour);
        b = UnpackRgb(screen_->table.rows_[row].cells[cell].background_colour);

        // Choose colour for cell border
        if (screen_->table.rows_[row].highlighted == false) {
          o = UnpackRgb(screen_->table.rows_[row].cells[cell].outline_colour);
        } else {
          o = UnpackRgb(screen_->table.rows_[row].cells[cell].highlight_colour);
        }

        newrow.addCell({screen_->table.rows_[row].cells[cell].text,
                        screen_->table.rows_[row].cells[cell].alignment, o.r,
                        o.g, o.b,       // Outline
                        b.r, b.g, b.b,  // Background
                        f.r, f.g, f.b,  // Foreground
                        screen_->table.rows_[row].font_weight},
                       screen_->table.rows_[row].cells[cell].width);
      }
      table.AddRow(newrow);
    }

    DrawTable(&table);
  }

  // Draw PPI (Plan Position Indicator)
  widget_list_[0]->Draw();

  if (screen_->control->visible) {
    DrawControlLabels(0, screen_->control->active, screen_->control->hidden);
  }

  // Generic message box
  if (screen_->message.visible) {
    char tmp[2][MAX_TEXT];
    GvaTable table(320 - 150, 260, 300);
    table.SetFontName(gva::configuration.GetThemeFont());
    GvaRow newrow;
    GvaRow newrow1;

    table.border_ = 2;

    strcpy(tmp[0], screen_->message.brief.text);
    newrow.addCell({tmp[0],
                    ALIGN_CENTRE,
                    {HMI_WHITE},
                    {HMI_DARK_GREEN},
                    {HMI_WHITE},
                    WEIGHT_BOLD},
                   100);
    table.AddRow(newrow);

    strcpy(tmp[1], screen_->message.detail.text);
    newrow1.addCell({tmp[1],
                     ALIGN_CENTRE,
                     {HMI_WHITE},
                     {HMI_DARK_GREEN},
                     {HMI_WHITE},
                     WEIGHT_NORMAL},
                    100);
    table.AddRow(newrow1);
    DrawTable(&table);

    DrawIcon(screen_->message.icon, 320 - 150 + 300 - 17, 270, 11, 11);
  }

  //
  // Refersh display
  //
  Draw();
  last_screen_ = *screen_;
}

WidgetX *ScreenGva::GetWidget(WidgetEnum widget) {
  for (int i = 0; i < widget_list_.size(); ++i) {
    // Try and match widget enum and return pointer to it
    if (widget_list_[i]->GetType() == widget) {
      return widget_list_[i];
    }
  }
}

char *PosDegrees(float lon, float lat) {}
}  // namespace gva
