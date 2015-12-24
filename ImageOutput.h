#ifndef TL_IMAGE_OUTPUT_H
#define TL_IMAGE_OUTPUT_H

#include <string>
#include <opencv2/opencv.hpp>

#include "Image.h"
#include "Rectangle.h"

using namespace std;
//using namespace cv;
using namespace TLImageProc;

namespace TLInOut
{

  class ImageOutput
  {
    private:
      string msWindowName;
      Image8U* mCurrentImage;

      static void mouseHandler(int event, int x, int y, int flags, void* param);

    public:
      ImageOutput(string window_name);
      ~ImageOutput();

      string windowName() { return msWindowName; };
      void setCurrentImage(Image8U* img) { mCurrentImage = img; };

      /** 
       * Shows a colour image and waites @a wait_ms. 
       * @param wait_ms Time in ms to wait after displaying the image. 
       *   If negative, waits infinitely for a key press.
       * @returns Number of key pressed or if @a wait_ms >= 0.
       */
      int showImage(Image8U* image, int wait_ms);
      int showIntImage(Image<int>* image, int wait_ms);
      int showFloatImage(Image<float>* image, int wait_ms);
      int showDoubleImage(Image<double>* image, int wait_ms);
      Rectangle selectROI();
      int wait(int ms);

      void draw(Rectangle r, Colour c, int Thickness=1);
      void draw(Rectangle r, double rotation, Colour c, int Thickness=1);
      void draw(int x, int y, Colour c, int Thickness=1);
      void draw(char* text, int fontsize, int x, int y, Colour c);
      void draw(int x0, int y0, int x1, int y1, Colour c, int Thickness=1);
  };

  // structure for mouse handler
  typedef struct mh_params {
    cv::Point loc1;
    cv::Point loc2;
    string win_name;
    cv::Mat orig_img;
    //Mat cur_img;
  } mh_params;
}



#endif
