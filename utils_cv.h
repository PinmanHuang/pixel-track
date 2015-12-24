#ifndef TL_UTILS_CV_H
#define TL_UTILS_CV_H

#include <string>
#include <opencv2/opencv.hpp>
#include "Image.h"

//using namespace cv;
using namespace TLImageProc;

namespace TLInOut
{
  
void drawText(Image8U* img, char* text, int fontsize, int x, int y, Colour c);
Image8U* loadImage(std::string filename);
void saveImage(Image8U* img, std::string filename);
void saveImage(Image<double>* img, std::string filename);
void saveImage(Image<float>* img, std::string filename);


}

#endif
