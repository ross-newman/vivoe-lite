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
#include <iomanip>
#include <osmscout/Database.h>
#include <osmscout/MapService.h>
#include <osmscout/MapPainterCairo.h>
#include "renderer_map.h"
#include "log_gva.h"
#include "gva.h"

#define DPI 96.0

rendererMap::rendererMap(string map, string style, int width, int height)
: width_(width), height_(height), map_(map), style_(style)
{
  database_ = (osmscout::DatabaseRef)(new osmscout::Database(databaseParameter_));
  mapService_ = (osmscout::MapServiceRef)(new osmscout::MapService(database_));

  if (!database_->Open(map_.c_str())) {
    logGva::log("Cannot open libosmscout database", LOG_ERROR);
  }

  styleConfig_ = (osmscout::StyleConfigRef)new osmscout::StyleConfig(database_->GetTypeConfig());

  if (!styleConfig_->Load(style_)) {
    logGva::log("Cannot open libosmscout style", LOG_ERROR);
  }

  surface_=cairo_image_surface_create(CAIRO_FORMAT_RGB24, width_, height_);
  if (surface_!=nullptr) {
    cairo_=cairo_create(surface_);
    logGva::log("Created libosmscout cairo surface", LOG_INFO);
  } else {
    logGva::log("Cannot create libosmscout cairo surface", LOG_ERROR);
  }
  
  DrawParameter_.SetFontSize(3.0);
  DrawParameter_.SetLabelLineMinCharCount(15);
  DrawParameter_.SetLabelLineMaxCharCount(30);
  DrawParameter_.SetLabelLineFitToArea(true);
  DrawParameter_.SetLabelLineFitToWidth(std::min(projection_.GetWidth(), projection_.GetHeight()));
  painter_ = new osmscout::MapPainterCairo(styleConfig_);
};

rendererMap::~rendererMap()
{
  cairo_destroy(cairo_);
  cairo_surface_destroy(surface_);
  free(painter_);
};

int 
rendererMap::Project(double zoom, double lon, double lat, cairo_surface_t **surface)
{

  if (surface_!=nullptr) {

    if (cairo_!=nullptr) {

      /*
      std::list<std::string> paths;
      paths.push_back("./libosmscout/data/icons/14x14/standard/");
      DrawParameter.SetIconMode(osmscout::MapParameter::FixedSizePixmap);
      DrawParameter.SetIconPaths(paths);
      */

      projection_.Set(osmscout::GeoCoord(lat,lon),
                     osmscout::Magnification(zoom),
                     DPI,
                     width_,
                     height_);


      mapService_->LookupTiles(projection_,tiles_);
      mapService_->LoadMissingTileData(searchParameter_,*styleConfig_, tiles_);
      mapService_->AddTileDataToMapData(tiles_,data_);

      if (painter_->DrawMap(projection_,
                          DrawParameter_,
                          data_,
                          cairo_)) {
        // Map rendered
      }
    }
  } else {
    return GVA_ERROR;
  }

  *surface = surface_;
  return GVA_SUCCESS;
};
