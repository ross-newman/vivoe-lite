//
// MIT License
//
// Copyright (c) 2020 Ross Newman (ross@rossnewman.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#ifndef GVA_VIDEO_H
#define GVA_VIDEO_H

#define VIDEO_DEFAULT_HEIGHT 480
#define VIDEO_DEFAULT_WIDTH 640

enum VideoFormat { RGBA_COLOUR = 0, RGB24_COLOUR, YUYV_COLOUR };

class GvaVideoSource {
 public:
  GvaVideoSource(int height, int width);
  virtual int GvaRecieveFrame(char *buffer, VideoFormat format) = 0;
  virtual int GvaTransmitFrame(char *buffer, VideoFormat format) = 0;
  int GetWidth() { return width_; };
  int GetHeight() { return height_; };

 private:
  unsigned int height_;
  unsigned int width_;
};
#endif
