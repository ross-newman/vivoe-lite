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
#include "debug.h"
#include "config_reader.h"
#include "config.pb.h"

using namespace std;

#define CONFIG_FILE "config.pb"
namespace gva {
  static config::Gva * configuration_;
  static config::Gva::Geo geo_;

   ConfigData::ConfigData() {
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    config::Gva * config = new config::Gva();
    {
      // Read the existing address book.
      fstream input(CONFIG_FILE, std::fstream::in | std::fstream::binary);
      if (!input) {
        cout << CONFIG_FILE << ": File not found. Creating a new file." << endl;
        config->set_name("default name");
        // Doesnt write defaults unless they have been set once
        config->set_height(config->height());
        config->set_width(config->width());
      } else if (!config->ParseFromIstream(&input)) {
        cerr << "Failed to parse address book." << endl;
        return;
      }
    }
    {
      // Write the new address book back to disk.
      fstream output(CONFIG_FILE,
                     std::fstream::out | std::fstream::trunc | std::
                     fstream::binary);
      if (!config->SerializeToOstream(&output)) {
        cerr << "Failed to write address book." << endl;
        return;
      }
    }
  }

  ConfigData::~ConfigData() {
    // Optional:  Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();
    free(configuration_);
  }

  int ConfigData::GetZoom() {
    return geo_.zoom();
  };

  void ConfigData::SetZoom(int zoom) {
    geo_.set_zoom(zoom);
  }

  double ConfigData::GetTestLon() {
    return geo_.test_lon();
  };

  void ConfigData::SetTestLon(double lon) {
    geo_.set_test_lon(lon);
  }

  double ConfigData::GetTestLat() {
    return geo_.test_lat();
  };

  void ConfigData::SetTestLat(double lat) {
    geo_.set_test_lat(lat);
  }
}
