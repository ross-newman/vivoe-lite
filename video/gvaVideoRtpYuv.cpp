#include <iostream>
#include <string>
#include "gvaVideoRtpYuv.hpp"

using namespace std;

gvaVideoRtpYuv::gvaVideoRtpYuv(char* ip, int port, int height, int width)
: gvaVideoSource(height, width)
{ 
  strcpy(m_ip, ip);
  m_port = port;
  m_frameCounter = 0;
};

gvaVideoRtpYuv::gvaVideoRtpYuv(char* ip, int port) 
: gvaVideoSource(VIDEO_DEFAULT_HEIGHT, VIDEO_DEFAULT_WIDTH)
{
  strcpy(m_ip, ip);
  m_port = port;
  m_stream = new rtpStream(VIDEO_DEFAULT_HEIGHT, VIDEO_DEFAULT_WIDTH);
  m_stream->rtpStreamIn(m_ip, m_port);
  m_stream->Open();
}

gvaVideoRtpYuv::~gvaVideoRtpYuv() {
  free(m_stream);
}

int
gvaVideoRtpYuv::gvaRecieveFrame(char *buffer, videoFormat format) {
  char* frameBuffer;
  m_stream->Recieve((void**)&frameBuffer, 5);
  switch (format) {
    case YUYV_COLOUR :
      memcpy(buffer, frameBuffer, getWidth() * getHeight() * 2);
      break;
    case RGBA_COLOUR :
      yuvtorgba(getHeight(), getWidth(), frameBuffer, buffer);
      break;
    case RGB24_COLOUR :
      yuvtorgb(getHeight(), getWidth(), frameBuffer, buffer);
      break;
  }
  m_frameCounter++;
}
