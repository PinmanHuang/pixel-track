#ifndef TL_VIDEO_INPUT_H
#define TL_VIDEO_INPUT_H

#include "Image.h"

using namespace TLImageProc;

namespace TLInOut
{

class VideoInput
{
  protected:
    int miFramesRead;
    float mfFPS;
    int miImageOffsetX, miImageOffsetY;

  public:
    VideoInput();
    virtual ~VideoInput();

    float getFPS() { return mfFPS; };

    virtual bool nextImageAvailable()=0;
    virtual Image<unsigned char>* nextImage()=0;

    virtual unsigned long long getCurrentTimestampMs()=0;

    int getImageOffsetX() { return miImageOffsetX; };
    int getImageOffsetY() { return miImageOffsetY; };

    virtual bool isGreyScale()=0;
};


}

#endif
