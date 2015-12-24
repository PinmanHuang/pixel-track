#include "RandomGenerator.h"
#include "VideoInputVOT.h"

namespace TLInOut
{

VideoInputVOT::VideoInputVOT(VOT* vot, float border_proportion/*=0*/)
{
  m_vot = vot;
  mbFrameGrabbed = false;
  mbNextImageAvailable = false;
  mfFPS = 25;
  mfBorderProportion=border_proportion;
}

VideoInputVOT::~VideoInputVOT()
{
}


bool VideoInputVOT::nextImageAvailable()
{
  return m_vot->nextImageAvailable();
}


Image<unsigned char>* VideoInputVOT::nextImage()
{
  if (!nextImageAvailable())
    return NULL;

  if (m_vot->getNextImage(mFrame)<0)
    return NULL;
  
  Image<unsigned char>* newimage;
  
  if (mfBorderProportion<1e-5) // no border
    newimage = new Image<unsigned char>(mFrame.cols, mFrame.rows, 3, mFrame.step, mFrame.data);
  else
  {
    newimage = new Image<unsigned char>(mFrame.cols*(1+2.0*mfBorderProportion), mFrame.rows*(1+2.0*mfBorderProportion), 3, mFrame.step, mFrame.data, mFrame.cols, mFrame.rows);
    miImageOffsetX = (newimage->width() -mFrame.cols)/2;
    miImageOffsetY = (newimage->height()-mFrame.rows)/2;
  }

  VideoInput::miFramesRead++;
  mbFrameGrabbed=false;
  return newimage;
}

bool VideoInputVOT::isGreyScale()
{
  if (!nextImageAvailable())
    return false;

  if (m_vot->peekNextImage(mFrame)<0)
    return false;
  
  Image<unsigned char>* newimage;
  RandomGenerator rg;
  
  newimage = new Image<unsigned char>(mFrame.cols, mFrame.rows, 3, mFrame.step, mFrame.data);
  // sample 20 random points
  int x,y;
  for(int i=0; i<20; i++)
  {
    x=(int)rg.boundedUniform(0, mFrame.cols-1); y=(int)rg.boundedUniform(0, mFrame.rows-1);
    if (newimage->get(x, y, 0)!=newimage->get(x, y, 1) || newimage->get(x, y, 0)!=newimage->get(x, y, 2))
    {
      delete newimage;
      return false;
    }
  }
  delete newimage;
  return true;
}




}

