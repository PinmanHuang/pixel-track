#ifndef TL_VIDEO_OUTPUT
#define TL_VIDEO_OUTPUT

#include <string>
#include "Image.h"
#include <opencv2/highgui/highgui.hpp>

namespace TLInOut
{

class VideoOutput
{
  private:
    cv::VideoWriter* mVWriter;
    float mfFPS;
    int miWidth;
    int miHeight;
    int miOffsetX, miOffsetY;

  public:
    VideoOutput(std::string filename, int width, int height, float fps=25, int offset_x=0, int offset_y=0);
    ~VideoOutput();

    void addFrame(TLImageProc::Image8U* img);
};

}


#endif
