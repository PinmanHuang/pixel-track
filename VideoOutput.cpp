#include "VideoOutput.h"

namespace TLInOut
{

VideoOutput::VideoOutput(std::string filename, int width, int height, float fps/*=25*/, int offset_x, int offset_y)
{
  mVWriter=NULL;
  mfFPS=fps;
  miWidth=width;
  miHeight=height;
  miOffsetX=offset_x;
  miOffsetY=offset_y;
  mVWriter = new cv::VideoWriter(filename.c_str(), CV_FOURCC('D','I','V','X'), mfFPS, cvSize(miWidth, miHeight), true);
  if (mVWriter==NULL)
  {
    MESSAGE(0, "Could not open video file out.avi.");
  }
}

VideoOutput::~VideoOutput()
{
  if (mVWriter)
    delete mVWriter;
}

void VideoOutput::addFrame(TLImageProc::Image8U* img)
{
  cv::Mat mat, wholemat;
  if (miOffsetX==0 && miOffsetY==0)
  {
    ASSERT(img->width()==miWidth && img->height()==miHeight, "Image and video dimensions do not match in VideoOutput::addFrame().");
    mat = cv::Mat(miHeight, miWidth, CV_8UC3, img->data(), img->widthStep());
  }
  else
  {
    wholemat = cv::Mat(img->height(), img->width(), CV_8UC3, img->data(), img->widthStep());
    mat = wholemat(cv::Range(miOffsetY, miOffsetY+miHeight), cv::Range(miOffsetX, miOffsetX+miWidth));
  }
  mVWriter->write(mat);
}


}

