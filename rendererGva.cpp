#include "debug.h"
#include "rendererGva.h"

void
FunctionKeySimple::draw(rendererGva *r, int hndl, int x, int y, char* text)
{
  r->drawRectangle (hndl, x, y, x+100,  y+50, true);
  r->drawColor(hndl, WHITE);
  r->drawText (hndl, x+7, y+30, text, 16);
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
  r->drawText (hndl, getX()+12, getY()+9, label1, 16);
  r->setColourBackground(hndl, DARK_GREEN);
  r->drawRectangle (hndl, getX()+50, getY()+5, getX()+95,  getY()+25, true);
  r->drawText (hndl, getX()+56, getY()+9, label2, 16);
}

void
rendererGva::drawFunctionKeys(int hndl, int x, int active, int hide)
{
  int i=0;
  int offset=50;
  char labels[6][80] = { "PriSight", "SecSight", "Undefine", "Front left", "Front Right", "IR" };

  setColourForground(hndl, DARK_GREEN2);
  setColourBackground(hndl, DARK_GREEN);
  setLineType (hndl, CAIRO_LINE_JOIN_ROUND);
  setLineThickness(hndl, 2);

  for (i=0; i<6; i++)
  {    
    setColourBackground(hndl, DARK_GREEN);
    if (!(1<<i & hide))
    { 
      (1<<i & active) ? setColourForground(hndl, YELLOW) : setColourForground(hndl, DARK_GREEN2);
      FunctionKeyToggle * key = new FunctionKeyToggle();
      key->draw (this, hndl, x, (i*65)+offset, labels[i]);
      if (i==0) key->toggle (this, hndl,  "On", "Off");
    }  
  }
}

void
rendererGva::drawSaKeys(int hndl, int y, int active, int hide)
{
  int i=0;
  int offset=20;

  setColourForground(hndl, DARK_GREEN2);
  setColourBackground(hndl, DARK_GREEN);
  setLineType (hndl, CAIRO_LINE_JOIN_ROUND);
  setLineThickness(hndl, 2);

  for (i=0; i<8; i++)
  {
    if (!(1<<i & hide))
    { 
      (1<<i & active) ? setColourBackground(hndl, YELLOW) : setColourBackground(hndl, DARK_GREEN);
      drawRectangle (hndl, (i*75)+offset, y, (i*75)+60+offset, y+10, true);
    }
  }
}

void
rendererGva::drawControlKeys(int hndl, int y, int active, int hide)
{
  int i=0;
  int offset=20;
  int w = 75;

  char labels[8][80] = { "UP", "DOWN", "Alarms", "Ack", "", "", "Labels", ""};

  setColourForground(hndl, DARK_GREEN2);
  setColourBackground(hndl, DARK_GREEN);
  setLineType (hndl, CAIRO_LINE_JOIN_ROUND);
  setLineThickness(hndl, 2);

  for (i=0; i<8; i++)
  {
    if (!(1<<i & hide))
    { 
      (1<<i & active) ? setColourBackground(hndl, GREY) : setColourBackground(hndl, DARK_GREEN);
      (1<<i & active) ? setColourForground(hndl, DARK_GREY) : setColourForground(hndl, DARK_GREEN2);
      drawRectangle (hndl, (i*w)+offset, y, (i*w)+w-5+offset, y+20, true);
      (1<<i & active) ? drawColor(hndl, BLACK) : drawColor(hndl, WHITE);
      drawText (hndl, (i*w)+offset+5, y+6, labels[i], 10);
    }
  }
}

void
rendererGva::drawCompass(int hndl, int x, int y, int degrees)
{
  // Compass
  setColourBackground(hndl, BLACK);
  setColourForground(hndl, WHITE);
  setLineThickness(hndl, 1);
  drawCircle (hndl, x, y, 50, true);      // Compass
  drawCircle (hndl, x, y, 3, true);      // Compass
  
  // Heading
  setColourBackground(hndl, LIGHT_BLUE);
  setColourForground(hndl, WHITE);
  drawTriangle (hndl, x-3, y, x+3, y, x, y+40, true);
}

void
rendererGva::drawTable(int hndl)
{
  int y = 443;
  int column=0;
  int padding=5;
  int offset = padding;
  int columns = 5;
  int widths[5] = { 40, 30, 10, 10, 10 };
  char labels[5][80] = { "Long:  Lat:  ", "12:30:00", "A: 0", "E: 0", "C: 0" };
  
  setColourForground(hndl, WHITE);
  setColourBackground(hndl, DARK_GREEN);
  setLineThickness(hndl, 2);

  for (column=0; column<columns; column++)
  { 
  	int width = getWidth(hndl)-padding*2;
	  int tmp = widths[column] * ((double)width/100); 
  
    drawRectangle (hndl, offset, y, offset+tmp, y+20, true);
    drawText (hndl, offset+4, y+6, labels[column], 12);
	  offset+=tmp;
  }
}
