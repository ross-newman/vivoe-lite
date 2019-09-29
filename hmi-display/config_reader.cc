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
#include <iostream>
#include <fstream>
#include <string>
#include "log_gva.h"
#include "config_reader.h"

using namespace std;

#define CONFIG_FILE "./config.pb"

namespace gva {
  static config::Gva * configuration_;

   ConfigData::ConfigData() {
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    current_config_ = new config::Gva();
    {
      // Read the existing address book.
      fstream input(CONFIG_FILE, std::fstream::in | std::fstream::binary);
      if (!input) {
		char tmp[100];
        sprintf(tmp, "%s : File not found. Creating a new file.", CONFIG_FILE);
        logGva::log(tmp, LOG_INFO);
        current_config_->set_name("Test HMI configuration.");
        // Doesnt write defaults unless they have been set once
        current_config_->set_height(current_config_->height());
        current_config_->set_width(current_config_->width());
      } else if (!current_config_->ParseFromIstream(&input)) {
        logGva::log("Failed to parse config file.", LOG_INFO);
        return;
      }
    }
    {
      // Write the new config back to disk.
      fstream output(CONFIG_FILE, std::fstream::out 
                     | std::fstream::trunc | std::fstream::binary);
      if (!current_config_->SerializeToOstream(&output)) {
        logGva::log("Failed to write config file.", LOG_INFO);
        return;
      }
    }
  }

  ConfigData::~ConfigData() {

    // Write the config back to disk.
    fstream output(CONFIG_FILE, std::fstream::out | std::fstream::trunc 
                   | std::fstream::binary);
    if (!current_config_->SerializeToOstream(&output)) {
      logGva::log("Failed to update config file.", LOG_INFO);
	  return;
    }
    // Log
    logGva::log("Updated configuration file", LOG_INFO);
    // Optional:  Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();
    free(configuration_);
  }

  int ConfigData::GetZoom() {
    return current_config_->zoom();
  };

  void ConfigData::SetZoom(int zoom) {
    current_config_->set_zoom(zoom);
  }

  double ConfigData::GetTestLon() {
    return current_config_->test_lon();
  };

  void ConfigData::SetTestLon(double lon) {
    current_config_->set_test_lon(lon);
  }

  double ConfigData::GetTestLat() {
    return current_config_->test_lat();
  };

  void ConfigData::SetTestLat(double lat) {
    current_config_->set_test_lat(lat);
  }
  
  bool ConfigData::GetFullscreen() {
	  return current_config_->fullscreen();	
  }

  void ConfigData::SetFullscreen(bool fullscreen) {
	  current_config_->set_fullscreen(fullscreen);	
  }
  
  unsigned int ConfigData::GetThemeBackground(){
	  return (unsigned int)current_config_->theme().theme_background();	
  }
  
  unsigned int ConfigData::GetThemeLabelBackgroundActive() {
	  return (unsigned int)current_config_->theme().theme_label_background_active();	
  }
  
  unsigned int ConfigData::GetThemeLabelBackgroundInactive() {
	  return (unsigned int)current_config_->theme().theme_label_background_inactive();	
  }
  
  unsigned int ConfigData::GetThemeLabelTextActive() {
	  return (unsigned int)current_config_->theme().theme_label_text_active();	
  }
  
  unsigned int ConfigData::GetThemeLabelTextInactive() {
	  return (unsigned int)current_config_->theme().theme_label_text_inactive();	
  }
  
  unsigned int ConfigData::GetThemeLabelBorderSelected() {
	  return (unsigned int)current_config_->theme().theme_label_border_selected();	
  }
  
  unsigned int ConfigData::GetThemeLabelBorderActive() {
	  return (unsigned int)current_config_->theme().theme_label_border_active();	
  }
  
  unsigned int ConfigData::GetThemeLabelBorderInactive() {
	  return (unsigned int)current_config_->theme().theme_label_border_inactive();	
  }

  unsigned int ConfigData::GetThemeStatusBackground() {
	  return (unsigned int)current_config_->theme().theme_status_background();	
  }

  unsigned int ConfigData::GetThemeStatusBorder() {
	  return (unsigned int)current_config_->theme().theme_status_border();	
  }

  unsigned int ConfigData::GetThemeStatusText() {
	  return (unsigned int)current_config_->theme().theme_status_text();	
  }
 
  unsigned int ConfigData::GetThemeAlert() {
	  return (unsigned int)current_config_->theme().theme_alert();	
  }
  
  unsigned int ConfigData::GetThemeCritical() {
	  return (unsigned int)current_config_->theme().theme_critical();	
  } 

  char* ConfigData::GetThemeFont() {
	  return (char*)current_config_->theme().theme_font().c_str();;	
  }
}
