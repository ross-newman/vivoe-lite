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
