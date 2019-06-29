#ifndef RENDERER_MAP_HPP
#define RENDERER_MAP_HPP

#include <string>
#include <osmscout/Database.h>
#include <osmscout/MapService.h>
#include <osmscout/MapPainterCairo.h>

using namespace std;

class rendererMap {
public:
  rendererMap(string map, string style, int width, int height);
  ~rendererMap();
  int project(double zoom, double lon, double lat);
private:
  int m_width;
  int m_height;
  string m_map;
  string m_style;
  osmscout::MapServiceRef m_mapService;
  osmscout::StyleConfigRef m_styleConfig;
  cairo_surface_t *m_surface;
};
#endif
