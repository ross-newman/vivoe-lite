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
#ifndef GVA_VIDEO_RTP_YUV_H
#define GVA_VIDEO_RTP_YUV_H
#include "gva_video.h"
#include "rtp_stream.h"

//
// Buffers are assumed to be YUV
//
class GvaVideoRtpYuv : public GvaVideoSource {
 public:
  GvaVideoRtpYuv(char *ip, int port, int height, int width);
  GvaVideoRtpYuv(char *ip, int port);
  ~GvaVideoRtpYuv();

  // Implementation of pure virtual base class functions
  int GvaRecieveFrame(char *buffer, VideoFormat format);
  int GvaTransmitFrame(char *buffer, VideoFormat format) { return -1; };

 private:
  char ip_[50];
  int port_;
  int frame_counter_;
  RtpStream *stream_;
};
#endif
