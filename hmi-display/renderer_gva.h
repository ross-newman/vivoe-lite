// 
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

#ifndef RENDERER_GVA_H
#define RENDERER_GVA_H

#include <string.h>
#include <vector> 
#include "gva.h"
#include "renderer_cairo.h"

#define MAX_ROWS 50
#define MAX_CELLS 10
#define MAX_TEXT 80

typedef enum
{
  KEYBOARD_LOWER = 0,
  KEYBOARD_UPPER,
  KEYBOARD_NUMBERS
} KeyboardModeType;

typedef enum
{
  ALIGN_LEFT = 0,
  ALIGN_CENTRE,
  ALIGN_RIGHT
} cellAlignType;

typedef enum
{
  WEIGHT_NORMAL = 0,
  WEIGHT_BOLD,
  WEIGHT_ITALIC
} WeightType;

typedef enum {
  ICON_NONE = 0,
  ICON_UP_ARROW,
  ICON_DOWN_ARROW,
  ICON_LEFT_ARROW,
  ICON_RIGHT_ARROW,
  ICON_UP_ARROW_OUTLINE,
  ICON_DOWN_ARROW_OUTLINE,
  ICON_LEFT_ARROW_OUTLINE,
  ICON_RIGHT_ARROW_OUTLINE,
  ICON_PLUS,
  ICON_MINUS,
  ICON_ENTER,
  ICON_ROTATE_LEFT,
  ICON_ROTATE_RIGHT,
  ICON_POWER_OFF,
  ICON_LOCATION,
  ICON_WARNING,
  ICON_ERROR,
  ICON_INFO,
  ICON_CENTRE
} IconType;

struct GvaColourType_
{
  int red;
  int green;
  int blue;
};

class Widget {
public:
  Widget() { };
  Widget(int x, int y) : m_x(x), m_y(y), m_width(0), m_height(0)  { };
  Widget(int x, int y, int width) : m_x(x), m_y(y), m_width(width), m_height(0) { };
  Widget(int x, int y, int width, int height) : m_x(x), m_y(y), m_width(width), m_height(height) { };
  int GetX() { return m_x; };
  int GetY() { return m_y; };
  int GetWidth() { return m_width; };
  int GetHeight() { return m_height; };
private:
  int m_x = 0;
  int m_y = 0;
  int m_width = 0;
  int m_height = 0;
};

typedef struct GvaCellType  {
public :
  char *text;
  cellAlignType align;
  GvaColourType_ foreground;
  GvaColourType_ background;
  GvaColourType_ textcolour;
  WeightType weight;  
} GvaCell;

class GvaRow : public Widget {
public :
  GvaRow() { };  
  GvaRow(int x, int y) : Widget(x, y) {};  
  int addCell(GvaCellType newcell, int width);
  GvaCellType cell_[MAX_CELLS];
  int widths_[MAX_CELLS];
  int cells_ = 0;
};

class GvaTable : public Widget {
public :
  GvaTable(int x, int y, int width) : Widget(x, y, width) {};  
  int AddRow(GvaRow newrow) { row_[rows_++] = newrow; return rows_; };
  void SetFontName(char *name) { strcpy(fontname_, name); };
  void SetBorderThickness(int thickness) { border_=thickness; };
  int rows_ = 0;
  int border_ = 2;
  GvaRow row_[MAX_ROWS];
  char fontname_[100] = "Courier";
};

class Hotspot : public Widget {
public :
  Hotspot(int groupId, int x, int y) : group_id_(groupId), binding_(0), Widget(x, y) {};  
  Hotspot(int groupId, int binding, int x, int y, int width, int height) : group_id_(groupId), binding_(binding), Widget(x, y, width, height) {};  
  int GetGroupId() { return group_id_; };
  int GetBinding() { return binding_; };
private:
  int group_id_; // Group hostpots together
  int binding_; // Bind a value or a key to this Hotspot
};

class TouchGva 
{
public:
  GvaStatusTypes Add(int groupId, int x, int y) { hotspots_.push_back(Hotspot(groupId, x, y)); return GVA_SUCCESS; }
  GvaStatusTypes Add(int groupId, int binding, int x, int y, int width, int height) { hotspots_.push_back(Hotspot(groupId, binding, x, y, width, height));  return GVA_SUCCESS; }
  GvaStatusTypes AddAbsolute(int groupId, int binding, int x, int y, int xx, int yy) { hotspots_.push_back(Hotspot(groupId, binding, x, y, xx-x, yy-y));  return GVA_SUCCESS; }
  void SetResolution(int x, int y) { x_=x; y_=y; };
  void Reset() { hotspots_.clear(); };
  bool Check(int groupId, int *binding, int x, int y) {
    // Adjust for resized windows
    x = x / (float)(Renderer::GetWidth() / (float)DEFAULT_WIDTH); 
    y = y / (float)(Renderer::GetHeight() / (float)DEFAULT_HEIGHT);
    // Invert now adjusted for size
    y = MIN_HEIGHT - y;
    
    for (auto i = hotspots_.begin(); i != hotspots_.end(); ++i) {

      if ( (x > i->GetX()) &&
           (x < ( i->GetX() + i->GetWidth() ) ) &&
           (y > i->GetY()) &&
           (y < ( i->GetY() + i->GetHeight() ) ) &&
           (i->GetGroupId() == groupId) ) {
        *binding = i->GetBinding();
        return true;
      }
    }
  }
public:
  int x_;
  int y_;
  std::vector<Hotspot> hotspots_;
};

class RendererGva : public RendererCairo {
public:  
  RendererGva(int width, int height);
  void DrawLabels(char * text, int fontSize, int x, int y);
  void DrawFunctionLabels(int x, int active, int hide, int toggle, int toggleOn, char labels[6][40]);
  void DrawTopLabels(int y, int active, int hide);
  void DrawControlLabels(int y, int active, int hide);
  void DrawIcon (IconType icon, int x, int y, int width, int height);
  void DrawPPI(int x, int y, int degrees, int sightAzimuth);
  void DrawTable(GvaTable *table);
  void DrawMode();
  void DrawButton (char *keytext, int fontSize, int x, int y, int size);
  void DrawButton (char *keytext, int fontSize, int x, int y, int height, int width, int align);
  void DrawKeyboard(KeyboardModeType mode);
  TouchGva *GetTouch() { return &touch_; };
private:
  TouchGva touch_;
  char upperKeys_[3][10] = { { 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P' }, { 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', '-' }, { 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ' ', '-', '-' } };
  char lowerKeys_[3][10] = { { 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p' }, { 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '-' }, { 'z', 'x', 'c', 'v', 'b', 'n', 'm', ' ', '-', '-' } };
  char numKeys_[3][10] = { { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' }, { '-', '_', '.', ',', ';', ':', '"', '-', '-', '-' }, { '!', '@', '#', '$', '%', '^', '&', ' ', '-', '-' } };
};

class FunctionKeySimple
{
public:
  void Draw(RendererGva *r, int x, int y, int width, int height, char* text);
  int GetX() { return x_; };
  int GetY() { return y_; };
private: 
  int x_; 
  int y_;
};

class FunctionKeyToggle : public FunctionKeySimple
{
public:
  void toggle(RendererGva *r, char* label1, char* label2);
};

#endif
