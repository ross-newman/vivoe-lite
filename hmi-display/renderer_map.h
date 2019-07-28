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

#ifndef RENDERER_MAP_H
#define RENDERER_MAP_H

#include <string>
#include <osmscout/Database.h>
#include <osmscout/MapService.h>
#include <osmscout/MapPainterCairo.h>
#include <cairo.h>

using namespace std;

class rendererMap {
public:
  rendererMap(string map, string style, int width, int height);
  ~rendererMap();
  int Project(double zoom, double lon, double lat, cairo_surface_t **surface);
  int SetHeight(int height) { height_ = height_; printf("height %d\n", height); };
  int SetWidth(int width) { width_ = width_;  printf("width %d\n", width); };
private:
  int width_;
  int height_;
  string map_;
  string style_;
  osmscout::MapServiceRef mapService_;
  osmscout::StyleConfigRef styleConfig_;
  osmscout::DatabaseParameter databaseParameter_;
  osmscout::DatabaseRef database_;
  osmscout::MercatorProjection projection_;
  osmscout::MapParameter DrawParameter_;
  osmscout::AreaSearchParameter searchParameter_;
  osmscout::MapData data_;
  std::list<osmscout::TileRef> tiles_;
  osmscout::MapPainterCairo *painter_;  
  cairo_surface_t *surface_;
  cairo_t *cairo_;
};
#endif
