#ifndef VIEW_GVA_HPP
#define VIEW_GVA_HPP
#include <string>
#include <functional>
#include "gva.hpp"
#include "screenGva.hpp"

#define MAX_LABEL 50
namespace gva
{
  struct toggleInfo {
    bool visable;
    bool rightActive;
    char rightLabel[MAX_LABEL];
    bool leftActive;
    char leftLabel[MAX_LABEL];
  };

  class labelAction {
  public:
    void bind(std::function<int(int parentId, gvaKeyEnum key)> fn) {
      privateCallback = std::bind(fn, std::placeholders::_1, std::placeholders::_2);
    }
    toggleInfo toggle;
  private:
    std::function<int(int parentId, gvaKeyEnum key)> privateCallback; // Callback
  };

  class viewGva 
  {
  public:
    viewGva(gvaFunctionEnum function, functionSelectType *top, commonTaskKeysType *bottom, functionKeysType left, functionKeysType right) :
      m_function(function), m_functionTop(top), m_commonBottom(bottom), m_functionLeft(left), m_functionRight(right) { m_valid = true; };
    bool valid() { return m_valid; };
    bool release() { m_valid = false; };
    void addToggle(gvaKeyEnum key, bool rightActive, char* rightText, bool leftActive, char* leftText) { };
    functionSelectType* getTop() { return m_functionTop; };
    commonTaskKeysType* getBottom() { return m_commonBottom; };
    functionKeysType* getLeft() { return &m_functionLeft; };
    functionKeysType* getRight() { return &m_functionRight; };
    gvaFunctionEnum getFunction() { return m_function; };
  private:
    bool m_valid = false;
    gvaFunctionEnum m_function;
    // Screen top
    functionSelectType *m_functionTop;
    // Screen bottom
    commonTaskKeysType *m_commonBottom;
    // Screen left
    functionKeysType m_functionLeft;
    labelAction *m_functionLeftAction[6] = { 0 };
    // Screen right
    functionKeysType m_functionRight;
    labelAction *m_functionRightAction[6] = { 0 };
  };
  
  typedef viewGva viewGva;
  
  class viewGvaManager  
  {
  public:
    viewGvaManager(statusBarType *statusBar);
    viewGva *getNewView(gvaFunctionEnum function, functionSelectType *top, commonTaskKeysType *bottom, functionKeysType left, functionKeysType right);
    screenType getScreen(gvaFunctionEnum function);
    viewGva *getView(gvaFunctionEnum function);
  private:
    viewGva *m_view[20];
    statusBarType *m_statusBar;
    int m_idLast;
    int m_id = 0;
  };
}

#endif