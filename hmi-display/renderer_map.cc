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
: m_width(width), m_height(height), m_map(map), m_style(style)
{
  osmscout::DatabaseParameter databaseParameter;
  osmscout::DatabaseRef       database(new osmscout::Database(databaseParameter));
  m_mapService = (osmscout::MapServiceRef)(new osmscout::MapService(database));

  if (!database->Open(map.c_str())) {
    logGva::log("Cannot open libosmscout database", LOG_ERROR);
  }

  m_styleConfig = (osmscout::StyleConfigRef)new osmscout::StyleConfig(database->GetTypeConfig());

  if (!m_styleConfig->Load(style)) {
    logGva::log("Cannot open libosmscout style", LOG_ERROR);
  }
};

rendererMap::~rendererMap()
{
  free(m_surface);
};

int 
rendererMap::project(double zoom, double lon, double lat)
{
  m_surface=cairo_image_surface_create(CAIRO_FORMAT_RGB24, m_width, m_height);

  if (m_surface!=nullptr) {
    cairo_t *cairo=cairo_create(m_surface);

    if (cairo!=nullptr) {
      osmscout::MercatorProjection  projection;
      osmscout::MapParameter        drawParameter;
      osmscout::AreaSearchParameter searchParameter;
      osmscout::MapData             data;
      osmscout::MapPainterCairo     painter(m_styleConfig);

      drawParameter.SetFontSize(3.0);
      drawParameter.SetLabelLineMinCharCount(15);
      drawParameter.SetLabelLineMaxCharCount(30);
      drawParameter.SetLabelLineFitToArea(true);
      drawParameter.SetLabelLineFitToWidth(std::min(projection.GetWidth(), projection.GetHeight()));

      /*
      std::list<std::string> paths;
      paths.push_back("./libosmscout/data/icons/14x14/standard/");
      drawParameter.SetIconMode(osmscout::MapParameter::FixedSizePixmap);
      drawParameter.SetIconPaths(paths);
      */

      projection.Set(osmscout::GeoCoord(lat,lon),
                     osmscout::Magnification(zoom),
                     DPI,
                     m_width,
                     m_height);

      std::list<osmscout::TileRef> tiles;

      m_mapService->LookupTiles(projection,tiles);
      m_mapService->LoadMissingTileData(searchParameter,*m_styleConfig,tiles);
      m_mapService->AddTileDataToMapData(tiles,data);

      if (painter.DrawMap(projection,
                          drawParameter,
                          data,
                          cairo)) {
        // Map rendered
      }

      cairo_destroy(cairo);
    }
  }
  return 0;
};
