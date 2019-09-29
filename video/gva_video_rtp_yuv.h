#ifndef GVA_VIDEO_RTP_YUV_H
#define GVA_VIDEO_RTP_YUV_H
#include "gva_video.h"
#include "rtp_stream.h"

//
// Buffers are assumed to be YUV
//
class GvaVideoRtpYuv : public GvaVideoSource {
public:
  GvaVideoRtpYuv(char* ip, int port, int height, int width);
  GvaVideoRtpYuv(char* ip, int port);
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
