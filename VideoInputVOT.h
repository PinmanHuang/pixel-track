#ifndef TL_VIDEO_INPUT_VOT_H
#define TL_VIDEO_INPUT_VOT_H

#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

//#include "../Parameters.h"
#include "Error.h"

#include "VideoInput.h"
#ifdef VOT_2013
#include "vot2013.hpp"
#else
#include "vot.hpp"
#endif

using namespace std;
//using namespace cv;

namespace TLInOut
{

class VideoInputVOT : public VideoInput
{
  private:
    cv::Mat mFrame;
    bool mbFrameGrabbed;
    bool mbNextImageAvailable;
    Image<unsigned char>* mCurrentImage;
    VOT* m_vot;
    float mfBorderProportion;

  public:
    VideoInputVOT(VOT* vot, float border_percentage=0);
    ~VideoInputVOT();

    bool nextImageAvailable();
    Image<unsigned char>* nextImage();
    unsigned long long getCurrentTimestampMs() { return (miFramesRead-1UL)*1000.0/mfFPS; };

    bool isGreyScale();
};

}


#endif
