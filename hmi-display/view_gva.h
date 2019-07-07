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

#ifndef VIEW_GVA_H
#define VIEW_GVA_H
#include <string>
#include <functional>
#include "gva.h"
#include "screen_gva.h"

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
