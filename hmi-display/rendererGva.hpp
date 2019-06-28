#ifndef RENDERER_GVA_HPP
#define RENDERER_GVA_HPP

#include <string.h>
#include <vector> 
#include "gva.hpp"
#include "rendererCairo.hpp"

#define MAX_ROWS 50
#define MAX_CELLS 10
#define MAX_TEXT 80

typedef enum
{
  KEYBOARD_LOWER = 0,
  KEYBOARD_UPPER,
  KEYBOARD_NUMBERS
} keyboardModeType;

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
} weightType;

struct gvaColourType
{
  int red;
  int green;
  int blue;
};

class widget {
public:
  widget() { };
  widget(int x, int y) : m_x(x), m_y(y), m_width(0), m_height(0)  { };
  widget(int x, int y, int width) : m_x(x), m_y(y), m_width(width), m_height(0) { };
  widget(int x, int y, int width, int height) : m_x(x), m_y(y), m_width(width), m_height(height) { };
  int getX() { return m_x; };
  int getY() { return m_y; };
  int getWidth() { return m_width; };
  int getHeight() { return m_height; };
private:
  int m_x = 0;
  int m_y = 0;
  int m_width = 0;
  int m_height = 0;
};

typedef struct gvaCellType  {
public :
  char *text;
  cellAlignType align;
  gvaColourType foreground;
  gvaColourType background;
  gvaColourType textcolour;
  weightType weight;  
} gvaCell;

class gvaRow : public widget {
public :
  gvaRow() { };  
  gvaRow(int x, int y) : widget(x, y) {};  
  int addCell(gvaCellType newcell, int width);
  gvaCellType m_cell[MAX_CELLS];
  int m_widths[MAX_CELLS];
  int m_cells = 0;
};

class gvaTable : public widget {
public :
  gvaTable(int x, int y, int width) : widget(x, y, width) {};  
  int addRow(gvaRow newrow) { m_row[m_rows++] = newrow; return m_rows; };
  void setFontName(char *name) { strcpy(m_fontname, name); };
  void setBorderThickness(int thickness) { m_border=thickness; };
  int m_rows = 0;
  int m_border = 2;
  gvaRow m_row[MAX_ROWS];
  char m_fontname[100] = "Courier";
};

class hotspot : public widget {
public :
  hotspot(int groupId, int x, int y) : m_groupId(groupId), m_binding(0), widget(x, y) {};  
  hotspot(int groupId, int binding, int x, int y, int width, int height) : m_groupId(groupId), m_binding(binding), widget(x, y, width, height) {};  
  int getGroupId() { return m_groupId; };
  int getBinding() { return m_binding; };
private:
  int m_groupId; // Group hostpots together
  int m_binding; // Bind a value or a key to this hotspot
};

class touchGva 
{
public:
  gvaStatusTypes add(int groupId, int x, int y) { m_hotspots.push_back(hotspot(groupId, x, y)); return GVA_SUCCESS; }
  gvaStatusTypes add(int groupId, int binding, int x, int y, int width, int height) { m_hotspots.push_back(hotspot(groupId, binding, x, y, width, height));  return GVA_SUCCESS; }
  gvaStatusTypes addAbsolute(int groupId, int binding, int x, int y, int xx, int yy) { m_hotspots.push_back(hotspot(groupId, binding, x, y, xx-x, yy-y));  return GVA_SUCCESS; }
  void reset() { m_hotspots.clear(); };
  bool check(int groupId, int *binding, int x, int y) {
    y = MIN_HEIGHT - y;
    for (auto i = m_hotspots.begin(); i != m_hotspots.end(); ++i) {

      if ( (x > i->getX()) &&
           (x < ( i->getX() + i->getWidth() ) ) &&
           (y > i->getY()) &&
           (y < ( i->getY() + i->getHeight() ) ) &&
           (i->getGroupId() == groupId) ) {
        *binding = i->getBinding();
        return true;
      }
    }
  }
public:
  std::vector<hotspot> m_hotspots;
};

class rendererGva : public rendererCairo
{
public:  
  rendererGva(int width, int height);
  void drawFunctionLabels(int handle, int x, int active, int hide, int toggle, int toggleOn, char labels[6][40]);
  void drawTopLabels(int handle, int y, int active, int hide);
  void drawControlLabels(int handle, int y, int active, int hide);
  void drawPPI(int handle, int x, int y, int degrees, int sightAzimuth);
  void drawTable(int handle, gvaTable *table);
  void drawMode(int handle);
  void drawKeyboard(int handle, keyboardModeType mode);
  touchGva *getTouch() { return &m_touch; };
private:
  touchGva m_touch;
  int m_width;
  int m_height;
  char m_upperKeys[3][10] = { { 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P' }, { 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', '-' }, { 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '-', '-', '-' } };
  char m_lowerKeys[3][10] = { { 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p' }, { 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '-' }, { 'z', 'x', 'c', 'v', 'b', 'n', 'm', '-', '-', '-' } };
  char m_numKeys[3][10] = { { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' }, { '-', '_', '.', ',', ';', ':', '"', '-', '-', '-' }, { '!', '@', '#', '$', '%', '^', '&', '-', '-', '-' } };
};

class FunctionKeySimple
{
public:
  void draw(rendererGva *r, int hndl, int x, int y, int width, int height, char* text);
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
