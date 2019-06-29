#ifndef GVA_VIDEO_RTP_YUV_H
#define GVA_VIDEO_RTP_YUV_H
#include "gva_video.h"
#include "rtp_stream.h"

/*
 * Buffers are assumed to be YUV
 */

class gvaVideoRtpYuv : public gvaVideoSource {
public:
  gvaVideoRtpYuv(char* ip, int port, int height, int width);
  gvaVideoRtpYuv(char* ip, int port);
  ~gvaVideoRtpYuv();
  
  // Implementation of pure virtual base class functions
  int gvaRecieveFrame(char *buffer, videoFormat format);
  int gvaTransmitFrame(char *buffer, videoFormat format) { return -1; };
private:
  char m_ip[50];
  int m_port;
  int m_frameCounter;
  rtpStream *m_stream;
};
#endif
