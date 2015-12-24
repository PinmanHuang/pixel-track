#include <string>
#include <sstream>
#include <algorithm>
#include <math.h>

#include "util.h"
#include "Rectangle.h"

using namespace std;

namespace TLImageProc {

  Rectangle::Rectangle(int first_col,int first_line, int last_col,int last_line){
    init(first_col,first_line,last_col,last_line);
  }

  Rectangle::Rectangle(string x_y_w_h)
  {
    stringstream ss(x_y_w_h);
    string sval;
    getline(ss, sval, ',');
    miFirstColumn=ROUND_TO_INT(atof(sval.c_str()));
    getline(ss, sval, ',');
    miFirstLine=ROUND_TO_INT(atof(sval.c_str()));
    getline(ss, sval, ',');
    miWidth=ROUND_TO_INT(atof(sval.c_str()));
    getline(ss, sval, ',');
    miHeight=ROUND_TO_INT(atof(sval.c_str()));
  }

  //     First lines and columns assumed to be 0
  Rectangle::Rectangle(int nb_columns,int nb_lines){
    init(0,0,nb_columns-1,nb_lines-1);
  }

  void Rectangle::init(int first_col,int first_line,int last_col,int last_line){
    miFirstLine=first_line; miFirstColumn=first_col;
    miHeight=last_line-first_line+1;
    miWidth=last_col-first_col+1;
    miWidth=max(miWidth,0);
    miHeight=max(miHeight,0);
  }


  void Rectangle::initPosAndSize(int first_col,int first_line, int width,int height){
    miFirstLine=first_line; miFirstColumn=first_col;
    miWidth=width;  miHeight=height;
  }

  void Rectangle::initCenterAndSize(int center_x,int center_y, int width,int height){
    miFirstLine=center_y-height/2; miFirstColumn=center_x-width/2;
    miWidth=width;  miHeight=height;
  }

  void Rectangle::initSize(int width,int height){
    miWidth=width;  miHeight=height;
  }

  void Rectangle::translate(int dep_co,int dep_li){
    miFirstLine+=dep_li;
    miFirstColumn+=dep_co;
  }

  bool Rectangle::inside(int column,int line){
    if(line>=miFirstLine && column>=miFirstColumn && line <= lastLine() && column <= lastColumn())
      return true;
    else
      return false;
  }

  bool Rectangle::inside(Rectangle smaller)
  {
    if (smaller.miFirstColumn>=miFirstColumn && smaller.miFirstLine>=miFirstLine && smaller.lastColumn()<=lastColumn() && smaller.lastLine()<=lastLine())
      return true;
    else
      return false;
  }

  int Rectangle::area(){
    return miWidth*miHeight;
  }

  void Rectangle::rotate(double angle, double& ulx, double& uly, double& urx, double& ury, double& lrx, double& lry, double& llx, double& lly)
  {
    double angle_rad = angle*M_PI/180.0;
    double cx = centerXf(), cy = centerYf();
    double x1 = miFirstColumn-cx, y1 = miFirstLine-cy, x2 = lastColumn()-cx, y2 = miFirstLine-cy, x3 = lastColumn()-cx, y3 = lastLine()-cy, x4 = miFirstColumn-cx, y4 = lastLine()-cy;
    
    ulx = x1*cos(angle_rad) - y1*sin(angle_rad) + cx; 
    uly = x1*sin(angle_rad) + y1*cos(angle_rad) + cy;
    urx = x2*cos(angle_rad) - y2*sin(angle_rad) + cx; 
    ury = x2*sin(angle_rad) + y2*cos(angle_rad) + cy;
    lrx = x3*cos(angle_rad) - y3*sin(angle_rad) + cx; 
    lry = x3*sin(angle_rad) + y3*cos(angle_rad) + cy;
    llx = x4*cos(angle_rad) - y4*sin(angle_rad) + cx; 
    lly = x4*sin(angle_rad) + y4*cos(angle_rad) + cy;
  }


  bool Rectangle::empty(){
    if(miWidth==0 || miHeight==0)
      return true;
    else
      return false;
  }

  void Rectangle::setEmpty(){
    init(0,0,0,0);
  }

  void Rectangle::add(int col,int li){
    if(empty()){
      miFirstColumn=col; miWidth = 1; miFirstLine=li; miHeight=1;
    }
    else {
      if(col<miFirstColumn) miFirstColumn=col;
      if(col>lastColumn()) miWidth += (col-lastColumn());
      if(li<miFirstLine) miFirstLine=li;
      if(li>lastLine()) miHeight += (li-lastLine());
    }
  }

  void Rectangle::add(float col,float  li){
    add((int)round(col),(int)round(li));
  }

  void Rectangle::intersection(Rectangle & BBox2){
    Rectangle B(
	max(firstColumn(),BBox2.firstColumn()),
	max(firstLine(),BBox2.firstLine()),
	min(lastColumn(),BBox2.lastColumn()),
	min(lastLine(),BBox2.lastLine())
	);
    (*this)=B;
  }

  bool Rectangle::isOverlapping(Rectangle& bb)
  {
    if ((firstColumn() < bb.firstColumn() && lastColumn() < bb.firstColumn()) || (bb.firstColumn() < firstColumn() && bb.lastColumn() < firstColumn()))
      return false;
    if ((firstLine() < bb.firstLine() && lastLine() < bb.firstLine()) || (bb.firstLine() < firstLine() && bb.lastLine() < firstLine()))
      return false;
    return true;
  }

  float Rectangle::precision(Rectangle& ground_truth)
  {
    // "this" object is detection result
    Rectangle tmp = *this;
    tmp.intersection(ground_truth);
    int a = area();
    if (a>0)
      return (float)tmp.area()/a;
    else
      return 0;
  }

  float Rectangle::recall(Rectangle& ground_truth)
  {
    // "this" object is detection result
    Rectangle tmp = *this;
    tmp.intersection(ground_truth);
    int a = ground_truth.area();
    if (a>0)
      return (float)tmp.area()/a;
    else 
      return 0;
  }

  float Rectangle::f_measure(Rectangle& ground_truth)
  {
    if (!isOverlapping(ground_truth))
      return 0;

    float res;
    Rectangle intersec = *this;
    intersec.intersection(ground_truth);
    res = (2.0*intersec.area())/(area()+ground_truth.area());
    return res;
  }


  float Rectangle::agarwal_measure(Rectangle& ground_truth)
  {
    Rectangle intersect = *this;
    intersect.intersection(ground_truth);
    float intersection_area = intersect.area();
    float gt_area = ground_truth.area();
    if (gt_area>0)
      return intersection_area/gt_area;
    else
      return 0.0;
  }

  float Rectangle::pascal_measure(Rectangle& ground_truth)
  {
    Rectangle intersect = *this;
    intersect.intersection(ground_truth);
    float intersection_area = intersect.area();
    float gt_area = ground_truth.area();
    float union_area = area()+gt_area-intersection_area;
    if (union_area>0)
      return intersection_area/union_area;
    else
      return 0.0;
  }

  void Rectangle::outerBoundingBox(Rectangle & BBox2){
    Rectangle B(min(firstColumn(),BBox2.firstColumn()),
	min(firstLine(),BBox2.firstLine()),
	max(lastColumn(),BBox2.lastColumn()),
	max(lastLine(),BBox2.lastLine())
	);
    (*this)=B;
  }

  void Rectangle::scale(float scalefactor)
  {
    miFirstColumn = int(miFirstColumn*scalefactor+.5);
    miFirstLine = int(miFirstLine*scalefactor+.5);
    miWidth = int(miWidth*scalefactor+.5);
    miHeight = int(miHeight*scalefactor+.5);
  }

  void Rectangle::halve(int n_times)
  {
    miFirstColumn = miFirstColumn>>n_times;
    miFirstLine = miFirstLine>>n_times;
    miWidth = miWidth>>n_times;
    miHeight = miHeight>>n_times;
  }

  void Rectangle::scale(float width_scale, float height_scale)
  {
    miFirstColumn = int(miFirstColumn*width_scale+.5);
    miFirstLine = int(miFirstLine*height_scale+.5);
    miWidth = int(miWidth*width_scale+.5);
    miHeight = int(miHeight*height_scale+.5);
  }

  // scales the BB around the centre
  void Rectangle::enlarge(float scalefactor)
  {
    Rectangle bbtmp;
    bbtmp.miFirstColumn = ROUND_TO_INT(centerXf()-(float)miWidth/2.0*scalefactor);
    bbtmp.miFirstLine = ROUND_TO_INT(centerYf()-(float)miHeight/2.0*scalefactor);
    bbtmp.miWidth = int(miWidth*scalefactor+.5);
    bbtmp.miHeight = int(miHeight*scalefactor+.5);
    *this = bbtmp;
  }

  // scales the BB around the centre
  void Rectangle::enlarge(float scalefactor_x, float scalefactor_y)
  {
    Rectangle bbtmp;
    bbtmp.miFirstColumn = int(centerX()-(float)miWidth/2*scalefactor_x);
    bbtmp.miFirstLine = int(centerY()-(float)miHeight/2*scalefactor_y);
    bbtmp.miWidth = int(miWidth*scalefactor_x+.5);
    bbtmp.miHeight = int(miHeight*scalefactor_y+.5);
    *this = bbtmp;
  }

  // scales the height of the BB around the centre
  void Rectangle::enlargeX(float scalefactor)
  {
    Rectangle bbtmp;
    bbtmp.miFirstLine = miFirstLine;
    bbtmp.miFirstColumn = int(centerX()-(float)miWidth/2*scalefactor);
    bbtmp.miHeight = miHeight;
    bbtmp.miWidth = int(miWidth*scalefactor+.5);
    *this = bbtmp;
  }

  // scales the height of the BB around the centre
  void Rectangle::enlargeY(float scalefactor)
  {
    Rectangle bbtmp;
    bbtmp.miFirstColumn = miFirstColumn;
    bbtmp.miFirstLine = int(centerY()-(float)miHeight/2*scalefactor);
    bbtmp.miWidth = miWidth;
    bbtmp.miHeight = int(miHeight*scalefactor+.5);
    *this = bbtmp;
  }

  void Rectangle::reduceBy(int nbPixels)
  {
    int neww, newh;
    neww = miWidth-2*nbPixels;
    newh = miHeight-2*nbPixels;
    if (neww<=0)
    {
      miFirstColumn=centerX();
      miWidth=0;
    }
    else
    {
      miWidth=neww;
      miFirstColumn+=nbPixels;
    }
    if (newh<=0)
    {
      miFirstLine=centerY();
      miHeight=0;
    }
    else
    {
      miHeight=newh;
      miFirstLine+=nbPixels;
    }
  }

  void Rectangle::reduceBy(int nbPixelsX, int nbPixelsY)
  {
    int neww, newh;
    neww = miWidth-2*nbPixelsX;
    newh = miHeight-2*nbPixelsY;
    if (neww<=0)
    {
      miFirstColumn=centerX();
      miWidth=0;
    }
    else
    {
      miWidth=neww;
      miFirstColumn+=nbPixelsX;
    }
    if (newh<=0)
    {
      miFirstLine=centerY();
      miHeight=0;
    }
    else
    {
      miHeight=newh;
      miFirstLine+=nbPixelsY;
    }
  }

  // returns a distance measure normalised by the widths of both BBs
  float Rectangle::normalisedDistance(Rectangle bb)
  {
    float dx1 = bb.centerX()-centerX();
    float dy1 = bb.centerY()-centerY();
    float dx2 = dx1/bb.miWidth;
    float dy2 = dy1/bb.miHeight;
    dx1 /= miWidth;
    dy1 /= miHeight;
    return 0.5*(sqrtf(dx1*dx1+dy1*dy1)+sqrtf(dx2*dx2+dy2*dy2));
  }
  
  // returns the unnormalised distance between the centres of two rectangles
  float Rectangle::unnormalisedDistance(Rectangle bb)
  {
    float dx = bb.centerX()-centerX();
    float dy = bb.centerY()-centerY();
    return sqrtf(dx*dx+dy*dy);
  }
  
  // returns a distance measure normalised by the width of this Rectangle
  float Rectangle::distance(Rectangle bb)
  {
    float dx1 = float(bb.centerX()-centerX())/miWidth;
    float dy1 = float(bb.centerY()-centerY())/miHeight;
    return sqrtf(dx1*dx1+dy1*dy1);
  }

  void Rectangle::setCenter(int x, int y)
  {
    miFirstColumn = x-miWidth/2;
    miFirstLine = y-miHeight/2;
  }

} 

