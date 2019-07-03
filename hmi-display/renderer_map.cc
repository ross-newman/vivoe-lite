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
  m_database = (osmscout::DatabaseRef)(new osmscout::Database(m_databaseParameter));
  m_mapService = (osmscout::MapServiceRef)(new osmscout::MapService(m_database));

  if (!m_database->Open(map.c_str())) {
    logGva::log("Cannot open libosmscout database", LOG_ERROR);
  }

  m_styleConfig = (osmscout::StyleConfigRef)new osmscout::StyleConfig(m_database->GetTypeConfig());

  if (!m_styleConfig->Load(style)) {
    logGva::log("Cannot open libosmscout style", LOG_ERROR);
  }

  m_surface=cairo_image_surface_create(CAIRO_FORMAT_RGB24, m_width, m_height);
  if (m_surface!=nullptr) {
    m_cairo=cairo_create(m_surface);
    logGva::log("Created libosmscout cairo surface", LOG_INFO);
  } else {
    logGva::log("Cannot create libosmscout cairo surface", LOG_ERROR);
  }
  
  drawParameter.SetFontSize(3.0);
  drawParameter.SetLabelLineMinCharCount(15);
  drawParameter.SetLabelLineMaxCharCount(30);
  drawParameter.SetLabelLineFitToArea(true);
  drawParameter.SetLabelLineFitToWidth(std::min(projection.GetWidth(), projection.GetHeight()));
  painter = new osmscout::MapPainterCairo(m_styleConfig);
};

rendererMap::~rendererMap()
{
printf("ERROR\n");
  cairo_destroy(m_cairo);
  cairo_surface_destroy(m_surface);
  free(painter);
};

int 
rendererMap::project(double zoom, double lon, double lat, cairo_surface_t **surface)
{

  if (m_surface!=nullptr) {

    if (m_cairo!=nullptr) {

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


      m_mapService->LookupTiles(projection,tiles);
      m_mapService->LoadMissingTileData(searchParameter,*m_styleConfig,tiles);
      m_mapService->AddTileDataToMapData(tiles,data);

      if (painter->DrawMap(projection,
                          drawParameter,
                          data,
                          m_cairo)) {
        // Map rendered
      }
    }
  } else {
    return GVA_ERROR;
  }

  *surface = m_surface;
  return GVA_SUCCESS;
};
