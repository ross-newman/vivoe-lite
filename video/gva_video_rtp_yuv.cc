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
#include "gva_video_rtp_yuv.h"

#include <iostream>
#include <string>

using namespace std;

GvaVideoRtpYuv::GvaVideoRtpYuv(char* ip, int port, int height, int width)
    : GvaVideoSource(height, width) {
  strcpy(ip_, ip);
  port_ = port;
  frame_counter_ = 0;
};

GvaVideoRtpYuv::GvaVideoRtpYuv(char* ip, int port)
    : GvaVideoSource(VIDEO_DEFAULT_HEIGHT, VIDEO_DEFAULT_WIDTH) {
  strcpy(ip_, ip);
  port_ = port;
  stream_ = new RtpStream(VIDEO_DEFAULT_HEIGHT, VIDEO_DEFAULT_WIDTH);
  stream_->RtpStreamIn(ip_, port_);
  stream_->Open();
}

GvaVideoRtpYuv::~GvaVideoRtpYuv() { free(stream_); }

int GvaVideoRtpYuv::GvaRecieveFrame(char* buffer, VideoFormat format) {
  char* frame_buffer;
  stream_->Recieve((void**)&frame_buffer, 5);
  switch (format) {
    case YUYV_COLOUR:
      memcpy(buffer, frame_buffer, GetWidth() * GetHeight() * 2);
      break;
    case RGBA_COLOUR:
      yuvtorgba(GetHeight(), GetWidth(), frame_buffer, buffer);
      break;
    case RGB24_COLOUR:
      yuvtorgb(GetHeight(), GetWidth(), frame_buffer, buffer);
      break;
  }
  frame_counter_++;
}
