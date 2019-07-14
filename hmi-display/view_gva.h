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
  struct ToggleInfo {
    bool visable;
    bool right_active;
    char right_label[MAX_LABEL];
    bool left_active;
    char left_label[MAX_LABEL];
  };

  class labelAction {
  public:
    void Bind(std::function<int(int parentId, GvaKeyEnum key)> fn) {
      privateCallback = std::bind(fn, std::placeholders::_1, std::placeholders::_2);
    }
    ToggleInfo toggle_;
  private:
    std::function<int(int parentId, GvaKeyEnum key)> privateCallback; // Callback
  };

  class ViewGva 
  {
  public:
    ViewGva(GvaFunctionEnum function, FunctionSelectType *top, CommonTaskKeysType *bottom, FunctionKeysType left, FunctionKeysType right) :
      function_(function), function_top_(top), common_bottom_(bottom), function_left_(left), function_right_(right) { valid_ = true; };
    bool Valid() { return valid_; };
    bool Release() { valid_ = false; };
    void AddToggle(GvaKeyEnum key, bool rightActive, char* rightText, bool leftActive, char* leftText) { };
    FunctionSelectType* GetTop() { return function_top_; };
    CommonTaskKeysType* GetBottom() { return common_bottom_; };
    FunctionKeysType* GetLeft() { return &function_left_; };
    FunctionKeysType* GetRight() { return &function_right_; };
    GvaFunctionEnum GetFunction() { return function_; };
  private:
    bool valid_ = false;
    GvaFunctionEnum function_;
    // Screen top
    FunctionSelectType *function_top_;
    // Screen bottom
    CommonTaskKeysType *common_bottom_;
    // Screen left
    FunctionKeysType function_left_;
    labelAction *function_left_action_[6] = { 0 };
    // Screen right
    FunctionKeysType function_right_;
    labelAction *function_right_action_[6] = { 0 };
  };
  
  typedef ViewGva ViewGva;
  
  class ViewGvaManager  
  {
  public:
    ViewGvaManager(StatusBarType *StatusBar);
    ViewGva *GetNewView(GvaFunctionEnum function, FunctionSelectType *top, CommonTaskKeysType *bottom, FunctionKeysType left, FunctionKeysType right);
    ScreenType GetScreen(GvaFunctionEnum function);
    ViewGva *GetView(GvaFunctionEnum function);
  private:
    ViewGva *view_[20];
    StatusBarType *status_bar_;
    int idLast_;
    int id_ = 0;
  };
}

#endif
