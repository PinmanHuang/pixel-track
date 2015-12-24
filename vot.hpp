/* 
 *  Author : Tomas Vojir
 *  Date   : 2013-06-05
 *  Desc   : Simple class for parsing VOT inputs and providing 
 *           interface for image loading and storing output.
 */ 

#ifndef CPP_VOT_H
#define CPP_VOT_H

#include <string>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

// Bounding box type
typedef struct {
    float x1;
    float y1;
    float x2;
    float y2;
    float x3;
    float y3;
    float x4;
    float y4;
} VOTPolygon;

class VOT
{
public:
    VOT(const std::string & region_file, const std::string & images, const std::string & ouput)
    {
        p_region_stream.open(region_file.c_str());
        VOTPolygon p;
	cv::Rect r;
        if (p_region_stream.is_open()){
            char ch;
            p_region_stream >> p.x1 >> ch >> p.y1 >> ch;
            p_region_stream >> p.x2 >> ch >> p.y2 >> ch;
            p_region_stream >> p.x3 >> ch >> p.y3 >> ch;
            p_region_stream >> p.x4 >> ch >> p.y4;

	    // added by SD 2014-06-05
	    int minx=p.x1, minx2=p.x1;  // smallest and second smallest x
	    int miny=p.y1, miny2=p.y1;  // smallest and second smallest y
	    int maxx=p.x1, maxx2=p.x1;  // largest and second largest x
	    int maxy=p.y1, maxy2=p.y1;  // largest and second largest y

	    if (p.x2<minx)
	      minx=p.x2;
	    else
	      minx2=p.x2;
	    if (p.x3<minx2)
	    {
	      if (p.x3<minx)
	      {
		minx2=minx;
		minx=p.x3;
	      }
	      else
		minx2=p.x3;
	    }
	    if (p.x4<minx2)
	    {
	      if (p.x4<minx)
	      {
		minx2=minx;
		minx=p.x4;
	      }
	      else
		minx2=p.x4;
	    }

	    if (p.y2<miny)
	      miny=p.y2;
	    else
	      miny2=p.y2;
	    if (p.y3<miny2)
	    {
	      if (p.y3<miny)
	      {
		miny2=miny;
		miny=p.y3;
	      }
	      else
		miny2=p.y3;
	    }
	    if (p.y4<miny2)
	    {
	      if (p.y4<miny)
	      {
		miny2=miny;
		miny=p.y4;
	      }
	      else
		miny2=p.y4;
	    }
	    if (p.x2>maxx)
	      maxx=p.x2;
	    else
	      maxx2=p.x2;
	    if (p.x3>maxx2)
	    {
	      if (p.x3>maxx)
	      {
		maxx2=maxx;
		maxx=p.x3;
	      }
	      else
		maxx2=p.x3;
	    }
	    if (p.x4>maxx2)
	    {
	      if (p.x4>maxx)
	      {
		maxx2=maxx;
		maxx=p.x4;
	      }
	      else
		maxx2=p.x4;
	    }

	    if (p.y2>maxy)
	      maxy=p.y2;
	    else
	      maxy2=p.y2;
	    if (p.y3>maxy2)
	    {
	      if (p.y3>maxy)
	      {
		maxy2=maxy;
		maxy=p.y3;
	      }
	      else
		maxy2=p.y3;
	    }
	    if (p.y4>maxy2)
	    {
	      if (p.y4>maxy)
	      {
		maxy2=maxy;
		maxy=p.y4;
	      }
	      else
		maxy2=p.y4;
	    }
	    r.x = 0.5*(minx+minx2);
	    r.y = 0.5*(miny+miny2);
	    r.width = 0.5*(maxx+maxx2)-r.x;
	    r.height = 0.5*(maxy+maxy2)-r.y;
	    /*
	    r.x = 0.5*(0.8*minx+1.2*minx2);
	    r.y = 0.5*(0.8*miny+1.2*miny2);
	    r.width = 0.5*(0.8*maxx+1.2*maxx2)-r.x;
	    r.height = 0.5*(0.8*maxy+1.2*maxy2)-r.y;
	    */
	    // end SD

        }else{
            std::cerr << "Error loading initial region in file " << region_file << "!" << std::endl;
            p.x1=0;
            p.y1=0;
            p.x2=0;
            p.y2=0;
            p.x3=0;
            p.y3=0;
            p.x4=0;
            p.y4=0;
        }

        p_init_polygon = p;
	p_init_rectangle = r;

        p_images_stream.open(images.c_str());
        if (!p_images_stream.is_open())
            std::cerr << "Error loading image file " << images << "!" << std::endl;

        p_output_stream.open(ouput.c_str());
        if (!p_output_stream.is_open())
            std::cerr << "Error opening output file " << ouput << "!" << std::endl;
    }

    ~VOT()
    {
        p_region_stream.close();
        p_images_stream.close();
        p_output_stream.close();
    }

    inline VOTPolygon getInitPolygon() const 
    {   return p_init_polygon;    }

    // added by SD 2014-06-05
    inline cv::Rect getInitRectangle() const 
    {   return p_init_rectangle;    }

    inline void outputBoundingBox(const cv::Rect & bbox)
    {   p_output_stream << bbox.x << ", " << bbox.y << ", " << bbox.width << ", " << bbox.height << std::endl; }

    inline void outputPolygon(const VOTPolygon & poly)
    {
      p_output_stream << poly.x1 << "," << poly.y1 << ",";
      p_output_stream << poly.x2 << "," << poly.y2 << ",";
      p_output_stream << poly.x3 << "," << poly.y3 << ",";
      p_output_stream << poly.x4 << "," << poly.y4 << std::endl;
    }

    // added by SD 2014-06-05
    inline bool nextImageAvailable()
    {
      return !p_images_stream.eof();
    }

    inline int getNextImage(cv::Mat & img)
    {
    if (p_images_stream.eof() || !p_images_stream.is_open())
            return -1;

    std::string line;
    std::getline (p_images_stream, line);
	if (line.empty() && p_images_stream.eof()) return -1;
    img = cv::imread(line, CV_LOAD_IMAGE_COLOR);
 
    return 1;
  }

  inline int peekNextImage(cv::Mat & img)
  {
    if (p_images_stream.eof() || !p_images_stream.is_open())
      return -1;

    int pos = p_images_stream.tellg();
    std::string line;
    std::getline (p_images_stream, line);
    p_images_stream.seekg(pos);
    if (line.empty() && p_images_stream.eof()) return -1;
    img = cv::imread(line, CV_LOAD_IMAGE_COLOR);

    return 1;
  }

private:
    VOTPolygon p_init_polygon;
    cv::Rect p_init_rectangle;  // added by SD 2014-06-05
    std::ifstream p_region_stream;
    std::ifstream p_images_stream;
    std::ofstream p_output_stream;

};

#endif //CPP_VOT_H
