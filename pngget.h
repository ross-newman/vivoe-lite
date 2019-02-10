#ifndef PNGGET_H
#define PNGGET_H

#include <png.h>

class png
{
public:
  ~png();
  void readPngFile (char *file_name);
  png_bytep *getRGBBufferPointer (int row = 0);
  int getHeight() { return m_height; };
  int getWidth() { return m_width; };
private:
  png_byte * m_buffer;
  png_bytep * m_row_pointers = NULL;
  png_structp png_ptr;
  int m_width;
  int m_height;
};

#endif
