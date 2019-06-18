#ifndef GVA_VIDEO_H
#define GVA_VIDEO_H

#define VIDEO_DEFAULT_HEIGHT 480
#define VIDEO_DEFAULT_WIDTH 640

enum videoFormat {
  RGBA_COLOUR = 0,
  RGB24_COLOUR,
  YUYV_COLOUR
};

class gvaVideoSource {
public:
  gvaVideoSource(int height, int width);
  virtual int gvaRecieveFrame(char *buffer, videoFormat format) = 0;
  virtual int gvaTransmitFrame(char *buffer, videoFormat format) = 0;
  int getWidth() { return m_width; };
  int getHeight() { return m_height; };
private:
  unsigned int m_height;
  unsigned int m_width;  
};
#endif
