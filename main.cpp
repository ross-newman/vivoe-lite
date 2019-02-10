#include <iostream>
#include <unistd.h>
#include "debug.h"
#include "pngget.h"

using namespace std;

int hndl;
#if 1
#include "rendererCairo.h"
static rendererCairo *render = new rendererCairo;
#else
#include "rendererSdl.h"
static rendererSdl *render = new rendererSdl;
#endif

void
eye (int x, int y)
{
  render->setColour(hndl, WHITE);
  render->fillCircle (hndl, x, y, 20);  // eye
  render->setColour(hndl, BLACK);
  render->fillCircle (hndl, x, y, 5);   // eye
  render->setColourForground (hndl, WHITE);
}

void
mouth (int x, int y)
{
  render->setColour (hndl, WHITE);
  render->fillCircle (hndl, x, y, 50);  // Mouth
  render->setColour (hndl, BLACK);
  render->fillCircle (hndl, x, y + 40, 50);     // Mouth
  render->setColourForground (hndl, WHITE);
}

int
main (int argc, char *argv[])
{
  char *lena = 0;
  png *bitmap = new png ();
  hndl = render->init (1024, 800);

  bitmap->readPngFile ("lenna-lg.png");
  lena = (char *) bitmap->getRGBBufferPointer ();
  cout << bitmap->getWidth () << "x" << bitmap->getHeight () << "\n";
  render->textureRGB (hndl, bitmap->getWidth (), bitmap->getHeight (), lena);
  free (bitmap);

  // Man 1 
  render->drawColor (hndl, WHITE);
  render->drawLine (hndl, 100, 0, 200, 200);
  render->drawColor (hndl, RED);
  render->drawLine (hndl, 200, 200, 300, 0);
  render->drawColor (hndl, GREEN);
  render->drawLine (hndl, 200, 200, 200, 400);
  render->drawColor (hndl, BLUE);
  render->drawLine (hndl, 200, 400, 100, 200);  // Arm left
  render->drawLine (hndl, 200, 400, 300, 200);  // Arm Right
  render->fillCircle (hndl, 100, 200, 10);      // Hand left
  render->fillCircle (hndl, 300, 200, 10);      // Hand Right
  mouth (220, 460);
  render->drawCircle (hndl, 220, 550, 150);     // Head
  eye (200, 600);
  eye (300, 600);


  render->scale (hndl, 0.5);
  // Man 2 
  render->drawLine (hndl, 600, 0, 600, 100);    // Right Leg
  render->drawLine (hndl, 600, 100, 700, 200);  // Right Leg
  render->drawLine (hndl, 800, 100, 800, 0);    // Left Leg
  render->drawLine (hndl, 800, 100, 700, 200);  // Left Leg
  render->drawLine (hndl, 700, 200, 700, 400);
  render->drawLine (hndl, 900, 400, 800, 500);
  render->drawLine (hndl, 800, 500, 900, 400);
  render->drawLine (hndl, 900, 400, 400, 400);
  render->fillCircle (hndl, 400, 400, 10);      // Hand left
  render->fillCircle (hndl, 800, 500, 10);      // Hand Right
  mouth (700, 500);
  render->drawCircle (hndl, 700, 550, 150);     // Head
  render->fillCircle (hndl, 800, 500, 20);      // eye
  render->fillCircle (hndl, 800, 500, 20);      // eye
  eye (600, 600);
  eye (720, 600);

  render->setColour (hndl, RED);
  render->scale (hndl, 5.0);
  render->present (hndl);
  sleep (5);
}
