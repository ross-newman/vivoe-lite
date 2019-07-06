#include "screen_gva.h"
#include "view_gva.h"

namespace gva
{
  viewGvaManager::viewGvaManager(statusBarType *statusBar) : m_statusBar(statusBar) { 
    m_idLast = 0; 
  }

  viewGva *
  viewGvaManager::getNewView(gvaFunctionEnum function, functionSelectType *top, 
      commonTaskKeysType *bottom, functionKeysType left, functionKeysType right) { 
    m_view[m_idLast] = new viewGva(function, top, bottom, left, right);
    return m_view[m_idLast++];
  }
  
  screenType
  viewGvaManager::getScreen(gvaFunctionEnum function) {
    int i=0;
    screenType screen = { 0 };
    for(i=0;i<m_idLast;i++) {
      if (m_view[i]->getFunction() == function) {
        screen.statusBar = m_statusBar;
        screen.functionTop = m_view[i]->getTop();
        screen.control = m_view[i]->getBottom();
        screen.functionLeft = *m_view[i]->getLeft();
        screen.functionRight = *m_view[i]->getRight();
        screen.currentFunction = function;
        return screen;
      }
    }
  }
  
  viewGva *
  viewGvaManager::getView(gvaFunctionEnum function) {
    int i=0;
    screenType screen = { 0 };
    for(i=0;i<m_idLast;i++) {
      if (m_view[i]->getFunction() == function) {
        return m_view[i];
      }
    }
  }
  
};
