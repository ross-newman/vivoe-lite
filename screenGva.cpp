#include "screenGva.h"

screenGva::screenGva(screenType screen, int width, int height)
:
  rendererGva(width, height)
{  
  m_screen = screen;
  m_width = width;
  m_height = height;
  m_hndl = init(m_width, m_height);
  update(m_screen);
}

int 
screenGva::update(screenType screen)
{
  char *texture = 0;
  char *bitmap = "test2.png";

  // Set background green
  setColourForground(m_hndl, GREEN);
  setColourBackground(m_hndl, GREEN);
  drawRectangle (m_hndl, 0, 0, m_width, m_height, true);

#if 1
  textureRGB (m_hndl, 0, 0, texture, bitmap);
#endif

  drawFunctionKeys(m_hndl, 1, m_screen.functionLeft.active, m_screen.functionLeft.hidden, m_screen.functionLeft.labels);
  drawFunctionKeys(m_hndl, m_width-100-1, m_screen.functionRight.active, m_screen.functionRight.hidden, m_screen.functionRight.labels);
  drawSaKeys(m_hndl, m_height-11, m_screen.functionTop.active, m_screen.functionTop.hidden);
  drawTable(m_hndl);
  drawCompass(m_hndl, 165, 380, 0);
  
  drawControlKeys(m_hndl, 0, m_screen.control.active, m_screen.control.hidden);  
  
  drawColor(m_hndl, WHITE);
  drawTextCentre(m_hndl, 200, "VIVOE Lite", 12);
  // Refersh display
 // draw(m_hndl);
}
