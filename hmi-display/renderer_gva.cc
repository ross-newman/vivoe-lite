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
// @date 04 July 2019
// @brief Rendering functions for all the complex widgets needed by the HMI. Widgets are reusable components of the HMI.
//
#include <math.h>               /* sqrt */
#include "debug.h"
#include "renderer_gva.h"

#define PLOT_CIRCLE_X(x, radius, degree) x + (radius) * cos(((M_PI * 2) / 360) * degree)
#define PLOT_CIRCLE_Y(y, radius, degree) y - (radius) * sin(((M_PI * 2) / 360) * degree)
#define degreesToRadians(angleDegrees) ((angleDegrees) * M_PI / 180.0)
#define radiansToDegrees(angleRadians) ((angleRadians) * 180.0 / M_PI)

int 
gvaRow::addCell(gvaCellType newcell, int width) 
{
  m_widths[m_cells] = width;
  m_cell[m_cells++] = newcell; 
  return m_cells; 
};

rendererGva::rendererGva (int width, int height):
rendererCairo (width, height)
{
  m_height = height;
  m_width = width;
}

void
FunctionKeySimple::draw (rendererGva * r, int x, int y, int width, int height, char *text)
{
  char copy[256];
  char delim[] = ".";
  char *ptr = NULL;

  r->drawRectangle (x, y, x + width, y + height, true);
  r->drawColor (WHITE);

  strncpy (copy, text, 40);
  ptr = strtok (copy, delim);
  if (ptr != NULL)
    {
      r->drawText (x + 4, y + 30, ptr, 14);
      ptr = strtok (NULL, delim);
      if (ptr != NULL)
        r->drawText (x + 4, y + 10, ptr, 14);
    }
  else
    {
      r->drawText (x + 4, y + 30, text, 14);
    }

  m_x = x;
  m_y = y;
}

void
FunctionKeyToggle::toggle (rendererGva * r, char *label1,
                           char *label2)
{
  r->setColourForground (BLACK);
  r->setColourBackground (YELLOW);
  r->drawRectangle (getX () + 5, getY () + 5, getX () + 45,
                    getY () + 25, true);
  r->drawColor (BLACK);
  r->drawText (getX () + 12, getY () + 9, label1, 14);
  r->setColourBackground (GREY);
  r->setColourForground (DARK_GREY);
  r->drawRectangle (getX () + 50, getY () + 5, getX () + 95,
                    getY () + 25, true);
  r->drawColor (BLACK);
  r->drawText (getX () + 56, getY () + 9, label2, 14);
}

void
rendererGva::drawFunctionLabels (int x, int active, int hide,
                               int toggle, int toggleOn, char labels[6][40])
{
  int i = 0;
  int offset = m_height - 100;

  setColourForground (DARK_GREEN2);
  setColourBackground (DARK_GREEN);
  setLineType (CAIRO_LINE_JOIN_ROUND);
  setLineThickness (2, LINE_SOLID);
  setTextFont ((int) CAIRO_FONT_SLANT_NORMAL,
               (int) CAIRO_FONT_WEIGHT_NORMAL, "Courier");
               
  int firstKey = (x<DEFAULT_WIDTH/2) ? KEY_F1 : KEY_F7;
  int group = (x<DEFAULT_WIDTH/2) ? LEFT : RIGHT;
  for (i = 0; i < 6; i++)
    {
      setColourBackground (DARK_GREEN);
      if ((1 << (5 - i) & hide))
        {
          (1 << (5 - i) & active) ? setColourForground (YELLOW) :
            setColourForground (DARK_GREEN2);
          FunctionKeyToggle *key = new FunctionKeyToggle ();
          key->draw (this, x, offset - (i * 65), 100, 50, labels[i]);
          m_touch.add(group, (int)(firstKey + i),  x, offset - (i * 65), 100, 50);

          if ((1 << (5 - i) & toggle))
            key->toggle (this, "On", "Off");
        }
    }
}

void
rendererGva::drawTopLabels (int y, int active, int hide)
{
  int i = 0;
  int offset = m_width * 0.02;
  int width = (m_width - offset * 2) / 8;
  int spacing = width * 0.1;

  setColourForground (DARK_GREEN2);
  setColourBackground (DARK_GREEN);
  setLineType (CAIRO_LINE_JOIN_ROUND);
  setLineThickness (2, LINE_SOLID);

  for (i = 0; i < 8; i++)
    {
      if (!(1 << (7 - i) & hide))
        {
          (1 << (7 - i) & active) ? setColourBackground (YELLOW) :
            setColourBackground (DARK_GREEN);
           drawRectangle ((i * width) + offset, y,
                         (i * width) + width - spacing + offset, y + 10,
                         true);
           m_touch.addAbsolute(TOP, (int)(KEY_SA + i), (i * width) + offset, y,
                         (i * width) + width - spacing + offset, y + 10);
        }
    }
}

void
rendererGva::drawControlLabels (int y, int active, int hide)
{
  int i = 0;
  int offset = 20;
  int w = 75;

  char labels[8][80] =
    { "Up", "Alarms", "Threats", "Ack", "↑", "↓", "Labels", "Enter" };

  setColourForground (DARK_GREEN2);
  setColourBackground (DARK_GREEN);
  setLineType (CAIRO_LINE_JOIN_ROUND);
  setLineThickness (2, LINE_SOLID);
  setTextFont ((int) CAIRO_FONT_SLANT_NORMAL,
               (int) CAIRO_FONT_WEIGHT_BOLD, "Courier");

  for (i = 0; i < 8; i++) {
    if ((1 << (7 - i) & hide)) {
      setColourBackground (GREY);
      setColourForground (DARK_GREY);
    } else {
      setColourBackground (DARK_GREEN);
      (1 << (7 - i) & active) ? setColourForground (YELLOW) :
        setColourForground (DARK_GREEN2);
    }
    drawRectangle ((i * w) + offset, y, (i * w) + w - 5 + offset,
                   y + 20, true);
                   
    (1 << (7 - i) & hide) ? drawColor (BLACK) : drawColor (WHITE);
    m_touch.addAbsolute(BOTTOM, (int)(KEY_F13 + i), (i * w) + offset, y, (i * w) + w - 5 + offset, y + 20);
    drawText ((i * w) + offset + 5, y + 6, labels[i], 12);
  }
}

void
rendererGva::drawPPI (int x, int y, int degrees, int sightAzimuth)
{
  int radius = 50;
  int angle = 45;
  float d;
  
  drawColor(WHITE);
  /* Degrees north */
  degrees+=270;
  degrees = (degrees>=360) ? degrees-360: degrees;
  sightAzimuth+=270;
  sightAzimuth = (sightAzimuth>=360) ? sightAzimuth-360: sightAzimuth;
  d=degrees;

  // Compass
  setColourBackground (BLACK);
  setColourForground (WHITE);
  setLineThickness (1, LINE_SOLID);
  drawCircle (x, y, radius, true);        // Compass
  drawCircle (x, y, 8, true);     // Compass

  // Vehicle outline
  setColourForground (WHITE);
  setColourBackground (WHITE);
  setLineThickness (2, LINE_SOLID);
  movePen (x - 15, y - 20);
  drawPen (x + 15, y - 20, false);
  drawPen (x + 15, y + 20, false);
  drawPen (x + 5, y + 20, false);
  drawPen (x + 5, y + 15, false);
  drawPen (x - 5, y + 15, false);
  drawPen (x - 5, y + 20, false);
  drawPen (x - 15, y + 20, false);
  drawPen (x - 15, y - 20, true);

  // Compass Markings
  setTextFont ((int) CAIRO_FONT_SLANT_NORMAL,
               (int) CAIRO_FONT_WEIGHT_BOLD, "Courier");
  d=degreesToRadians(d);
  int pos = 6;
  drawText (x-3 + (radius - pos) * cos (d+(M_PI*2)), y-2 - (radius - pos) * sin (d+(M_PI*2)), "N", 10);
  drawText (x-3 + (radius - pos) * cos (d+(M_PI)),   y-2 - (radius - pos) * sin (d+(M_PI)), "S", 10);
  drawText (x-3 + (radius - pos) * cos (d+(M_PI/2)), y-2 - (radius - pos) * sin (d+(M_PI/2)), "E", 10);
  drawText (x-3 + (radius - pos) * cos (d+(M_PI+M_PI/2)), y-2 - (radius - pos) * sin (d+(M_PI+M_PI/2)), "W", 10);

  setLineThickness (1, LINE_SOLID);
  float step = (M_PI * 2) / 32;
  int p = 20;
  int c = 0;
  d=degrees;
  for (d=degreesToRadians(degrees); d <= degreesToRadians(degrees)+(M_PI*2); d += step)
    {
      p = c % 4 ? 14 : 10;
      c++;
      movePen (x + (radius - 21) * cos (d), y - (radius - 21) * sin (d));
      drawPen (x + (radius - p)  * cos (d), y - (radius - p)  * sin (d), true);
    }

  // Sight 
  setLineThickness (2, LINE_SOLID);
  setColourBackground (WHITE);
  setColourForground (WHITE);
  {
    int x2, y2;
    x2 = PLOT_CIRCLE_X (x, radius - 10, sightAzimuth);
    y2 = PLOT_CIRCLE_Y (y, radius - 10, sightAzimuth);
    movePen (x, y);
    drawPen (x2, y2, true);
    setLineThickness (1, LINE_DASHED);
    x2 = PLOT_CIRCLE_X (x, radius - 10, (sightAzimuth - (angle / 2)));
    y2 = PLOT_CIRCLE_Y (y, radius - 10, (sightAzimuth - (angle / 2)));
    movePen (x, y);
    drawPen (x2, y2, true);
    setLineThickness (1, LINE_DASHED);
    x2 = PLOT_CIRCLE_X (x, radius - 10, (sightAzimuth + (angle / 2)));
    y2 = PLOT_CIRCLE_Y (y, radius - 10, (sightAzimuth + (angle / 2)));
    movePen (x, y);
    drawPen (x2, y2, true);
  }

  // Heading 
  setLineThickness (1, LINE_SOLID);
  setColourBackground (CYAN);
  setColourForground (CYAN);
  drawRectangle (x - 1, y + 8, x + 1, y + 35, true);
}

void
rendererGva::drawMode ()
{
  int offset = m_width * 0.4;
  int y = m_height * 0.08;
  setColourForground (WHITE);
  setColourBackground (DARK_BLUE);
  setLineThickness (1, LINE_SOLID);

  setTextFont ((int) CAIRO_FONT_SLANT_NORMAL,
               (int) CAIRO_FONT_WEIGHT_NORMAL, "Courier");

  int w = getTextWidth ("Maintinance Mode", 12);
  int h = getTextHeight ("Maintinance Mode", 12);

  drawRectangle (m_width / 2 - (w / 2) - 5, y,
                 m_width / 2 + (w / 2) + 10, y + (h) + 15, true);
  drawText (m_width / 2 - (w / 2), y + 8, "Maintinance Mode", 12);
}

void
rendererGva::drawTable (gvaTable *table)
{
  int height = 20;
  int row = 0;
  int column = 0;
  int columns;

  setLineThickness (table->m_border, LINE_SOLID);
  setTextFont ((int) CAIRO_FONT_SLANT_NORMAL,
              (int) CAIRO_FONT_WEIGHT_NORMAL, table->m_fontname);

  for (row = 0; row< table->m_rows; row++) {
    int offset = table->getX();
    for (column = 0; column < table->m_row[row].m_cells; column++)
      {
        int pos = 0;
        int tmp = table->m_row[row].m_widths[column] * ((double)  table->getWidth() / 100);
  
        setTextFont ((int) CAIRO_FONT_SLANT_NORMAL,
                    (int) table->m_row[row].m_cell[column].weight == WEIGHT_BOLD ?
                    CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL, 
                    table->m_fontname);
  
        setColourForground (table->m_row[row].m_cell[column].foreground.red, table->m_row[row].m_cell[column].foreground.green, table->m_row[row].m_cell[column].foreground.blue);
        setColourBackground (table->m_row[row].m_cell[column].background.red, table->m_row[row].m_cell[column].background.green, table->m_row[row].m_cell[column].background.blue);
        drawRectangle (offset, table->getY() - (height * row), offset + tmp, table->getY() - (height * row) + height, true);

        drawColor (table->m_row[row].m_cell[column].textcolour.red, table->m_row[row].m_cell[column].textcolour.green, table->m_row[row].m_cell[column].textcolour.blue);
        
        
        int w = getTextWidth (table->m_row[row].m_cell[column].text, 12);
        int h = getTextHeight (table->m_row[row].m_cell[column].text, 12);
        switch (table->m_row[row].m_cell[column].align) {
        case ALIGN_CENTRE:
          pos = offset + (tmp/2 - w/2);
          break;
        case ALIGN_RIGHT:
          pos = offset + (tmp - w - 4);
          break;
        case ALIGN_LEFT:
        default:
          pos = offset + 4;
          break;
        }
        drawText (pos, table->getY() - (height * row) + 6, table->m_row[row].m_cell[column].text, 12);
        offset += tmp;
      }
    }
}

void
rendererGva::drawButton (char *keyText, int fontSize, int x, int y, int size) {
  drawButton(keyText, fontSize, x, y, size, size, ALIGN_LEFT);
}

void
rendererGva::drawButton (char *keyText, int fontSize, int x, int y, int width, int height, int align)
{
  int textX = 6;
  setColourForground (GREY);
  drawRoundedRectangle (x, y, width, height, 6, true);
  setColourForground (WHITE);
  setTextFont ( (int) CAIRO_FONT_SLANT_NORMAL,
               (int) CAIRO_FONT_WEIGHT_BOLD, "Courier");
  int textHeight = getTextHeight ("qh", fontSize);
  int textWidth = getTextWidth (keyText, fontSize);
  drawColor (WHITE);
  if (align==ALIGN_CENTRE) textX = (width / 2) - (textWidth / 2);
  drawText (x + textX, y + (height - textHeight - 4) , keyText, fontSize);  
};

void
rendererGva::drawKeyboard (keyboardModeType mode)
{
  int i = 0;
  int yLocation = 30;
  int bSize = 33;
  int padding = 5;
  int fontSize = 14;
  char keyText[5];
  char keyboard[3][10];

  setColourForground (MEDIUM_GREY);
  setColourBackground (MEDIUM_GREY);
  setLineThickness (3, LINE_SOLID);
  
  switch(mode) {
    case KEYBOARD_UPPER : 
      memcpy(keyboard, m_upperKeys, sizeof(keyboard));
      break;
    case KEYBOARD_LOWER : 
      memcpy(keyboard, m_lowerKeys, sizeof(keyboard));
      break;
    case KEYBOARD_NUMBERS : 
      memcpy(keyboard, m_numKeys, sizeof(keyboard));
      break;
    }
  
//  drawRoundedRectangle (110, 50, 420, 250, 10, true);
  drawRectangle (110, yLocation, 530,
                 yLocation + padding + ((bSize + 5) * 4) + 1, true);
  setColourBackground (DARK_GREY);
  setLineThickness (1, LINE_SOLID);
  setTextFont ((int) CAIRO_FONT_SLANT_NORMAL,
               (int) CAIRO_FONT_WEIGHT_BOLD, "Courier");

  // Draw keys
  setColourForground (GREY);
  setColourForground (WHITE);
  drawColor (WHITE);
  for (i = 0; i < 10; i++)
    {
      sprintf (keyText, "%c", keyboard[0][i]);
      drawButton (keyText, fontSize, 125 + (i * (bSize + 5)),
                            yLocation + padding + (bSize + 5) * 3, bSize);
    }
  for (i = 0; i < 9; i++)
    {
      sprintf (keyText, "%c", keyboard[1][i]);
      drawButton (keyText, fontSize, 140 + (i * (bSize + 5)),
                            yLocation + padding + (bSize + 5) * 2, bSize);
    }
  for (i = 0; i < 8; i++)
    {
      sprintf (keyText, "%c", keyboard[2][i]);
      drawButton (keyText, fontSize, 160 + (i * (bSize + 5)),
                            yLocation + padding + (bSize + 5) * 1, bSize);
    }

  //
  // Space Bar and Mode
  //
  drawButton ("123", fontSize, 144, yLocation + 5, bSize + 5, bSize, ALIGN_RIGHT);
  drawButton ("SPACE", fontSize, 185, yLocation + 5, bSize + 202, bSize, ALIGN_CENTRE);
  drawButton ("^", fontSize, 426, yLocation + 5, bSize, bSize, ALIGN_RIGHT);
  drawButton ("Mode", fontSize, 463, yLocation + 20, 50, 50, ALIGN_RIGHT);
}
