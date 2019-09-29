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
#ifndef CONFIG_READER_H
#define CONFIG_READER_H
#include "config.pb.h"

namespace gva {
  class ConfigData {
  public:
    ConfigData();
    ~ConfigData();
    int GetZoom();
    void SetZoom(int zoom);
    double GetTestLon();
    void SetTestLon(double lon);
    double GetTestLat();
    void SetTestLat(double lat);
    bool GetFullscreen();
    void SetFullscreen(bool fullscreen);
    unsigned int GetThemeBackground();
    unsigned int GetThemeLabelBackgroundActive();
    unsigned int GetThemeLabelBackgroundInactive();
    unsigned int GetThemeLabelTextActive();
    unsigned int GetThemeLabelTextInactive();
    unsigned int GetThemeLabelBorderSelected();
    unsigned int GetThemeLabelBorderActive();
    unsigned int GetThemeLabelBorderInactive();
    unsigned int GetThemeStatusBackground();
    unsigned int GetThemeStatusBorder();
    unsigned int GetThemeStatusText();
    unsigned int GetThemeAlert();
    unsigned int GetThemeCritical();
    char* GetThemeFont();
  private:
    config::Gva * current_config_;
  };
  
  // Static configuration object
  static ConfigData configuration;
};
#endif
