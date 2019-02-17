#ifndef RENDERER_GVA_H
#define RENDERER_GVA_H

#include "rendererCairo.h"

class rendererGva : public rendererCairo
{
public:  
  void drawFunctionKeys(int handle, int x, int active, int hide);
  void drawSaKeys(int handle, int y, int active, int hide);
  void drawControlKeys(int handle, int y, int active, int hide);
  void drawCompass(int handle, int x, int y, int degrees);
  void drawTable(int handle);
};

class FunctionKeySimple
{
  public:
    static void draw(rendererGva *r, int hndl, int x, int y, char* text);
};

#endif
