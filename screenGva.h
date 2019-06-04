#ifndef SCREEN_GVA_H
#define SCREEN_GVA_H

#include "rendererGva.h"
enum functions 
{
  SA,
  WPN,
  DEF,
  SYS,
  DRV,
  STR,
  COM,
  BMS
};

struct functionSelectType {
  bool visible;
  int active;
  int hidden;
};

struct functionKeysType {
  bool visible;
  int active;
  int hidden;
  char labels[6][40];
};

struct commonTaskKeysType {
  bool visible;
  int active;
  int hidden;
  char labels[8][40];
};

struct statusBarType {
  bool visible;
  char labels[6][40];
};

struct compassType {
  bool visible;
  int bearing;
  int bearingSight;
};

struct screenType {
  char name[100];
  functionSelectType functionTop;
  statusBarType statusBar;
  functionKeysType functionLeft;
  functionKeysType functionRight;
  commonTaskKeysType control;
  compassType compass;
};

class screenGva : public rendererGva
{
public:
  screenGva(screenType *screen, int width, int height);
  int update(screenType *screen);
private:
  screenType *m_screen;
  int m_hndl;
  int m_width;
  int m_height;
};
#endif
