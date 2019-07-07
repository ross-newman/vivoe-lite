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
#include <cairo-xlib.h>

using namespace std;

class rendererMap {
public:
  rendererMap(string map, string style, int width, int height);
  ~rendererMap();
  int project(double zoom, double lon, double lat, cairo_surface_t **surface);
private:
  int m_width;
  int m_height;
  string m_map;
  string m_style;
  osmscout::MapServiceRef m_mapService;
  osmscout::StyleConfigRef m_styleConfig;
  osmscout::DatabaseParameter m_databaseParameter;
  osmscout::DatabaseRef m_database;
  osmscout::MercatorProjection  projection;
  osmscout::MapParameter        drawParameter;
  osmscout::AreaSearchParameter searchParameter;
  osmscout::MapData             data;
  std::list<osmscout::TileRef> tiles;
  osmscout::MapPainterCairo *painter;  
  cairo_surface_t *m_surface;
  cairo_t *m_cairo;
};
#endif
