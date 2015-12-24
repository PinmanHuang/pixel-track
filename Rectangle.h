#ifndef TL_RECTANGLE_H
#define TL_RECTANGLE_H

#include <math.h>
#include <string>


namespace TLImageProc {

  class Rectangle
  {
    public: // members : same as opencv, so that we can do casting
      int miFirstColumn; 
      int miFirstLine; 
      int miWidth;
      int miHeight;

      Rectangle() { init(0,0,0,0); };
      Rectangle(std::string x_y_w_h);
      Rectangle(int first_col,int first_line, int last_col,int last_line);
      Rectangle(int nb_columns,int nb_lines);
      ~Rectangle() {}

      void init(int first_col,int first_line,int last_col,int last_line);
      void initPosAndSize(int first_col,int first_line, int width,int height);
      void initCenterAndSize(int center_x,int center_y, int width,int height);
      void initPos(int left, int top) { miFirstColumn = left; miFirstLine = top; };
      void initSize(int width,int height);

      int  firstLine() { return miFirstLine;}
      int  firstColumn() { return miFirstColumn; }
      int  lastLine() { return miFirstLine+miHeight-1;}
      int  lastColumn() { return miFirstColumn+miWidth-1; }
      float ratio() { return (float)miHeight/miWidth; };

      void translate(int dep_co,int dep_li);
      bool inside(int column,int line);
      bool inside(Rectangle smaller);
      int area();
      void rotate(double angle, double& ulx, double& uly, double& urx, double& ury, double& lrx, double& lry, double& llx, double& lly);

      bool empty();
      void setEmpty();
      void add(int col,int li);
      void add(float col,float  li);
      
      void setCenter(int x, int y);
      int centerX() { return miFirstColumn+miWidth/2; };
      int centerY() { return miFirstLine+miHeight/2; };
      float centerXf() { return miFirstColumn+(float)miWidth/2.0; };
      float centerYf() { return miFirstLine+(float)miHeight/2.0; };

      void intersection(Rectangle & BBox2);
      bool isOverlapping(Rectangle& bb);
      float precision(Rectangle& ground_truth);
      float recall(Rectangle& ground_truth);
      float f_measure(Rectangle& ground_truth);
      float agarwal_measure(Rectangle& ground_truth);
      float pascal_measure(Rectangle& ground_truth);
      void outerBoundingBox(Rectangle & BBox2);

      void scale(float scalefactor);
      void scale(float width_scale, float height_scale);
      void halve(int n_times);
      void enlarge(float scalefactor);
      void enlarge(float scalefactor_x, float scalefactor_y);
      void enlargeX(float scalefactor);
      void enlargeY(float scalefactor);
      void reduceBy(int nbPixels);
      void reduceBy(int nbPixelsX, int nbPixelsY);
      float normalisedDistance(Rectangle bb);
      float unnormalisedDistance(Rectangle bb);
      float distance(Rectangle bb);

    };
} 



#endif  
