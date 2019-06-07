#ifndef SCREEN_GVA_H
#define SCREEN_GVA_H
#include <pthread.h>
#include "rendererGva.h"
#include "logGva.h"

#define MAJOR 0
#define MINOR 0
#define PATCH 2

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
  char labels[5][80];
};

struct compassType {
  bool visible;
  int bearing;
  int bearingSight;
};

struct screenType {
  char name[100];
  functionSelectType *functionTop;
  statusBarType *statusBar;
  functionKeysType functionLeft;
  functionKeysType functionRight;
  commonTaskKeysType control;
  compassType compass;
};

class screenGva;

/*
 * These are used by the clock thread to update the time and refresh the screen 
 */
typedef struct arg_struct {
    char* clockString;
    screenGva* screen;
    bool active;
} args;

class screenGva : public rendererGva
{
public:
  screenGva(screenType *screen, int width, int height);
  ~screenGva();
  int update(screenType *screen);
  int refresh();
  void startClock(statusBarType *barData);
private:
  screenType *m_screen;
  args *m_args;
  int m_hndl;
  int m_width;
  int m_height;
  screenType m_last_screen;
  pthread_t m_clock_thread;
};
#endif
