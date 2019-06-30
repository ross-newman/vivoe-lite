#ifndef DEBUG_H
#define DEBUG_H

#define DUMMY_LON -0.124240 
#define DUMMY_LAT 51.500655
#define DEFAULT_ZOOM 20000
#define DEBUG_VIDEO_LIVE 0
#define TRACE_POINT cout << "Function : " << __FUNCTION__ << ", Line : " << __LINE__ << "\n"

#include <stdio.h>
void DumpHex(const void* data, size_t size);

#endif
