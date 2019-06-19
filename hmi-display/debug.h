#ifndef DEBUG_H
#define DEBUG_H

#define TRACE_POINT cout << "Function : " << __FUNCTION__ << ", Line : " << __LINE__ << "\n"

#include <stdio.h>
void DumpHex(const void* data, size_t size);

#endif
