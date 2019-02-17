#include <iostream>
#include <unistd.h>
#include "debug.h"
#include "pngget.h"

using namespace std;

#include "rendererGva.h"
static rendererGva *render = new rendererGva;

int
main (int argc, char *argv[])
{
  int hndl;
  resolution_type view = {640, 480, 24};
  char *lena = 0;
  png *bitmap = new png ();

  hndl = render->init (view.width, view.height);

  // Set background green
  render->setColourForground(hndl, GREEN);
  render->setColourBackground(hndl, GREEN);
  render->drawRectangle (hndl, 0, 0, view.width, view.height, true);

#if 1
  bitmap->readPngFile ("test.png");
//  bitmap->readPngFile ("lenna-lg.png");
  lena = (char *) bitmap->getRGBBufferPointer ();
  render->textureRGB (hndl, 0, 0, lena);
  free (bitmap);
#endif

  render->drawFunctionKeys(hndl, 1, 0b00000001, 0b00011100);
  render->drawFunctionKeys(hndl, view.width-100-1, 0b00100000, 0b00001111);
  render->drawSaKeys(hndl, view.height-11, 0b00001000, 0b00000010);
  render->drawTable(hndl);
  render->drawCompass(hndl, 165, 380, 0);
  
  render->drawControlKeys(hndl, 0, 0b00000100, 0b00000000);  
  
  render->drawColor(hndl, WHITE);
  render->drawTextCentre(hndl, 200, "VIVOE Lite", 12);
    
  sleep (15);
}
