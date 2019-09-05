#include "hmi_gva.h"

void HmiHelper::TableLicences(TableWidget *table) {
  table->Reset();
  table->visible_ = true;
  
  table->x_ = 110;
  table->y_ = 390;
  table->width_ = 420;

  table->AddRow(WEIGHT_BOLD);
  table->AddCell("Software", 80);
  table->AddCell("Licence", 20);

  table->AddRow();
  char tmp[100];
  sprintf(tmp, "Vivoe Lite");
  table->AddCell(tmp, 80);
  table->AddCell("MIT", 20);

  table->AddRow();
  table->AddCell("RTP Payloader", 80);
  table->AddCell("MIT", 20);

  table->AddRow();
  table->AddCell("OSMScout", 80);
  table->AddCell("LGPL", 20);

  table->AddRow();
  table->AddCell("Open Street Map", 80);
  table->AddCell("ODbL", 20);

  table->AddRow();
  table->AddCell("FastRTPS", 80);
  table->AddCell("Apache-2.0", 20);

  table->AddRow();
  table->AddCell("Protobuf", 80);
  table->AddCell("BSD", 20);
}

void HmiHelper::TableSystem(TableWidget *table) {
  table->Reset();
  table->visible_ = true;
  
  table->x_ = 110;
  table->y_ = 390;
  table->width_ = 420;

  table->AddRow(WEIGHT_BOLD);
  table->AddCell("Fuction", 80);
  table->AddCell("Status", 20);

  table->AddRow();
  char tmp[100];
  sprintf(tmp, "HMI Version %d.%d.%d", MAJOR, MINOR, PATCH);
  table->AddCell(tmp, 80);
  table->AddCell("Ok", 20);

  table->AddRow();
  table->AddCell("GPS Source, /dev/ttyUSB0", 80);
  table->AddCell("Error", 20, Renderer::PackRgb(RED));

  table->AddRow();
  table->AddCell("ODB Source, /dev/ttyUSB1", 80);
  table->AddCell("Error", 20, Renderer::PackRgb(RED));

  table->AddRow();
  table->AddCell("UK Maps, /opt/osmscout/maps/england", 80);
  table->AddCell("Ok", 20);

  table->AddRow();
  table->AddCell("Timesource GPS Lock", 80);
  table->AddCell("Off", 20, Renderer::PackRgb(ORANGE));
}
  
void HmiHelper::TableAlarms(TableWidget *table) {
  table->visible_ = true;
  table->x_ = 110;
  table->y_ = 390;
  table->width_ = 420;

  table->AddRow(WEIGHT_BOLD);
  table->AddCell("Time", 20);
  table->AddCell("Alarm Text", 50);
  table->AddCell("Cat", 10);
  table->AddCell("Status", 20);
  
  table->AddRow();
  table->AddCell("15/6 15:06", 20);
  table->AddCell("Low engine oil pressure", 50);
  table->AddCell("W", 10);
  table->AddCell("RES", 20);
  
  table->AddRow(Renderer::PackRgb(WHITE), 
                 Renderer::PackRgb(RED), 
                 Renderer::PackRgb(WHITE), 
                 Renderer::PackRgb(YELLOW), WEIGHT_NORMAL);
  table->AddCell("15/6 15:26", 20);
  table->AddCell("Engine over tempreture", 50);
  table->AddCell("W", 10);
  table->AddCell("UNACK", 20);
  
  table->AddRow();
  table->CurrentRowHighlight();
  table->AddCell("15/6 15:29", 20);
  table->AddCell("Engine over tempreture", 50);
  table->AddCell("W", 10);
  table->AddCell("RES", 20);
  
  table->AddRow();
  table->AddCell("15/6 14:00", 20);
  table->AddCell("Gun fault", 50);
  table->AddCell("C", 10);
  table->AddCell("RES", 20);
  
  table->AddRow();
  table->AddCell("15/6 18:16", 20);
  table->AddCell("Air con fault", 50);
  table->AddCell("A", 10);
  table->AddCell("ACT", 20);
  
  table->AddRow(Renderer::PackRgb(WHITE), 
                        Renderer::PackRgb(GREY), 
                        Renderer::PackRgb(WHITE), 
                        Renderer::PackRgb(YELLOW), WEIGHT_NORMAL);
  table->AddCell("15/6 19:03", 20);
  table->AddCell("Gun barrel over tempreture", 50);
  table->AddCell("C", 10);
  table->AddCell("ACT(OVR)", 20);
  
  table->AddRow(Renderer::PackRgb(WHITE), 
                        Renderer::PackRgb(ORANGE), 
                        Renderer::PackRgb(WHITE), 
                        Renderer::PackRgb(YELLOW), WEIGHT_NORMAL);
  table->AddCell("15/6 19:", 20);
  table->AddCell("LRU xx fault", 50);
  table->AddCell("C", 10);
  table->AddCell("ACT", 20);

  table->AddRow();
  table->AddCell("Page 1 of 1", 100, ALIGN_RIGHT);
}
