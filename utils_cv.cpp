#include <ostream>

#include "utils_cv.h"


using namespace std;


namespace TLInOut
{

void drawText(Image8U* img, char* text, int fontsize, int x, int y, Colour c)
{
  cv::Mat m(img->height(), img->width(), img->nChannels()==3?CV_8UC3:CV_8UC1, img->data(), img->widthStep());
  cv::putText(m, text, cvPoint(x, y), cv::FONT_HERSHEY_PLAIN, 1.0, CV_RGB(c.r,c.g,c.b));
}

Image8U* loadImage(std::string filename)
{
  cv::Mat m = cv::imread(filename, CV_LOAD_IMAGE_COLOR);
  Image8U* img = new Image8U(m.cols, m.rows, 3, m.step[0], m.data, false);;
  return img;
}

void saveImage(Image8U* img, string filename)
{
  cv::Mat m(img->height(), img->width(), img->nChannels()==3?CV_8UC3:CV_8UC1, img->data(), img->widthStep());
  vector<int> compression_params;
  imwrite(filename, m, compression_params);
}

void saveImage(Image<double>* img, string filename)
{
  cv::Mat m(img->height(), img->width(), CV_64FC1, img->data()); 
  cv::Mat res; 
  m.convertTo(res, CV_8UC1, 255);
  vector<int> compression_params;
  imwrite(filename, res, compression_params);
}

void saveImage(Image<float>* img, string filename)
{
  cv::Mat m(img->height(), img->width(), CV_32FC1, img->data()); 
  cv::Mat res; 
  m.convertTo(res, CV_8UC1, 255);
  vector<int> compression_params;
  imwrite(filename, res, compression_params);
}

}


