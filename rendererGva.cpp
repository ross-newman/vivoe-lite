#include <math.h>       /* sqrt */
#include "debug.h"
#include "rendererGva.h"

rendererGva::rendererGva(int width, int height)
:
rendererCairo(width, height)
{
  m_height=height;
  m_width=width;
}

void
FunctionKeySimple::draw(rendererGva *r, int hndl, int x, int y, char* text)
{
  r->drawRectangle (hndl, x, y, x+100,  y+50, true);
  r->drawColor(hndl, WHITE);
  r->drawText (hndl, x+4, y+30, text, 14);
  m_x = x;
  m_y = y;
}

void
FunctionKeyToggle::toggle(rendererGva *r, int hndl, char* label1, char* label2)
{
  r->setColourForground(hndl, DARK_GREEN2);
  r->setColourBackground(hndl, YELLOW);
  r->drawRectangle (hndl, getX()+5, getY()+5, getX()+45,  getY()+25, true);
  r->drawColor(hndl, BLACK);
  r->drawText (hndl, getX()+12, getY()+9, label1, 14);
  r->setColourBackground(hndl, DARK_GREEN);
  r->drawRectangle (hndl, getX()+50, getY()+5, getX()+95,  getY()+25, true);
  r->drawText (hndl, getX()+56, getY()+9, label2, 14);
}

void
rendererGva::drawFunctionKeys(int hndl, int x, int active, int hide, char labels[6][40])
{
  int i=0;
  int offset = m_height-100;

  setColourForground(hndl, DARK_GREEN2);
  setColourBackground(hndl, DARK_GREEN);
  setLineType (hndl, CAIRO_LINE_JOIN_ROUND);
  setLineThickness(hndl, 2, LINE_SOLID);
  setTextFont(hndl, (int)CAIRO_FONT_SLANT_NORMAL, (int)CAIRO_FONT_WEIGHT_NORMAL, "Courier");

  for (i=0; i<6; i++)
  {    
    setColourBackground(hndl, DARK_GREEN);
    if ((1<<(5-i) & hide))
    { 
      (1<<(5-i) & active) ? setColourForground(hndl, YELLOW) : setColourForground(hndl, DARK_GREEN2);
      FunctionKeyToggle * key = new FunctionKeyToggle();
      key->draw (this, hndl, x, offset-(i*65), labels[i]);
      
//      if (i==5) key->toggle (this, hndl,  "On", "Off");
    }  
  }
}

void
rendererGva::drawSaKeys(int hndl, int y, int active, int hide)
{
  int i=0;
  int offset=m_width * 0.02;
  int width=(m_width-offset*2) / 8;
  int spacing=width*0.1;
  
  setColourForground(hndl, DARK_GREEN2);
  setColourBackground(hndl, DARK_GREEN);
  setLineType (hndl, CAIRO_LINE_JOIN_ROUND);
  setLineThickness(hndl, 2, LINE_SOLID);

  for (i=0; i<8; i++)
  {
    if (!(1<<(7-i) & hide))
    { 
      (1<<(7-i) & active) ? setColourBackground(hndl, YELLOW) : setColourBackground(hndl, DARK_GREEN);
      drawRectangle (hndl, (i*width)+offset, y, (i*width)+width-spacing+offset, y+10, true);
    }
  }
}

void
rendererGva::drawControlKeys(int hndl, int y, int active, int hide)
{
  int i=0;
  int offset=20;
  int w = 75;

  char labels[8][80] = { "Up", "Alarms", "Threats", "Ack", "↑", "↓", "Labels", "Enter"};

  setColourForground(hndl, DARK_GREEN2);
  setColourBackground(hndl, DARK_GREEN);
  setLineType (hndl, CAIRO_LINE_JOIN_ROUND);
  setLineThickness(hndl, 2, LINE_SOLID);
  setTextFont(hndl, (int)CAIRO_FONT_SLANT_NORMAL, (int)CAIRO_FONT_WEIGHT_BOLD, "Courier");

  for (i=0; i<8; i++)
  {
    if (!(0b10000000>>i & hide))
    { 
      (0b10000000>>i & active) ? setColourBackground(hndl, GREY) : setColourBackground(hndl, DARK_GREEN);
      (0b10000000>>i & active) ? setColourForground(hndl, DARK_GREY) : setColourForground(hndl, DARK_GREEN2);
      drawRectangle (hndl, (i*w)+offset, y, (i*w)+w-5+offset, y+20, true);
      (0b10000000>>i & active) ? drawColor(hndl, BLACK) : drawColor(hndl, WHITE);
      drawText (hndl, (i*w)+offset+5, y+6, labels[i], 12);
    }
  }
}

#define PLOT_CIRCLE_X(x, radius, degree) x + (radius) * cos(((M_PI * 2) / 360) * degree)
#define PLOT_CIRCLE_Y(y, radius, degree) y - (radius) * sin(((M_PI * 2) / 360) * degree)
void
rendererGva::drawPPI(int hndl, int x, int y, int degrees)
{
  int radius = 50;
  int angle=36;
  int sightAzimuth=45;
  float d=0;
  
  // Compass
  setColourBackground(hndl, BLACK);
  setColourForground(hndl, WHITE);
  setLineThickness(hndl, 1, LINE_SOLID);
  drawCircle (hndl, x, y, radius, true);      // Compass
  drawCircle (hndl, x, y, 8, true);      // Compass
    
  // Vehicle outline
  setColourForground(hndl, WHITE);
  setColourBackground(hndl, WHITE);
  setLineThickness(hndl, 2, LINE_SOLID);
  movePen(hndl, x-15, y-20); 
  drawPen(hndl, x+15, y-20, false); 
  drawPen(hndl, x+15, y+20, false); 
  drawPen(hndl, x+5, y+20, false); 
  drawPen(hndl, x+5, y+15, false); 
  drawPen(hndl, x-5, y+15, false); 
  drawPen(hndl, x-5, y+20, false); 
  drawPen(hndl, x-15, y+20, false); 
  drawPen(hndl, x-15, y-20, true);
  
  // Compass Markings
  setTextFont(hndl, (int)CAIRO_FONT_SLANT_NORMAL, (int)CAIRO_FONT_WEIGHT_BOLD, "Courier");
  drawText(hndl, x-3, y+39, "N", 9);
  drawText(hndl, x-3, y-46, "S", 9);
  drawText(hndl, x-46, y-4, "W", 9);
  drawText(hndl, x+40, y-4, "E", 9);

  setLineThickness(hndl, 1, LINE_SOLID);
  float step = (M_PI * 2) / 32; 
  int p = 20;
  int c = 0;
  for (d=0;d<=(M_PI * 2);d+=step) {
    p = c++ % 4 ? 17 : 13;
    movePen(hndl, x + (radius-21) * cos(d), y - (radius-21) * sin(d)); 
    drawPen(hndl, x + (radius-p) * cos(d), y - (radius-p) * sin(d), true); 
  }

  // Sight 
  setLineThickness(hndl, 2, LINE_SOLID);
  setColourBackground(hndl, WHITE);
  setColourForground(hndl, WHITE);
  {
    int x2, y2;
    x2 = PLOT_CIRCLE_X(x, radius-10, sightAzimuth);
    y2 = PLOT_CIRCLE_Y(y, radius-10, sightAzimuth);
    movePen(hndl, x, y); 
    drawPen(hndl, x2, y2, true); 
    setLineThickness(hndl, 1, LINE_DASHED);
    x2 = PLOT_CIRCLE_X(x, radius-10, (sightAzimuth-(angle/2)));
    y2 = PLOT_CIRCLE_Y(y, radius-10, (sightAzimuth-(angle/2)));
    movePen(hndl, x, y); 
    drawPen(hndl, x2, y2, true); 
    setLineThickness(hndl, 1, LINE_DASHED);
    x2 = PLOT_CIRCLE_X(x, radius-10, (sightAzimuth+(angle/2)));
    y2 = PLOT_CIRCLE_Y(y, radius-10, (sightAzimuth+(angle/2)));
    movePen(hndl, x, y); 
    drawPen(hndl, x2, y2, true); 
  }

  // Heading 
  setLineThickness(hndl, 1, LINE_SOLID);
  setColourBackground(hndl, CYAN);
  setColourForground(hndl, CYAN);
  drawRectangle (hndl, x-1, y+8, x+1, y+35, true);
}

void
rendererGva::drawMode(int hndl)
{
  int offset=m_width * 0.4;
  int y=m_height * 0.1;
  setColourForground(hndl, WHITE);
  setColourBackground(hndl, DARK_BLUE);
  setLineThickness(hndl, 2, LINE_SOLID);

  setTextFont(hndl, (int)CAIRO_FONT_SLANT_NORMAL, (int)CAIRO_FONT_WEIGHT_NORMAL, "Courier");

  int w = getTextWidth(hndl, "Maintinance Mode");
  int h = getTextHeight(hndl, "Maintinance Mode");
  
  drawRectangle (hndl, m_width/2-(w/2)-6, y, m_width/2+(w/2)+19, y+(h)+14, true);
  drawText (hndl, m_width/2-(w/2), y+8, "Maintinance Mode", 18);
}

void
rendererGva::drawTable(int hndl, char labels[5][80])
{
  int y = m_height * 0.92;
  int column=0;
  int padding=m_height * 0.02;
  int offset = padding;
  int columns = 5;
  int widths[5] = { 30, 40, 10, 10, 10 };
    
  setColourForground(hndl, WHITE);
  setColourBackground(hndl, DARK_GREEN);
  setLineThickness(hndl, 2, LINE_SOLID);
  setTextFont(hndl, (int)CAIRO_FONT_SLANT_NORMAL, (int)CAIRO_FONT_WEIGHT_NORMAL, "Courier");

  for (column=0; column<columns; column++)
  { 
  	int width = m_width-(padding*2);
	  int tmp = widths[column] * ((double)width/100); 
  
    drawRectangle (hndl, offset, y, offset+tmp, y+20, true);
    drawText (hndl, offset+4, y+6, labels[column], 12);
	  offset+=tmp;
  }
}
