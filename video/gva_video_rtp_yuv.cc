#include <iostream>
#include <string>
#include "gva_video_rtp_yuv.h"

using namespace std;

GvaVideoRtpYuv::GvaVideoRtpYuv(char* ip, int port, int height, int width)
: GvaVideoSource(height, width)
{ 
  strcpy(ip_, ip);
  port_ = port;
  frame_counter_ = 0;
};

GvaVideoRtpYuv::GvaVideoRtpYuv(char* ip, int port) 
: GvaVideoSource(VIDEO_DEFAULT_HEIGHT, VIDEO_DEFAULT_WIDTH)
{
  strcpy(ip_, ip);
  port_ = port;
  stream_ = new RtpStream(VIDEO_DEFAULT_HEIGHT, VIDEO_DEFAULT_WIDTH);
  stream_->RtpStreamIn(ip_, port_);
  stream_->Open();
}

GvaVideoRtpYuv::~GvaVideoRtpYuv() {
  free(stream_);
}

int
GvaVideoRtpYuv::GvaRecieveFrame(char *buffer, VideoFormat format) {
  char* frame_buffer;
  stream_->Recieve((void**)&frame_buffer, 5);
  switch (format) {
    case YUYV_COLOUR :
      memcpy(buffer, frame_buffer, GetWidth() * GetHeight() * 2);
      break;
    case RGBA_COLOUR :
      yuvtorgba(GetHeight(), GetWidth(), frame_buffer, buffer);
      break;
    case RGB24_COLOUR :
      yuvtorgb(GetHeight(), GetWidth(), frame_buffer, buffer);
      break;
  }
  frame_counter_++;
}
