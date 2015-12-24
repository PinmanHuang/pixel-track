extern "C" {
  #include <libavformat/avformat.h>
}
#include "RandomGenerator.h"
#include "VideoInputFile.h"

namespace TLInOut
{

VideoInputFile::VideoInputFile(string filename, float fps/*=-1*/, float border_proportion/*=0*/)
{
  msFilename = filename;
  mbFrameGrabbed = false;
  mbNextImageAvailable = false;
  av_register_all();  // probably only necessary for determineFPS_ffmpeg()
  mCapture = new cv::VideoCapture();
  mfBorderProportion=border_proportion;
  if (!mCapture->open(msFilename))
  {
    delete mCapture;
    throw TLUtil::FileOpen_Ex(msFilename);
  }
  if (fps>0)
    mfFPS = fps;
  else
  { 
    mfFPS = mCapture->get(CV_CAP_PROP_FPS);
    if (mfFPS<=0 || isnan(mfFPS))
    {
      mCapture->release();

      if (determineFPS_ffmpeg(msFilename)!=0)
      {
        mfFPS=25;
        MESSAGE(1, "Couldn't retrieve FPS information. Assuming " << mfFPS << "fps");
      }

      mCapture->open(msFilename);
    }
  }
}

VideoInputFile::~VideoInputFile()
{
  delete mCapture;
}


bool VideoInputFile::nextImageAvailable()
{
  if (!mbFrameGrabbed)
  {
    if (mCapture->grab())
    {
      mbNextImageAvailable=true;
      mbFrameGrabbed=true;
      return true;
    }
    return false;
  }
  else
    return mbNextImageAvailable;
}


Image<unsigned char>* VideoInputFile::nextImage()
{
  if (!nextImageAvailable())
    return NULL;

  if (!mCapture->retrieve(mFrame))
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


int VideoInputFile::determineFPS_ffmpeg(string filename)
{
    AVFormatContext *pFormatCtx=NULL;
    int i, videoStream;
    AVCodecContext *pCodecCtx=NULL;

    //if(av_open_input_file(&pFormatCtx, filename.c_str(), NULL, 0, NULL)!=0)
    if(avformat_open_input(&pFormatCtx, filename.c_str(), NULL, NULL)!=0)
      return -1;


    //if (av_find_stream_info(pFormatCtx)<0)
    if (avformat_find_stream_info(pFormatCtx, NULL)<0)
    {
      //av_close_input_file(pFormatCtx);
      avformat_close_input(&pFormatCtx);
      return -2;
    }


    // Find the first video stream
    videoStream=-1;
    for(i=0; i<(int)pFormatCtx->nb_streams; i++)
      //if(pFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_VIDEO)
      if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
      {
        videoStream=i;
        break;
      }
    if(videoStream==-1)
    {
      //av_close_input_file(pFormatCtx);
      avformat_close_input(&pFormatCtx);
      return -3;
    }

    // Get a pointer to the codec context for the video stream
    pCodecCtx=pFormatCtx->streams[videoStream]->codec;

    // Hack to correct wrong frame rates that seem to be generated by some 
    // codecs
    //if(pCodecCtx->frame_rate>1000 && pCodecCtx->frame_rate_base==1)
      //pCodecCtx->frame_rate_base=1000;

    mfFPS = (float)pCodecCtx->time_base.den/pCodecCtx->time_base.num;

    if (mfFPS<=0 || mfFPS>500)
    {
      //av_close_input_file(pFormatCtx);
      avformat_close_input(&pFormatCtx);
      return -4;
    }

    //av_close_input_file(pFormatCtx);
    avformat_close_input(&pFormatCtx);
    return 0;
}

bool VideoInputFile::isGreyScale()
{
  if (!nextImageAvailable())
    return false;

  if (!mCapture->retrieve(mFrame))
    return false;
  
  RandomGenerator rg;
  Image<unsigned char>* newimage; 
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
