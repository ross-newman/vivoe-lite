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

// @file renderer_gva.cc
// @author ross@rossnewman.com
// @date 15 August 2019
// @brief Rendering functions for all the complex Widgets needed by the HMI. Widgets are reusable components of the HMI.
//
#include <math.h>               /* sqrt */
#include "debug.h"
#include "renderer_gva.h"

#define PLOT_CIRCLE_X(x, radius, degree) x + (radius) * cos(((M_PI * 2) / 360) * degree)
#define PLOT_CIRCLE_Y(y, radius, degree) y - (radius) * sin(((M_PI * 2) / 360) * degree)
#define degreesToRadians(angleDegrees) ((angleDegrees) * M_PI / 180.0)
#define radiansToDegrees(angleRadians) ((angleRadians) * 180.0 / M_PI)

int GvaRow::addCell(GvaCellType newcell, int width) {
  widths_[cells_] = width;
  cell_[cells_++] = newcell;
  return cells_;
};

RendererGva::RendererGva(int width, int height)
:RendererCairo(height, width) {
	touch_.SetResolution(width, height);
}

void FunctionKeySimple::Draw(RendererGva * r, int x, int y, int width, int height,
                         char *text) {
  char copy[256];
  char delim[] = ".";
  char *ptr = NULL;

  r->DrawRectangle(x, y, x + width, y + height, true);
  r->DrawColor(WHITE);

  if (!strncmp(text, "icon:", 5)) {
	if (!strcmp(&text[5], "exit"))
      r->DrawIcon(ICON_POWER_OFF, x + width/2, y + height/2,20,20);
	if (!strcmp(&text[5], "uparrow"))
      r->DrawIcon(ICON_UP_ARROW, x + width/2, y + height/2,20,20);
	if (!strcmp(&text[5], "downarrow"))
      r->DrawIcon(ICON_DOWN_ARROW, x + width/2, y + height/2,20,20);
	if (!strcmp(&text[5], "rightarrow"))
      r->DrawIcon(ICON_RIGHT_ARROW, x + width/2, y + height/2,20,20);
	if (!strcmp(&text[5], "leftarrow"))
      r->DrawIcon(ICON_LEFT_ARROW, x + width/2, y + height/2,20,20);
	if (!strcmp(&text[5], "rotateleft"))
      r->DrawIcon(ICON_ROTATE_LEFT, x + width/2, y + height/2,20,20);
	if (!strcmp(&text[5], "rotateright"))
      r->DrawIcon(ICON_ROTATE_RIGHT, x + width/2, y + height/2,20,20);
	if (!strcmp(&text[5], "plus"))
      r->DrawIcon(ICON_PLUS, x + width/2, y + height/2,20,20);
	if (!strcmp(&text[5], "minus"))
      r->DrawIcon(ICON_MINUS, x + width/2, y + height/2,20,20);
  } else {
    strncpy(copy, text, 40);
    ptr = strtok(copy, delim);
    if (ptr != NULL) {
      r->DrawText(x + 4, y + 30, ptr, 14);
      ptr = strtok(NULL, delim);
      if (ptr != NULL)
        r->DrawText(x + 4, y + 10, ptr, 14);
    } else {
      r->DrawText(x + 4, y + 30, text, 14);
    }
  }
  
  x_ = x;
  y_ = y;
}

void FunctionKeyToggle::toggle(RendererGva * r, char *label1, char *label2) {
  r->SetColourForground(DARK_GREEN2);
  r->SetColourBackground(YELLOW);
  r->DrawRectangle(GetX() + 5, GetY() + 5, GetX() + 45, GetY() + 25, true);
  r->DrawColor(BLACK);
  r->DrawText(GetX() + 12, GetY() + 9, label1, 14);
  r->SetColourBackground(GREY);
  r->SetColourForground(DARK_GREY);
  r->DrawRectangle(GetX() + 50, GetY() + 5, GetX() + 95, GetY() + 25, true);
  r->DrawColor(BLACK);
  r->DrawText(GetX() + 56, GetY() + 9, label2, 14);
}

void RendererGva::DrawFunctionLabels(int x, int active, int hide,
                                     int toggle, int toggleOn, 
                                     char labels[6][40]) 
{
  int i = 0;
  int offset = DEFAULT_HEIGHT - 88;

  SetColourForground(gva::configuration.GetThemeLabelBorderActive());
  SetColourBackground(gva::configuration.GetThemeLabelBackgroundActive());
  setLineType(CAIRO_LINE_JOIN_ROUND);
  SetLineThickness(2, LINE_SOLID);
  SetTextFont((int) CAIRO_FONT_SLANT_NORMAL,
              (int) CAIRO_FONT_WEIGHT_NORMAL, "Courier");

  int firstKey = (x < DEFAULT_WIDTH / 2) ? KEY_F1 : KEY_F7;
  int group = (x < DEFAULT_WIDTH / 2) ? LEFT : RIGHT;

  for (i = 0; i < 6; i++) {
    SetLineThickness(2, LINE_SOLID);
    SetColourBackground(gva::configuration.GetThemeLabelBackgroundActive());
    if ((1 << (5 - i) & hide)) {
      (1 << (5 - i) & active) ? SetColourForground(gva::configuration.GetThemeLabelBorderSelected()) :
        SetColourForground(gva::configuration.GetThemeLabelBorderActive());
      FunctionKeyToggle *key = new FunctionKeyToggle();

      key->Draw(this, x, offset - (i * 72), 100, 50, labels[i]);
      touch_.Add(group, (int) (firstKey + i), x, offset - (i * 72), 100, 50);

      if ((1 << (5 - i) & toggle))
        key->toggle(this, "On", "Off");
    }
  }
}

void RendererGva::DrawTopLabels(int y, int active, int hide) {
  int i = 0;
  int offset = DEFAULT_WIDTH * 0.02;
  int width = (DEFAULT_WIDTH - offset * 2) / 8;
  int spacing = width * 0.1;

  SetColourForground(gva::configuration.GetThemeLabelBorderActive());
  SetColourBackground(gva::configuration.GetThemeLabelBackgroundActive());
  setLineType(CAIRO_LINE_JOIN_ROUND);
  SetLineThickness(2, LINE_SOLID);

  for (i = 0; i < 8; i++) {
    if (!(1 << (7 - i) & hide)) {
      (1 << (7 - i) & active) ? SetColourForground(gva::configuration.GetThemeLabelBorderSelected()) :
        SetColourForground(gva::configuration.GetThemeLabelBorderActive());
      DrawRectangle((i * width) + offset, y,
                    (i * width) + width - spacing + offset, y + 10, true);
      touch_.AddAbsolute(TOP, (int) (KEY_SA + i), (i * width) + offset, y,
                         (i * width) + width - spacing + offset, y + 10);
    }
  }
}

void RendererGva::DrawControlLabels(int y, int active, int hide) {
  int i = 0;
  int offset = 20;
  int w = 75;

  char labels[8][80] =
    { "Up", "Alarms", "Threats", "Ack", "", "", "Labels", "Enter" };

  SetColourForground(gva::configuration.GetThemeLabelBorderActive());
  SetColourBackground(gva::configuration.GetThemeLabelBackgroundActive());
  setLineType(CAIRO_LINE_JOIN_ROUND);
  SetLineThickness(2, LINE_SOLID);
  SetTextFont((int) CAIRO_FONT_SLANT_NORMAL,
              (int) CAIRO_FONT_WEIGHT_BOLD, "Courier");

  for (i = 0; i < 8; i++) {
    SetLineThickness(2, LINE_SOLID);
    if ((1 << (7 - i) & hide)) {
      SetColourBackground(gva::configuration.GetThemeLabelBackgroundInactive());
      SetColourForground(gva::configuration.GetThemeLabelBorderInactive());
    } else {
      SetColourBackground(gva::configuration.GetThemeLabelBackgroundActive());
      (1 << (7 - i) & active) ? SetColourForground(gva::configuration.GetThemeLabelBorderSelected()) :
        SetColourForground(gva::configuration.GetThemeLabelBorderActive());
    }
    DrawRectangle((i * w) + offset, y, (i * w) + w - 5 + offset, y + 20, true);

    (1 << (7 - i) & hide) ? DrawColor(gva::configuration.GetThemeLabelTextInactive()) : 
      DrawColor(gva::configuration.GetThemeLabelTextActive());
    touch_.AddAbsolute(BOTTOM, (int) (KEY_F13 + i), (i * w) + offset, y,
                       (i * w) + w - 5 + offset, y + 20);
    DrawText((i * w) + offset + 5, y + 6, labels[i], 12);
    if (i == 4)
      DrawIcon(ICON_UP_ARROW, (i * w) + offset + 34, y + 11, 15, 8);
    if (i == 5)
      DrawIcon(ICON_DOWN_ARROW, (i * w) + offset + 34, y + 10, 15, 8);
  }
}

void RendererGva::DrawIcon(IconType icon, int x, int y, int width, int height) {
  double sx, sy;
  int arrow[8][2] = { {-5, -10},
  {-4, -10},
  {-4, 0},
  {-8, 0},
  {0, +10},
  {8, 0},
  {+4, 0},
  {+4, -10}
  };

  DrawColor(WHITE);
  SetColourBackground(WHITE);
  SetColourForground(WHITE);
  SetLineThickness(1, LINE_SOLID);

  sx = (width / (double) 13);
  sy = (height / (double) 15);

  Save();
  Translate(x, y);

  switch (icon) {
    case ICON_DOWN_ARROW:
      Rotate(M_PI);
    case ICON_UP_ARROW:
      Scale(sx, sy);
      MovePenRaw(arrow[0][0], arrow[0][1]);
      for (int i = 1; i < 8; i++) {
        DrawPenRaw(arrow[i][0], arrow[i][1]);
      }
      ClosePath(true);
      break;    
    case ICON_DOWN_ARROW_OUTLINE:
      Rotate(M_PI);
    case ICON_UP_ARROW_OUTLINE:
      Scale(sx, sy);
      MovePenRaw(arrow[0][0], arrow[0][1]);
      for (int i = 1; i < 8; i++) {
        DrawPenRaw(arrow[i][0], arrow[i][1]);
      }
      ClosePath(false);
      break;
    case ICON_RIGHT_ARROW:
      Rotate(M_PI);
    case ICON_LEFT_ARROW:
      Rotate(M_PI*1.5);
      Scale(sx, sy);
      MovePenRaw(arrow[0][0], arrow[0][1]);
      for (int i = 1; i < 8; i++) {
        DrawPenRaw(arrow[i][0], arrow[i][1]);
      }
      ClosePath(true);
      break;
    case ICON_RIGHT_ARROW_OUTLINE:
      Rotate(M_PI);
    case ICON_LEFT_ARROW_OUTLINE:
      Rotate(M_PI*1.5);
      Scale(sx, sy);
      MovePenRaw(arrow[0][0], arrow[0][1]);
      for (int i = 1; i < 8; i++) {
        DrawPenRaw(arrow[i][0], arrow[i][1]);
      }
      ClosePath(false);
      break;
    case ICON_POWER_OFF:
      SetLineThickness(2, LINE_SOLID, LINE_CAP_ROUND);
      Scale(sx, sy);
      DrawArcRaw(0, 0, 8, 290, 250);
      MovePenRaw(0, -4);
      DrawPenRaw(0, -10);
      ClosePath(false);
      break;
    case ICON_ROTATE_LEFT:
      SetLineThickness(2, LINE_SOLID, LINE_CAP_ROUND);
      Scale(sx, sy);
      DrawArcRaw(0, 0, 8, 315, 225);
      MovePenRaw(5, -6);
      DrawPenRaw(5, -1);
      MovePenRaw(5, -6);
      DrawPenRaw(9, -6);
      ClosePath(false);
      break;
    case ICON_ROTATE_RIGHT:
      SetLineThickness(2, LINE_SOLID, LINE_CAP_ROUND);
      Scale(sx, sy);
      DrawArcRaw(0, 0, 8, 315, 225);
      MovePenRaw(-5, -6);
      DrawPenRaw(-5, -1);
      MovePenRaw(-5, -6);
      DrawPenRaw(-9, -6);
      ClosePath(false);
      break;
    case ICON_PLUS:
      SetLineThickness(2, LINE_SOLID, LINE_CAP_ROUND);
      Scale(sx, sy);
      MovePenRaw(-10, 0);
      DrawPenRaw(10, 0);
      MovePenRaw(0, -10);
      DrawPenRaw(0, 10);
      ClosePath(false);
      break;
    case ICON_MINUS:
      SetLineThickness(2, LINE_SOLID, LINE_CAP_ROUND);
      Scale(sx, sy);
      MovePenRaw(-10, 0);
      DrawPenRaw(10, 0);
      ClosePath(false);
      break;
    case ICON_INFO :
    case ICON_ERROR :
    case ICON_WARNING :
      SetLineThickness(2, LINE_SOLID);
      if (icon == ICON_INFO) SetColourBackground(GREEN);
      if (icon == ICON_ERROR) SetColourBackground(RED);
      if (icon == ICON_WARNING) SetColourBackground(ORANGE);
      Scale(sx, sy);
      MovePenRaw(-10, -10);
      DrawPenRaw(0, +10);
      DrawPenRaw(+10, -10);
      DrawPenRaw(-10, -10);
      ClosePath(true);
      DrawColor(WHITE);
      MovePenRaw(0, +3);
      DrawPenRaw(0, -3);
      ClosePath(false);
      MovePenRaw(0, -6);
      DrawPenRaw(0, -7);
      ClosePath(false);
      break;
  }
  Restore();
}

void RendererGva::DrawPPI(int x, int y, int degrees, int sightAzimuth) {
  int radius = 50;
  int angle = 45;
  float d;

  DrawColor(WHITE);
  /* Degrees north */
  degrees += 270;
  degrees = (degrees >= 360) ? degrees - 360 : degrees;
  sightAzimuth += 270;
  sightAzimuth = (sightAzimuth >= 360) ? sightAzimuth - 360 : sightAzimuth;
  d = degrees;

  // Compass
  SetColourBackground(BLACK);
  SetColourForground(WHITE);
  SetLineThickness(1, LINE_SOLID);
  DrawCircle(x, y, radius, true);       // Compass
  DrawCircle(x, y, 8, true);    // Compass

  // Vehicle outline
  Save();
  SetColourForground(WHITE);
  SetColourBackground(WHITE);
  SetLineThickness(2, LINE_SOLID);
  MovePen(x - 15, y - 20);
  DrawPen(x + 15, y - 20, false);
  DrawPen(x + 15, y + 20, false);
  DrawPen(x + 5, y + 20, false);
  DrawPen(x + 5, y + 15, false);
  DrawPen(x - 5, y + 15, false);
  DrawPen(x - 5, y + 20, false);
  DrawPen(x - 15, y + 20, false);
  DrawPen(x - 15, y - 20, true);
//  ClosePath(false);
  Restore();

  // Compass Markings
  SetTextFont((int) CAIRO_FONT_SLANT_NORMAL,
              (int) CAIRO_FONT_WEIGHT_BOLD, "Courier");
  d = degreesToRadians(d);
  int pos = 6;

  DrawText(x - 3 + (radius - pos) * cos(d + (M_PI * 2)),
           y - 2 - (radius - pos) * sin(d + (M_PI * 2)), "N", 10);
  DrawText(x - 3 + (radius - pos) * cos(d + (M_PI)),
           y - 2 - (radius - pos) * sin(d + (M_PI)), "S", 10);
  DrawText(x - 3 + (radius - pos) * cos(d + (M_PI / 2)),
           y - 2 - (radius - pos) * sin(d + (M_PI / 2)), "E", 10);
  DrawText(x - 3 + (radius - pos) * cos(d + (M_PI + M_PI / 2)),
           y - 2 - (radius - pos) * sin(d + (M_PI + M_PI / 2)), "W", 10);

  SetLineThickness(1, LINE_SOLID);
  float step = (M_PI * 2) / 32;
  int p = 20;
  int c = 0;

  d = degrees;
  for (d = degreesToRadians(degrees);
       d <= degreesToRadians(degrees) + (M_PI * 2); d += step) {
    p = c % 4 ? 14 : 10;
    c++;
    MovePen(x + (radius - 21) * cos(d), y - (radius - 21) * sin(d));
    DrawPen(x + (radius - p) * cos(d), y - (radius - p) * sin(d), true);
  }

  // Sight 
  SetLineThickness(2, LINE_SOLID);
  SetColourBackground(WHITE);
  SetColourForground(WHITE);
  {
    int x2, y2;

    x2 = PLOT_CIRCLE_X(x, radius - 10, sightAzimuth);
    y2 = PLOT_CIRCLE_Y(y, radius - 10, sightAzimuth);
    MovePen(x, y);
    DrawPen(x2, y2, true);
    SetLineThickness(1, LINE_DASHED);
    x2 = PLOT_CIRCLE_X(x, radius - 10, (sightAzimuth - (angle / 2)));
    y2 = PLOT_CIRCLE_Y(y, radius - 10, (sightAzimuth - (angle / 2)));
    MovePen(x, y);
    DrawPen(x2, y2, true);
    SetLineThickness(1, LINE_DASHED);
    x2 = PLOT_CIRCLE_X(x, radius - 10, (sightAzimuth + (angle / 2)));
    y2 = PLOT_CIRCLE_Y(y, radius - 10, (sightAzimuth + (angle / 2)));
    MovePen(x, y);
    DrawPen(x2, y2, true);
  }

  // Heading 
  SetLineThickness(1, LINE_SOLID);
  SetColourBackground(CYAN);
  SetColourForground(CYAN);
  DrawRectangle(x - 1, y + 8, x + 1, y + 35, true);
}

void RendererGva::DrawMode() {
  int offset = DEFAULT_WIDTH * 0.4;
  int y = DEFAULT_HEIGHT * 0.08;

  SetColourForground(WHITE);
  SetColourBackground(DARK_BLUE);
  SetLineThickness(1, LINE_SOLID);

  SetTextFont((int) CAIRO_FONT_SLANT_NORMAL,
              (int) CAIRO_FONT_WEIGHT_NORMAL, "Courier");

  int w = GetTextWidth("Maintinance Mode", 12);
  int h = GetTextHeight("Maintinance Mode", 12);

  DrawRectangle(DEFAULT_WIDTH / 2 - (w / 2) - 5, y,
                DEFAULT_WIDTH / 2 + (w / 2) + 10, y + (h) + 15, true);
  DrawText(DEFAULT_WIDTH / 2 - (w / 2), y + 8, "Maintinance Mode", 12);
}

void RendererGva::DrawTable(GvaTable * table) {
  int height = 20;
  int row = 0;
  int column = 0;
  int columns;

  SetLineThickness(table->border_, LINE_SOLID);
  SetTextFont((int) CAIRO_FONT_SLANT_NORMAL,
              (int) CAIRO_FONT_WEIGHT_NORMAL, table->fontname_);

  for (row = 0; row < table->rows_; row++) {
    int offset = table->GetX();

    for (column = 0; column < table->row_[row].cells_; column++) {
      int pos = 0;
      int tmp =
        table->row_[row].widths_[column] * ((double) table->GetWidth() / 100);

      SetTextFont((int) CAIRO_FONT_SLANT_NORMAL,
                  (int) table->row_[row].cell_[column].weight == WEIGHT_BOLD ?
                  CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL,
                  table->fontname_);

      SetColourForground(table->row_[row].cell_[column].foreground.red,
                         table->row_[row].cell_[column].foreground.green,
                         table->row_[row].cell_[column].foreground.blue);
      SetColourBackground(table->row_[row].cell_[column].background.red,
                          table->row_[row].cell_[column].background.green,
                          table->row_[row].cell_[column].background.blue);
      DrawRectangle(offset, table->GetY() - (height * row), offset + tmp,
                    table->GetY() - (height * row) + height, true);

      DrawColor(table->row_[row].cell_[column].textcolour.red,
                table->row_[row].cell_[column].textcolour.green,
                table->row_[row].cell_[column].textcolour.blue);


      int w = GetTextWidth(table->row_[row].cell_[column].text, 12);
      int h = GetTextHeight(table->row_[row].cell_[column].text, 12);

      switch (table->row_[row].cell_[column].align) {
        case ALIGN_CENTRE:
          pos = offset + (tmp / 2 - w / 2);
          break;
        case ALIGN_RIGHT:
          pos = offset + (tmp - w - 4);
          break;
        case ALIGN_LEFT:
        default:
          pos = offset + 4;
          break;
      }
      DrawText(pos, table->GetY() - (height * row) + 6,
               table->row_[row].cell_[column].text, 12);
      offset += tmp;
    }
  }
}

void
  RendererGva::DrawButton(char *keyText, int fontSize, int x, int y, int size) {
  DrawButton(keyText, fontSize, x, y, size, size, ALIGN_LEFT);
}

void
 RendererGva::DrawButton(char *keyText, int fontSize, int x, int y, int width,
                         int height, int align) {
  int textX = 6;

  SetColourForground(GREY);
  DrawRoundedRectangle(x, y, width, height, 6, true);
  SetColourForground(WHITE);
  SetTextFont((int) CAIRO_FONT_SLANT_NORMAL,
              (int) CAIRO_FONT_WEIGHT_BOLD, "Courier");
  int textHeight = GetTextHeight("qh", fontSize);
  int textWidth = GetTextWidth(keyText, fontSize);

  DrawColor(WHITE);
  if (align == ALIGN_CENTRE)
    textX = (width / 2) - (textWidth / 2);
  DrawText(x + textX, y + (height - textHeight - 4), keyText, fontSize);
};

void RendererGva::DrawKeyboard(KeyboardModeType mode) {
  int i = 0;
  int yLocation = 30;
  int bSize = 33;
  int padding = 5;
  int fontSize = 14;
  char keyText[5];
  char keyboard[3][10];

  SetColourForground(MEDIUM_GREY);
  SetColourBackground(MEDIUM_GREY);
  SetLineThickness(3, LINE_SOLID);

  switch (mode) {
    case KEYBOARD_UPPER:
      memcpy(keyboard, upperKeys_, sizeof(keyboard));
      break;
    case KEYBOARD_LOWER:
      memcpy(keyboard, lowerKeys_, sizeof(keyboard));
      break;
    case KEYBOARD_NUMBERS:
      memcpy(keyboard, numKeys_, sizeof(keyboard));
      break;
  }

//  DrawRoundedRectangle (110, yLocation, 530,
//                 yLocation + padding + ((bSize + 5) * 4) + 1, 10, true);
  DrawRectangle(110, yLocation, 530,
                yLocation + padding + ((bSize + 5) * 4) + 1, true);
  SetColourBackground(DARK_GREY);
  SetLineThickness(1, LINE_SOLID);
  SetTextFont((int) CAIRO_FONT_SLANT_NORMAL,
              (int) CAIRO_FONT_WEIGHT_BOLD, "Courier");

  // Draw keys
  SetColourForground(WHITE);
  DrawColor(WHITE);
  for (i = 0; i < 10; i++) {
    sprintf(keyText, "%c", keyboard[0][i]);
    DrawButton(keyText, fontSize, 125 + (i * (bSize + 5)),
               yLocation + padding + (bSize + 5) * 3, bSize);
  }
  for (i = 0; i < 9; i++) {
    sprintf(keyText, "%c", keyboard[1][i]);
    DrawButton(keyText, fontSize, 140 + (i * (bSize + 5)),
               yLocation + padding + (bSize + 5) * 2, bSize);
  }
  for (i = 0; i < 8; i++) {
    sprintf(keyText, "%c", keyboard[2][i]);
    DrawButton(keyText, fontSize, 160 + (i * (bSize + 5)),
               yLocation + padding + (bSize + 5) * 1, bSize);
  }
  DrawIcon(ICON_LEFT_ARROW, 426+bSize/2, yLocation + padding + (bSize + 5) + bSize/2, 8, 10);
  SetColourBackground(DARK_GREY);
  SetLineThickness(1, LINE_SOLID);
  SetColourForground(WHITE);

  //
  // Space Bar and Mode
  //
  DrawButton("123", fontSize, 144, yLocation + 5, bSize + 5, bSize,
             ALIGN_RIGHT);
  DrawButton("SPACE", fontSize, 185, yLocation + 5, bSize + 202, bSize,
             ALIGN_CENTRE);
  DrawButton ("", fontSize, 426, yLocation + 5, bSize, bSize, ALIGN_RIGHT);
  DrawIcon(ICON_UP_ARROW, 426+bSize/2, yLocation + 5 + bSize/2+2, 12, 11);
  SetColourBackground(DARK_GREY);
  SetLineThickness(1, LINE_SOLID);
  SetColourForground(WHITE);
  DrawButton("Mode", fontSize, 463, yLocation + 20, 50, 50, ALIGN_RIGHT);
}
