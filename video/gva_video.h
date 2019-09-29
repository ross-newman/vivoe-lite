#ifndef GVA_VIDEO_H
#define GVA_VIDEO_H

#define VIDEO_DEFAULT_HEIGHT 480
#define VIDEO_DEFAULT_WIDTH 640

enum VideoFormat {
  RGBA_COLOUR = 0,
  RGB24_COLOUR,
  YUYV_COLOUR
};

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
