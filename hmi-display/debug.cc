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

#include "debug.h"

void
DumpHex (const void *data, size_t size)
{
  char ascii[17];
  size_t i, j;
  ascii[16] = '\0';
  for (i = 0; i < size; ++i)
    {
      printf ("%02X ", ((unsigned char *) data)[i]);
      if (((unsigned char *) data)[i] >= ' '
          && ((unsigned char *) data)[i] <= '~')
        {
          ascii[i % 16] = ((unsigned char *) data)[i];
        }
      else
        {
          ascii[i % 16] = '.';
        }
      if ((i + 1) % 8 == 0 || i + 1 == size)
        {
          printf (" ");
          if ((i + 1) % 16 == 0)
            {
              printf ("|  %s \n", ascii);
            }
          else if (i + 1 == size)
            {
              ascii[(i + 1) % 16] = '\0';
              if ((i + 1) % 16 <= 8)
                {
                  printf (" ");
                }
              for (j = (i + 1) % 16; j < 16; ++j)
                {
                  printf ("   ");
                }
              printf ("|  %s \n", ascii);
            }
        }
    }
}
