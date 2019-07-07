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
