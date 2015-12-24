#ifndef TL_VIDEO_INPUT_FILE_H
#define TL_VIDEO_INPUT_FILE_H

#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Error.h"

#include "VideoInput.h"

using namespace std;
//using namespace cv;

namespace TLInOut
{

class VideoInputFile : public VideoInput
{
  private:
    string msFilename;
    cv::VideoCapture* mCapture;
    cv::Mat mFrame;
    bool mbFrameGrabbed;
    bool mbNextImageAvailable;
    float mfBorderProportion;
    //Image<unsigned char>* mCurrentImage;

    int determineFPS_ffmpeg(string filename);

  public:
    VideoInputFile(string filename, float fps=-1, float border_percentage=0);
    ~VideoInputFile();

    bool nextImageAvailable();
    Image<unsigned char>* nextImage();
    unsigned long long getCurrentTimestampMs() { return (miFramesRead-1UL)*1000.0/mfFPS; };

    bool isGreyScale();
};
}


#endif
