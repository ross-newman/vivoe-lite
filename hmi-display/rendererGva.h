#ifndef RENDERER_GVA_H
#define RENDERER_GVA_H

#include "rendererCairo.h"

typedef enum
{
  KEYBOARD_LOWER = 0,
  KEYBOARD_UPPER,
  KEYBOARD_NUMBERS
} keyboardModeType;

class rendererGva : public rendererCairo
{
public:  
  rendererGva(int width, int height);
  void drawFunctionKeys(int handle, int x, int active, int hide, int toggle, int toggleOn, char labels[6][40]);
  void drawSaKeys(int handle, int y, int active, int hide);
  void drawControlKeys(int handle, int y, int active, int hide);
  void drawPPI(int handle, int x, int y, int degrees);
  void drawTable(int handle, char labels[5][80]);
  void drawMode(int handle);
  void drawKeyboard(int handle, keyboardModeType mode);
private:
  int m_width;
  int m_height;
  char m_upperKeys[3][10] = { { 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P' }, { 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', '-' }, { 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '-', '-', '-' } };
  char m_lowerKeys[3][10] = { { 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p' }, { 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '-' }, { 'z', 'x', 'c', 'v', 'b', 'n', 'm', '-', '-', '-' } };
  char m_numKeys[3][10] = { { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' }, { '-', '_', '.', ',', ';', ':', '"', '-', '-', '-' }, { '!', '@', '#', '$', '%', '^', '&', '-', '-', '-' } };
};

class FunctionKeySimple
{
public:
  void draw(rendererGva *r, int hndl, int x, int y, char* text);
  int getX() { return m_x; };
  int getY() { return m_y; };
private: 
  int m_x; 
  int m_y;
};

class FunctionKeyToggle : public FunctionKeySimple
{
public:
  void toggle(rendererGva *r, int hndl, char* label1, char* label2);
};

#endif
