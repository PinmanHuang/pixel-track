#include <opencv2/highgui/highgui.hpp>

#include "Error.h"
#include "utils_cv.h"
#include "Draw.h"
#include "ImageOutput.h"

//using namespace cv;
using namespace TLImageProc;
using namespace TLUtil;

namespace TLInOut
{

ImageOutput::ImageOutput(string window_name)
{
  msWindowName = window_name;
  mCurrentImage=NULL;
  cv::namedWindow(msWindowName);
}

ImageOutput::~ImageOutput()
{
  cv::destroyWindow(msWindowName);
}

int ImageOutput::showImage(Image8U* image, int wait_ms)
{
  cv::Mat m(image->height(), image->width(), image->nChannels()==3?CV_8UC3:CV_8UC1, image->data(), image->widthStep());
  cv::imshow(msWindowName, m);
  mCurrentImage = image;
  return (cv::waitKey(wait_ms));
}

int ImageOutput::showIntImage(Image<int>* image, int wait_ms)
{
  cv::Mat m(image->height(), image->width(), image->nChannels()==3?CV_32SC3:CV_32SC1, image->data(), image->widthStep());
  cv::imshow(msWindowName, m);
  //mCurrentImage = image;
  return (cv::waitKey(wait_ms));
}

int ImageOutput::showFloatImage(Image<float>* image, int wait_ms)
{
  cv::Mat m(image->height(), image->width(), CV_32F, image->data(), image->widthStep()*sizeof(float));
  cv::imshow(msWindowName, m);
  //mCurrentImage = image;
  return (cv::waitKey(wait_ms));
}

int ImageOutput::showDoubleImage(Image<double>* image, int wait_ms)
{
  cv::Mat m(image->height(), image->width(), CV_64F, image->data(), image->widthStep()*sizeof(double));
  cv::imshow(msWindowName, m);
  //mCurrentImage = image;
  return (cv::waitKey(wait_ms));
}

int ImageOutput::wait(int ms)
{
  return (cv::waitKey(ms));
}

void ImageOutput::mouseHandler(int event, int x, int y, int flags, void* param)
{
  mh_params* p = (mh_params*)param;
  cv::Point loc;
  cv::Mat tmp;
  static int pressed = false;
  
  // LEFT button DOWN: first corner
  if( event == CV_EVENT_LBUTTONDOWN )
    {
      loc.x = x;
      loc.y = y;
      p->loc1=loc;
      pressed = true;
    }

  // LEFT button UP: finished the rectangle
  else if( event == CV_EVENT_LBUTTONUP )
    {
      loc.x = x;
      loc.y = y;
      p->loc2=loc;
      tmp = p->orig_img.clone();
      //rectangle( p->orig_img, p->loc1, loc, CV_RGB(255,0,0), 1, 8, 0 );
      rectangle( tmp, p->loc1, loc, CV_RGB(255,0,0), 1, 8, 0 );
      //imshow( p->win_name, p->orig_img);
      imshow( p->win_name, tmp);
      pressed = false;
    }

  // LEFT button DOWN: drawing the rectangle
  else if( event == CV_EVENT_MOUSEMOVE  &&  flags & CV_EVENT_FLAG_LBUTTON )
    {
      tmp = p->orig_img.clone();
      rectangle( tmp, p->loc1, cv::Point(x, y), CV_RGB(130, 0, 0), 1, 8, 0 );
      imshow( p->win_name, tmp );
    }
}

Rectangle ImageOutput::selectROI()
{
  ASSERT(mCurrentImage!=NULL, "No image displayed.");

  mh_params p;
  //Rect* r;
  int x1, y1, x2, y2, w, h;
  
  p.orig_img = cv::Mat(mCurrentImage->height(), mCurrentImage->width(), CV_8UC3, mCurrentImage->data(), mCurrentImage->widthStep());;
  p.win_name = msWindowName;
  
  cv::setMouseCallback(msWindowName, mouseHandler, &p);
  cv::waitKey( 0 );

  // extract ROI selected by the user
  x1 = min( p.loc1.x, p.loc2.x );
  x2 = max( p.loc1.x, p.loc2.x );
  y1 = min( p.loc1.y, p.loc2.y );
  y2 = max( p.loc1.y, p.loc2.y );
  w = x2 - x1;
  h = y2 - y1;

  MESSAGE(1, "Coordinates of ROI: " << x1 << " " << x2 << " " << y1 << " " << y2);
  MESSAGE(1, "                    " << x1 << "," << y1 << "," << w << "," << h);
  return Rectangle( x1, y1, x1+w-1, y1+h-1 );
}

void ImageOutput::draw(Rectangle r, Colour c, int Thickness/*=1*/)
{
  ASSERT(mCurrentImage!=NULL, "No image displayed.");

  drawRectangle(mCurrentImage, r, c, Thickness);
}

void ImageOutput::draw(Rectangle r, double rotation, Colour c, int Thickness/*=1*/)
{
  ASSERT(mCurrentImage!=NULL, "No image displayed.");

  drawRectangle(mCurrentImage, r, rotation, c, Thickness);
}

void ImageOutput::draw(int x, int y, Colour c, int Thickness/*=1*/)
{
  ASSERT(mCurrentImage!=NULL, "No image displayed.");

  drawCross(mCurrentImage, x, y, c, Thickness);
}

void ImageOutput::draw(char* text, int fontsize, int x, int y, Colour c)
{
  ASSERT(mCurrentImage!=NULL, "No image displayed.");

  drawText(mCurrentImage, text, fontsize, x, y, c);
}

void ImageOutput::draw(int x0, int y0, int x1, int y1, Colour c, int Thickness/*=1*/)
{
  ASSERT(mCurrentImage!=NULL, "No image displayed.");

  drawLine(mCurrentImage, x0, y0, x1, y1, c, Thickness);
}



}
