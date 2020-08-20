//
// MIT License
//
// Copyright (c) 2020 Ross Newman (ross@rossnewman.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include "view_gva.h"

#include "screen_gva.h"

namespace gva {
ViewGvaManager::ViewGvaManager(StatusBarType *status_bar)
    : status_bar_(status_bar) {
  idLast_ = 0;
}

ViewGva *ViewGvaManager::GetNewView(GvaFunctionEnum function,
                                    FunctionSelectType *top,
                                    CommonTaskKeysType *bottom,
                                    FunctionKeysType left,
                                    FunctionKeysType right) {
  view_[idLast_] = new ViewGva(function, top, bottom, left, right);
  return view_[idLast_++];
}

ScreenType ViewGvaManager::GetScreen(GvaFunctionEnum function) {
  int i = 0;
  ScreenType screen = {0};
  for (i = 0; i < idLast_; i++) {
    if (view_[i]->GetFunction() == function) {
      screen.status_bar = status_bar_;
      screen.function_top = view_[i]->GetTop();
      screen.control = view_[i]->GetBottom();
      screen.function_left = *view_[i]->GetLeft();
      screen.function_right = *view_[i]->GetRight();
      screen.currentFunction = function;
      return screen;
    }
  }
}

ViewGva *ViewGvaManager::GetView(GvaFunctionEnum function) {
  int i = 0;
  ScreenType screen = {0};
  for (i = 0; i < idLast_; i++) {
    if (view_[i]->GetFunction() == function) {
      return view_[i];
    }
  }
}

};  // namespace gva
