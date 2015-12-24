#ifndef TL_IMAGE_H
#define TL_IMAGE_H

#include <float.h>
#include <sys/time.h>
#include <string.h>
#include "tltypes.h"
#include "Rectangle.h"
#include "Error.h"
#include "Convert.h"

using namespace TLUtil;

namespace TLImageProc
{


template<class Type>
class Image
{
  private:
    int miWidth;
    int miHeight;
    int miChannels;
    int miWidthStep;
    bool mbExternalData;;
    Rectangle mBB;
    Type* mData;  

  public:
    Image<Type>(int width, int height, int channels);
    Image<Type>(int width, int height, int channels, int widthstep, Type* data, bool externaldata=true);
    Image(int width, int height, int channels, int widthstep, Type* data, int datawidth, int dataheight); // adding black border
    Image<Type>(int width, int height, int channels, int widthstep, Type** data);
    ~Image();

    int height() { return miHeight; };
    int width() { return miWidth; };
    int widthStep() { return miWidthStep; };
    int nChannels() { return miChannels; };
    Type* data() { return mData; };
    Type operator()(int x, int y) { return get(x,y); };
    Type get(int x, int y) { return *(mData+miWidthStep*y + x); };
    Type get(int x, int y, int channel) { return *(mData+miWidthStep*y + miChannels*x + channel); };
    void set(int x, int y, Type value) { ASSERT(miChannels==1, "Image::set() only for single channel images."); *(mData+miWidthStep*y + x) = value; };
    void set(int x, int y, int channel, Type value) { *(mData+miWidthStep*y + x*miChannels + channel) = value; };
    void add(int x, int y, Type value) { *(mData+miWidthStep*y + x) += value; };
    void inc(int x, int y) { (*(mData+miWidthStep*y + x))++; };
    void setColourBGR(int x, int y, Colour c);
    Colour getColourBGR(int x, int y);
    void init(Type value);
    void init(Type value, Rectangle roi);
    void setZero();
    Image<Type>* clone();
    void copy(Image<Type>* res);
    void copy(Image<Type>* res, int dx, int dy, bool clear_margin=true);
    Image<Type>* resize(Rectangle roi, int dest_width, int dest_height);
    void resize(Rectangle roi, Image<Type>* dest);
    void resize(Rectangle roi, float** dest, int dest_width, int dest_height, float factor, float offset);
    void resize(Rectangle roi, double** dest, int dest_width, int dest_height, float factor, float offset);
    void resizeToGrey(Rectangle roi, double** dest, int dest_width, int dest_height, float factor, float offset);
    void resizeToHSV(Rectangle roi, double*** dest, int dest_width, int dest_height, float factor, float offset);
    void rotateAndResizeToGrey(Rectangle roi, double** dest, int dest_width, int dest_height, float angle, float factor, float offset);
    void rotateAndResizeToHSV(Rectangle roi, double*** dest, int dest_width, int dest_height, float angle, float factor, float offset);
    void rotateAndResizeToHSV(Rectangle roi, Image<Type>* dest, float angle);
    void resizeToColourNames(Rectangle roi, double*** dest, int dest_width, int dest_height, float factor, float offset, double** colour_lut, int clt_nlines, int clt_shift);
    Image<Type>* resizeBilinear(Rectangle srcroi, int dest_width, int dest_height);
    void resizeBilinear(Rectangle srcroi, Image<Type>* dest);
    Image<Type>* flip();
    void translate(int dx, int dy);

    Image<unsigned char>* toGreyScale();
    bool toGreyScale(Image<unsigned char>* res);

    void multiply(Type f);
    void multiply(Type f, Image<Type>* result);
    void multiply(Image<Type>* i2, Image<Type>* result);
    void add(Type a, Rectangle roi);
    void update(Image<Type>* upd_img, float update_factor);
    void normaliseByMax();

    void sobelX(Image<short>* result);
    void sobelY(Image<short>* result);
    void gaussianBlur(Image<Type>* result);
    void average(Image<Type>* result);
    void binarise(float thresh);
    float entropy(Rectangle roi);
    float varianceFromCentre(Rectangle roi);
    Type maxLoc(Rectangle roi, int& maxx, int& maxy);
    void centreOfMass(Rectangle roi, int& maxx, int& maxy);
    Type sum(Rectangle roi);
    double average(Rectangle roi) { return (double)sum(roi)/roi.area(); };
    int sumGreaterThanThreshold(Rectangle roi, float thresh);

    Image<Type>* equaliseHist();
    float uniformity(Rectangle roi);
    float percentageChanged(Rectangle roi, int offx, int offy, Image<Type>* img);
    void integralImage(Image<double>* result);
    void cumulativeSum(Image<double>* result);
    void cumulativeSum(Image<double>* int_img, Rectangle scan_win, Image<double>* result);
    void sumSlidingWindow(Image<double>* int_img, Rectangle scan_win, Image<double>* result);
    Rectangle sampleRegion(int width, int height);
    void sample(int& x, int& y);

    float crossCorrelation(Image<float>* pattern, Rectangle roi, float scale_pattern_x, float scale_pattern_y, Rectangle* pattern_roi, int& maxx, int& maxy);
    float fastCrossCorrelation(Image<float>* pattern, Rectangle roi, float scale_pattern_x, float scale_pattern_y, Rectangle* pattern_roi, int& maxx, int& maxy);

	/**
	 * Write image data to file.
	 * For floating point image, only xml and yml file name extension is allowed.
	 */
	//void writeToFile(const std::string& filename);
};


typedef Image<unsigned char> Image8U;


/*
template<class Type1, class Type2>
void castImage(Image<Type1>* src, Image<Type2>* dst)
{
}
*/


template<class Type>
Image<Type>::Image(int width, int height, int channels)
{
  mData = new Type[width*height*channels];
  mbExternalData = false;
  miWidth = width;
  miWidthStep = width*channels;
  miHeight = height;
  miChannels = channels;
  mBB.initPosAndSize(0, 0, miWidth, miHeight);
}


template<class Type>
Image<Type>::Image(int width, int height, int channels, int widthstep, Type* data, bool externaldata/*=true*/)
{
  miWidth = width;
  miWidthStep = widthstep;
  miHeight = height;
  miChannels = channels;
  mbExternalData = externaldata;

  if (externaldata)
    mData = data;
  else
  {
    mData = new Type[miHeight*miWidthStep];
    memcpy(mData, data, miHeight*miWidthStep*sizeof(Type));
  }
  mBB.initPosAndSize(0, 0, miWidth, miHeight);
}


// adding black border
template<class Type>
Image<Type>::Image(int width, int height, int channels, int widthstep, Type* data, int datawidth, int dataheight)
{
  miWidth = width;
  miWidthStep = width*channels;
  miHeight = height;
  miChannels = channels;
  mbExternalData = false;
  int offsetx = (width-datawidth)/2, offsety = (height-dataheight)/2;

  mData = new Type[miHeight*miWidthStep];
  setZero();

  Type* dstline = mData + offsety*miWidthStep + offsetx*miChannels;
  Type* srcline = data;
  for(int i=0; i<dataheight; i++)
  {
    memcpy(dstline, srcline, widthstep*sizeof(Type));
    dstline += miWidthStep;
    srcline += widthstep;
  }

  mBB.initPosAndSize(0, 0, miWidth, miHeight);
}


template<class Type>
Image<Type>::Image(int width, int height, int channels, int widthstep, Type** data)
{
  miWidth = width;
  miWidthStep = widthstep;
  miHeight = height;
  miChannels = channels;
  mbExternalData = false;

  mData = new Type[miHeight*miWidthStep];
  for(int i=0; i<height; i++)
    for(int j=0; j<width; j++)
      set(j, i, data[i][j]);

  mBB.initPosAndSize(0, 0, miWidth, miHeight);
}


template<class Type>
Image<Type>::~Image()
{
  if (!mbExternalData)
    delete [] mData;
}


template<typename Type>
void Image<Type>::init(Type value)
{
  int size = miWidthStep*miHeight;
  Type* ptr = mData;
  for(int i=0; i<size; i++)
    *ptr++ = value;
}


template<typename Type>
void Image<Type>::init(Type value, Rectangle roi)
{
  //ASSERT(miChannels==1, "Image::init(Type,Rectangle) requires a single-channel image.");

  roi.intersection(mBB);

  Type* ptr = mData + miWidthStep*roi.miFirstLine + roi.miFirstColumn*miChannels;
  int linestep = miWidthStep-roi.miWidth*miChannels;

  for(int i=0; i<roi.miHeight; i++)
  {
    for(int j=0; j<roi.miWidth; j++)
      for(int c=0; c<miChannels; c++)
      *ptr++ = value;
    ptr+=linestep;
  }
}


template<typename Type>
void Image<Type>::setZero()
{
  memset(mData, 0, miWidthStep*miHeight*sizeof(Type));
}

template<typename Type>
void Image<Type>::setColourBGR(int x, int y, Colour c)
{
  Type* data_ptr = mData+(y*miWidthStep*sizeof(Type))+x*sizeof(Type)*3;
  data_ptr[0] = c.b;
  data_ptr[1] = c.g;
  data_ptr[2] = c.r;
}

template<typename Type>
Colour Image<Type>::getColourBGR(int x, int y)
{
  Type* data_ptr = mData+(y*miWidthStep*sizeof(Type))+x*sizeof(Type)*3;
  return Colour(data_ptr[2], data_ptr[1], data_ptr[0]);;
}


template<class Type>
Image<Type>* Image<Type>::clone()
{
  return new Image<Type>(miWidth, miHeight, miChannels, miWidthStep, mData, false);
}

template<class Type>
void Image<Type>::copy(Image<Type>* res)
{
  ASSERT(miChannels==res->nChannels(), "Image::clone(): Image types not compatible.");
  ASSERT(miWidth==res->width() && miHeight==res->height(), "Image::clone(): Image sizes not compatible.");
  memcpy(res->data(), mData, miHeight*miWidthStep*sizeof(Type));
}




/**
 * Copies the image into @a res, shifting it by @a dx, @a dy pixels.
 * The images must have the same depth.
 * If @a clear_margin is true, the  margins are cleared, i.e. set to 0.
 */
template<class Type>
void Image<Type>::copy(Image<Type>* res, int dx, int dy, bool clear_margin/*=true*/)
{
  ASSERT(miChannels==res->nChannels(), "Image::copy() requires images with the same number of channels.");

  if (clear_margin)
    res->setZero();

  unsigned char* ptr = mData + std::max(0, -dy)*miWidthStep + std::max(0, -dx)*miChannels;
  unsigned char* resptr = res->mData + std::max(0, dy)*res->miWidthStep + std::max(0, dx)*miChannels;
  int w = std::min(miWidth, std::min(res->miWidth, miWidth+dx) - std::max(0, dx)); 
  int h = std::min(miHeight, std::min(res->miHeight, miHeight+dy) - std::max(0, dy)); 
  if (w<=0 || h<=0)
    return;
  int xstep = miWidthStep-miWidth*miChannels + (miWidth-w)*miChannels;
  int res_xstep = res->miWidthStep-res->miWidth*res->miChannels + (res->miWidth-w)*res->miChannels;

  if (res==this && (dy>0 || (dy==0 && dx>0)))
  {
    // when copying onto same image, start from the end in order not to erase the image
    /*
    ptr += h*(w*miChannels+xstep)-xstep-1;
    resptr += h*(w*res->miChannels+res_xstep)-res_xstep-1;
    */
    ptr += h*miWidthStep - xstep -1;
    resptr += h*res->miWidthStep - res_xstep -1;
    for(int i=0; i<h; i++)
    {
      for(int j=0; j<w; j++)
      {
	for(int c=0; c<miChannels; c++)
	  *resptr-- = *ptr--;
      }
      ptr-=xstep;
      resptr-=res_xstep;
    }
  }
  else
  {
    for(int i=0; i<h; i++)
    {
      for(int j=0; j<w; j++)
      {
	for(int c=0; c<miChannels; c++)
	  *resptr++ = *ptr++;
      }
      ptr+=xstep;
      resptr+=res_xstep;
    }
  }
}



template<>
inline bool Image<unsigned char>::toGreyScale(Image<unsigned char>* res)
{
  if (miChannels==3)  // colour image (assuming BGR)
  {
    unsigned char* ptr = mData;
    unsigned char* resptr = res->mData;
    int r, g, b;
    int xstep = miWidthStep-miWidth*3;
    int res_xstep = res->miWidthStep-res->miWidth;
    for(int i=0; i<miHeight; i++)
    {
      for(int j=0; j<miWidth; j++)
      {
	b = *ptr++;
	g = *ptr++;
	r = *ptr++;
        *resptr++ = 0.299*r+0.587*g+0.114*b;
      }
      ptr+=xstep;
      resptr+=res_xstep;
    }
    return true;
  }
  else if (miChannels==1)
  {
    ASSERT(miWidthStep = res->miWidthStep, "Grey scale images have different width step in Image::toGreyScale()");
    memcpy(res->mData, mData, miHeight*miWidthStep);
    return true;
  }
  return false;
}


template<>
inline Image<unsigned char>* Image<unsigned char>::toGreyScale()
{
  Image<unsigned char>* res = new Image<unsigned char>(miWidth, miHeight, 1);
  if (toGreyScale(res))
    return res;
  else 
    return NULL;
}


template<class Type>
void Image<Type>::multiply(Type f)
{
  int size = miWidthStep*miHeight;
  Type* ptr = mData;
  for(int i=0; i<size; i++)
    *ptr++ *= f;
}

template<class Type>
void Image<Type>::multiply(Type f, Image<Type>* result)
{
  int size = miWidthStep*miHeight;
  Type* ptr = mData;
  Type* optr = result->data();
  for(int i=0; i<size; i++)
    *optr++ = *ptr++ * f;
}

template<class Type>
void Image<Type>::multiply(Image<Type>* i2, Image<Type>* result)
{
  ASSERT(miWidth==i2->width() && miHeight==i2->height(), "Image::multiply() requires second image of same dimension.");
  ASSERT(miWidth==result->width() && miHeight==result->height(), "Image::multiply() requires result image of same dimension.");
  int size = miWidthStep*miHeight;
  Type* ptr = mData;
  Type* ptr2 = i2->data();;
  Type* optr = result->data();
  for(int i=0; i<size; i++)
    *optr++ = sqrtf(*ptr++ * *ptr2++);
}


template<class Type>
void Image<Type>::add(Type a, Rectangle roi)
{
  ASSERT(miChannels==1, "Image::add() requires a single-channel image.");

  roi.intersection(mBB);

  Type* ptr = mData + miWidthStep*roi.miFirstLine + roi.miFirstColumn;
  int linestep = miWidthStep-roi.miWidth;

  for(int i=0; i<roi.miHeight; i++)
  {
    for(int j=0; j<roi.miWidth; j++)
      *ptr++ += a;
    ptr+=linestep;
  }
}

template<class Type>
void Image<Type>::update(Image<Type>* upd_img, float update_factor)
{
  ASSERT(miChannels==upd_img->nChannels(), "Image::update() requires images with the same number of channels.");
  ASSERT(miWidth==upd_img->width() && miHeight==upd_img->height(), "Image::updated() requires images of same dimension.");

  Type* src = upd_img->data();
  Type* dst = mData;
  int src_linestep = upd_img->widthStep()-upd_img->width()*upd_img->nChannels();
  int dst_linestep = miWidthStep-miWidth*nChannels();
  float one_minus_uf = 1.0-update_factor;

  for(int i=0; i<miHeight; i++)
  {
    for(int j=0; j<miWidth; j++)
    {
      *dst = update_factor*(*src++) + one_minus_uf*(*dst);
      dst++;
    }
    dst+=dst_linestep;
    src+=src_linestep;
  }
}


template<class Type>
void Image<Type>::gaussianBlur(Image<Type>* result)
{
  Type *i1, *i2, *i3, *i4, *i5, *i6, *i7, *i8, *i9;
  Type *out;
  int instep, outstep;
  int out_widthstep = result->widthStep();;
  Type* input = mData;
  Type* output = result->data();

  instep = miWidthStep-(miWidth+2)*miChannels;
  outstep = out_widthstep-(miWidth+2)*miChannels;

  i1 = input;
  i2 = input+1*miChannels;
  i3 = input+2*miChannels;
  i4 = input+miWidthStep;
  i5 = input+miWidthStep+1*miChannels;  // centre pixel
  i6 = input+miWidthStep+2*miChannels;
  i7 = input+2*miWidthStep;
  i8 = input+2*miWidthStep+1*miChannels;
  i9 = input+2*miWidthStep+2*miChannels;

  out = output+out_widthstep+1*miChannels;
  for(int i=0; i<miHeight-2; i++)
  {
    for(int j=0; j<miWidth-2; j++)
    {
      for(int c=0; c<miChannels; c++)
      {
	*out = (Type)((1.0/16)*(*i1 + 2*(*i2) + *i3 + 2*(*i4) + 4*(*i5) + 2*(*i6) + *i7 + 2*(*i8) + *i9));
	out++;
	i1++; i2++; i3++; i4++; i5++; i6++; i7++; i8++; i9++;
      }
    }
    out+=outstep;
    i1+=instep;
    i2+=instep;
    i3+=instep;
    i4+=instep;
    i5+=instep;
    i6+=instep;
    i7+=instep;
    i8+=instep;
    i9+=instep;
  }
}


template<class Type>
void Image<Type>::sobelX(Image<short>* result)
{
  Type *i1, *i2, *i3, *i4, *i5, *i6, *i7, *i8, *i9;
  short *out;
  int instep, outstep;
  int out_widthstep = result->widthStep();;
  Type* input = mData;
  short* output = result->data();

  instep = miWidthStep-miWidth+2;
  outstep = out_widthstep-miWidth+2;

  i1 = input;
  //i2 = input+1;
  i3 = input+2;
  i4 = input+miWidthStep;
  //i5 = input+miWidthStep+1;  // centre pixel
  i6 = input+miWidthStep+2;
  i7 = input+2*miWidthStep;
  //i8 = input+2*miWidthStep+1;
  i9 = input+2*miWidthStep+2;

  out = output+out_widthstep+1;
  for(int i=0; i<miHeight-2; i++)
  {
    for(int j=0; j<miWidth-2; j++)
    {
      *out = (short)(*i3 + 2*(*i6) + *i9 - *i1 - 2*(*i4) - *i7);
      out++;
      i1++; i3++; i4++; i6++; i7++; i9++;
    }
    out+=outstep;
    i1+=instep;
    //i2+=instep;
    i3+=instep;
    i4+=instep;
    //i5+=instep;
    i6+=instep;
    i7+=instep;
    //i8+=instep;
    i9+=instep;
  }
}


template<class Type>
void Image<Type>::sobelY(Image<short>* result)
{
  Type *i1, *i2, *i3, *i4, *i5, *i6, *i7, *i8, *i9;
  short *out;
  int instep, outstep;
  int out_widthstep = result->widthStep();;
  Type* input = mData;
  short* output = result->data();

  instep = miWidthStep-miWidth+2;
  outstep = out_widthstep-miWidth+2;

  i1 = input;
  i2 = input+1;
  i3 = input+2;
  //i4 = input+miWidthStep;
  //i5 = input+miWidthStep+1; // centre pixel
  //i6 = input+miWidthStep+2;
  i7 = input+2*miWidthStep;
  i8 = input+2*miWidthStep+1;
  i9 = input+2*miWidthStep+2;

  out = output+out_widthstep+1;
  for(int i=0; i<miHeight-2; i++)
  {
    for(int j=0; j<miWidth-2; j++)
    {
      *out = (short)(*i7 + 2*(*i8) + *i9 - *i1 - 2*(*i2) - *i3);
      out++;
      i1++; i2++; i3++; i7++; i8++; i9++;
    }
    out+=outstep;
    i1+=instep;
    i2+=instep;
    i3+=instep;
    //i4+=instep;
    //i5+=instep;
    //i6+=instep;
    i7+=instep;
    i8+=instep;
    i9+=instep;
  }
}


template<class Type>
void Image<Type>::average(Image<Type>* result)
{
  Type *i1, *i2, *i3, *i4, *i5, *i6, *i7, *i8, *i9;
  short *out;
  int instep, outstep;
  int out_widthstep = result->miWidthStep;
  Type* input = mData;
  Type* output = result->mData;

  instep = miWidthStep-miWidth+2;
  outstep = out_widthstep-miWidth+2;

  i1 = input;
  i2 = input+1;
  i3 = input+2;
  i4 = input+miWidthStep;
  i5 = input+miWidthStep+1; // centre pixel
  i6 = input+miWidthStep+2;
  i7 = input+2*miWidthStep;
  i8 = input+2*miWidthStep+1;
  i9 = input+2*miWidthStep+2;

  out = output+out_widthstep+1;
  for(int i=0; i<miHeight-2; i++)
  {
    for(int j=0; j<miWidth-2; j++)
    {
      *out = (Type)(((double) *i1 + *i2 + *i3 + *i4 + *i5 + *i6 + *i7 + *i8 + *i9)*0.111111111);
      out++;
      i1++; i2++; i3++; i4++; i5++; i6++; i7++; i8++; i9++;
    }
    out+=outstep;
    i1+=instep;
    i2+=instep;
    i3+=instep;
    i4+=instep;
    i5+=instep;
    i6+=instep;
    i7+=instep;
    i8+=instep;
    i9+=instep;
  }
}

template<class Type>
Type Image<Type>::maxLoc(Rectangle roi, int& maxx, int& maxy)
{
  ASSERT(miChannels==1, "Image::maxLoc() requires a single-channel image.");

  roi.intersection(mBB);

  Type* ptr = mData + miWidthStep*roi.miFirstLine + roi.miFirstColumn;
  int linestep = miWidthStep-roi.miWidth;
  Type maxval = *ptr;
  Type val;
  int fl, fc, ll, lc;
  fl = roi.miFirstLine;
  fc = roi.miFirstColumn;
  ll = roi.lastLine();
  lc = roi.lastColumn();
  maxx=roi.miFirstColumn;
  maxy=roi.miFirstLine;

  for(int i=fl; i<=ll; i++)
  {
    for(int j=fc; j<=lc; j++)
    {
      val = *ptr++;
      if (val>maxval)
      {
	maxval = val;
	maxx = j;
	maxy = i;
      }
    }
    ptr+=linestep;
  }
  return maxval;
}


template<class Type>
void Image<Type>::centreOfMass(Rectangle roi, int& maxx, int& maxy)
{
  ASSERT(miChannels==1, "Image::centreOfMass() requires a single-channel image.");

  roi.intersection(mBB);

  Type* ptr = mData + miWidthStep*roi.miFirstLine + roi.miFirstColumn;
  int linestep = miWidthStep-roi.miWidth;
  Type maxval = *ptr;
  Type val;
  int fl, fc, ll, lc;
  fl = roi.miFirstLine;
  fc = roi.miFirstColumn;
  ll = roi.lastLine();
  lc = roi.lastColumn();
  float xsum=0, ysum=0;
  float fmaxx, fmaxy;
  fmaxx=fmaxy=0;

  for(int i=fl; i<=ll; i++)
  {
    for(int j=fc; j<=lc; j++)
    {
      val = *ptr++;
      fmaxx+=val*j;
      fmaxy+=val*i;
      xsum+=val;
      ysum+=val;
    }
    ptr+=linestep;
  }
  if (xsum==0 || ysum==0)
  {
    maxx = roi.centerX();
    maxy = roi.centerY();
  }
  else
  {
    maxx=roundf(fmaxx/xsum);
    maxy=roundf(fmaxy/ysum);
  }
}



template<class Type>
void Image<Type>::binarise(float thresh)
{
  ASSERT(miChannels==1, "Image::maxLoc() requires a single-channel image.");

  Type* ptr = mData;
  Type val;
  int linestep = miWidthStep-miWidth;

  for(int i=0; i<miHeight; i++)
  {
    for(int j=0; j<miWidth; j++)
    {
      val = *ptr;
      if (val>thresh)
	*ptr = 1.0;
      else
	*ptr = 0.0;
      ptr++; 
    }
    ptr+=linestep;
  }
}



template<class Type>
float Image<Type>::entropy(Rectangle roi)
{
  ASSERT(miChannels==1, "Image::maxLoc() requires a single-channel image.");

  roi.intersection(mBB);

  Type* ptr = mData + miWidthStep*roi.miFirstLine + roi.miFirstColumn;
  int linestep = miWidthStep-roi.miWidth;
  Type val;
  int fl, fc, ll, lc;
  fl = roi.miFirstLine;
  fc = roi.miFirstColumn;
  ll = roi.lastLine();
  lc = roi.lastColumn();
  float result;
  float sumroi = sum(roi); 

  for(int i=fl; i<=ll; i++)
  {
    for(int j=fc; j<=lc; j++)
    {
      val = *ptr++;
      val/=sumroi;
      if (val>0)
	result -= val*log(val);
    }
    ptr+=linestep;
  }
  return result/roi.area();
}

template<class Type>
Type Image<Type>::sum(Rectangle roi)
{
  ASSERT(miChannels==1, "Image::sum() requires a single-channel image.");

  roi.intersection(mBB);

  Type* ptr = mData + miWidthStep*roi.miFirstLine + roi.miFirstColumn;
  int linestep = miWidthStep-roi.miWidth;
  Type val;
  int fl, fc, ll, lc;
  fl = roi.miFirstLine;
  fc = roi.miFirstColumn;
  ll = roi.lastLine();
  lc = roi.lastColumn();
  Type result=0;

  for(int i=fl; i<=ll; i++)
  {
    for(int j=fc; j<=lc; j++)
    {
      result += *ptr++;
    }
    ptr+=linestep;
  }
  return result;
}



template<class Type>
float Image<Type>::varianceFromCentre(Rectangle roi)
{
  ASSERT(miChannels==1, "Image::maxLoc() requires a single-channel image.");

  roi.intersection(mBB);

  Type* ptr = mData + miWidthStep*roi.miFirstLine + roi.miFirstColumn;
  int linestep = miWidthStep-roi.miWidth;
  Type val;
  int fl, fc, ll, lc;
  fl = roi.miFirstLine;
  fc = roi.miFirstColumn;
  ll = roi.lastLine();
  lc = roi.lastColumn();
  float result=0;
  float dx, dy;
  float w2 = 0.5*roi.miWidth, h2 = 0.5*roi.miHeight;
  float dist;
  float sumroi = sum(roi); 
  if (sumroi==0)
    return 0;

  for(int i=fl; i<=ll; i++)
  {
    dy = i-h2;
    for(int j=fc; j<=lc; j++)
    {
      dx = j-w2;
      dist = (dx*dx+dy*dy);

      val = *ptr++/sumroi;
      result += dist*val;
    }
    ptr+=linestep;
  }
  return result;
}



template<class Type>
Image<Type>* Image<Type>::resize(Rectangle roi, int dest_width, int dest_height)
{
  Image<Type>* dest = new Image<Type>(dest_width, dest_height, miChannels);
  float source_pixsize_x = (float)roi.miWidth/dest_width;
  float source_pixsize_y = (float)roi.miHeight/dest_height;
  float source_x, source_y;
  int isource_x, isource_y;

  // no interpolation !!!
  source_y=roi.miFirstLine;
  for(int i=0; i<dest_height; i++)
  {
    isource_y = int(source_y+.5);
    source_x=roi.miFirstColumn;
    for(int j=0; j<dest_width; j++)
    {
      isource_x = int(source_x+.5)*miChannels;
      for(int c=0; c<miChannels; c++)
      {
        dest->set(j, i, c, *(mData+miWidthStep*isource_y + isource_x + c)); 
      }
      source_x += source_pixsize_x;
    }
    source_y += source_pixsize_y;
  }

  return dest;
}


template<class Type>
void Image<Type>::resize(Rectangle roi, Image<Type>* dest)
{
  if (dest==this)
    return;

  if (miWidth==dest->width() && miHeight==dest->height())
  {
    copy(dest);
    return;
  }

  roi.intersection(mBB);
  if (roi.area()==0)
    return;

  int dest_width = dest->width();
  int dest_height = dest->height();
  float source_pixsize_x = (float)roi.miWidth/dest_width;
  float source_pixsize_y = (float)roi.miHeight/dest_height;
  float source_x, source_y;
  int isource_x, isource_y;

  // no interpolation !!!
  source_y=roi.miFirstLine;
  for(int i=0; i<dest_height; i++)
  {
    isource_y = int(source_y+.5);
    source_x=roi.miFirstColumn;
    for(int j=0; j<dest_width; j++)
    {
      isource_x = int(source_x+.5)*miChannels;
      for(int c=0; c<miChannels; c++)
      {
        dest->set(j, i, c, *(mData+miWidthStep*isource_y + isource_x + c)); 
      }
      source_x += source_pixsize_x;
    }
    source_y += source_pixsize_y;
  }
}


template<class Type>
void Image<Type>::resize(Rectangle roi, float** dest, int dest_width, int dest_height, float factor, float offset)
{
  ASSERT(miChannels==1, "Image::resize() requires a single-channel image.");
  roi.intersection(mBB);
  if (roi.area()==0)
    return;

  float source_pixsize_x = (float)roi.miWidth/dest_width;
  float source_pixsize_y = (float)roi.miHeight/dest_height;
  float source_x, source_y;
  int isource_x, isource_y;

  // no interpolation !!!
  source_y=roi.miFirstLine;
  for(int i=0; i<dest_height; i++)
  {
    isource_y = int(source_y+.5);
    source_x=roi.miFirstColumn;
    for(int j=0; j<dest_width; j++)
    {
      isource_x = int(source_x+.5)*miChannels;
      dest[i][j] = *(mData+miWidthStep*isource_y + isource_x) * factor + offset;
      source_x += source_pixsize_x;
    }
    source_y += source_pixsize_y;
  }
}



template<class Type>
void Image<Type>::resize(Rectangle roi, double** dest, int dest_width, int dest_height, float factor, float offset)
{
  ASSERT(miChannels==1, "Image::resize() requires a single-channel image.");
  roi.intersection(mBB);
  if (roi.area()==0)
    return;

  float source_pixsize_x = (float)roi.miWidth/dest_width;
  float source_pixsize_y = (float)roi.miHeight/dest_height;
  float source_x, source_y;
  int isource_x, isource_y;

  // no interpolation !!!
  source_y=roi.miFirstLine;
  for(int i=0; i<dest_height; i++)
  {
    isource_y = int(source_y+.5);
    source_x=roi.miFirstColumn;
    for(int j=0; j<dest_width; j++)
    {
      isource_x = int(source_x+.5)*miChannels;
      dest[i][j] = *(mData+miWidthStep*isource_y + isource_x) * factor + offset;
      source_x += source_pixsize_x;
    }
    source_y += source_pixsize_y;
  }
}


template<class Type>
void Image<Type>::resizeToGrey(Rectangle roi, double** dest, int dest_width, int dest_height, float factor, float offset)
{
  ASSERT(miChannels==3, "Image::resizeToGrey() requires a 3-channel (colour) image.");
  roi.intersection(mBB);
  if (roi.area()==0)
    return;

  float source_x, source_y, source_pixsize_x = (float)roi.miWidth/dest_width, source_pixsize_y = (float)roi.miHeight/dest_height;
  int isource_x, isource_y, source_cx = roi.centerX(), source_cy = roi.centerY();;
  unsigned char r, g, b;
  double ffactor = factor/3.0;

  // no interpolation !!!
  for(int i=0; i<dest_height; i++)
  {
    for(int j=0; j<dest_width; j++)
    {
      isource_x = roi.miFirstColumn + j*source_pixsize_x;
      isource_y = roi.miFirstLine + i*source_pixsize_y;
      r = *(mData+miWidthStep*isource_y + isource_x*miChannels);
      g = *(mData+miWidthStep*isource_y + isource_x*miChannels+1);
      b = *(mData+miWidthStep*isource_y + isource_x*miChannels+2);
      dest[i][j] = ffactor * (r+g+b) + offset;
    }
  }
}


template<class Type>
void Image<Type>::resizeToHSV(Rectangle roi, double*** dest, int dest_width, int dest_height, float factor, float offset)
{
  ASSERT(miChannels==3, "Image::resize() requires a 3-channel (colour) image.");
  roi.intersection(mBB);
  if (roi.area()==0)
    return;

  float source_pixsize_x = (float)roi.miWidth/dest_width;
  float source_pixsize_y = (float)roi.miHeight/dest_height;
  float source_x, source_y;
  int isource_x, isource_y;
  unsigned char r, g, b, h, s, v;

  // no interpolation !!!
  source_y=roi.miFirstLine;
  for(int i=0; i<dest_height; i++)
  {
    isource_y = int(source_y+.5);
    source_x=roi.miFirstColumn;
    for(int j=0; j<dest_width; j++)
    {
      isource_x = int(source_x+.5)*miChannels;
      r = *(mData+miWidthStep*isource_y + isource_x);
      g = *(mData+miWidthStep*isource_y + isource_x+1);
      b = *(mData+miWidthStep*isource_y + isource_x+2);
      RGBtoHSV(r, g, b, h, s, v);
      //RGBtoYCbCr(r, g, b, h, s, v);
      //RGBtoXYZ(r, g, b, h, s, v);
      dest[0][i][j] = h * factor + offset;
      dest[1][i][j] = s * factor + offset;
      dest[2][i][j] = v * factor + offset;
      source_x += source_pixsize_x;
    }
    source_y += source_pixsize_y;
  }
}

template<class Type>
void Image<Type>::resizeToColourNames(Rectangle roi, double*** dest, int dest_width, int dest_height, float factor, float offset, double** colour_lut, int clt_nlines, int clt_shift)
{
  ASSERT(miChannels==3, "Image::resize() requires a 3-channel (colour) image.");
  roi.intersection(mBB);
  if (roi.area()==0)
    return;

  float source_pixsize_x = (float)roi.miWidth/dest_width;
  float source_pixsize_y = (float)roi.miHeight/dest_height;
  float source_x, source_y;
  int isource_x, isource_y;
  unsigned char r, g, b;
  int nbins = int(pow(2,clt_shift)+.5);

  // no interpolation !!!
  source_y=roi.miFirstLine;
  for(int i=0; i<dest_height; i++)
  {
    isource_y = int(source_y+.5);
    source_x=roi.miFirstColumn;
    for(int j=0; j<dest_width; j++)
    {
      isource_x = int(source_x+.5)*miChannels;
      r = *(mData+miWidthStep*isource_y + isource_x) / nbins;
      g = *(mData+miWidthStep*isource_y + isource_x+1) / nbins;
      b = *(mData+miWidthStep*isource_y + isource_x+2) / nbins;
      for(int cn=0; cn<11; cn++)
	dest[cn][i][j] = colour_lut[((r<<clt_shift)+g << clt_shift)+b][cn];
      source_x += source_pixsize_x;
    }
    source_y += source_pixsize_y;
  }
}



template<class Type>
void Image<Type>::rotateAndResizeToGrey(Rectangle roi, double** dest, int dest_width, int dest_height, float angle, float factor, float offset)
{
  ASSERT(miChannels==3, "Image::resize() requires a 3-channel (colour) image.");
  roi.intersection(mBB);
  if (roi.area()==0)
    return;

  float source_x, source_y, source_pixsize_x = (float)roi.miWidth/dest_width, source_pixsize_y = (float)roi.miHeight/dest_height, angle_rad = angle*M_PI/180.0;
  int isource_x, isource_y, source_cx = roi.centerX(), source_cy = roi.centerY();;
  unsigned char r, g, b;
  double ffactor = factor/3.0;

  // no interpolation !!!
  for(int i=0; i<dest_height; i++)
  {
    for(int j=0; j<dest_width; j++)
    {
      source_x = roi.miFirstColumn + j*source_pixsize_x - source_cx; // un-rotated centered position
      source_y = roi.miFirstLine + i*source_pixsize_y - source_cy;
      isource_x = int(source_x*cos(-angle_rad) - source_y*sin(-angle_rad)+.5) + source_cx; // rotated position
      isource_y = int(source_x*sin(-angle_rad) + source_y*cos(-angle_rad)+.5) + source_cy;
      r = *(mData+miWidthStep*isource_y + isource_x*miChannels);
      g = *(mData+miWidthStep*isource_y + isource_x*miChannels+1);
      b = *(mData+miWidthStep*isource_y + isource_x*miChannels+2);
      dest[i][j] = ffactor * (r+g+b) + offset;
    }
  }
}

template<class Type>
void Image<Type>::rotateAndResizeToHSV(Rectangle roi, double*** dest, int dest_width, int dest_height, float angle, float factor, float offset)
{
  ASSERT(miChannels==3, "Image::resize() requires a 3-channel (colour) image.");
  roi.intersection(mBB);
  if (roi.area()==0)
    return;

  float source_x, source_y, source_pixsize_x = (float)roi.miWidth/dest_width, source_pixsize_y = (float)roi.miHeight/dest_height, angle_rad = angle*M_PI/180.0;
  int isource_x, isource_y, source_cx = roi.centerX(), source_cy = roi.centerY();;
  unsigned char r, g, b, h, s, v;

  // no interpolation !!!
  for(int i=0; i<dest_height; i++)
  {
    for(int j=0; j<dest_width; j++)
    {
      source_x = roi.miFirstColumn + j*source_pixsize_x - source_cx; // un-rotated centered position
      source_y = roi.miFirstLine + i*source_pixsize_y - source_cy;
      isource_x = int(source_x*cos(-angle_rad) - source_y*sin(-angle_rad)+.5) + source_cx; // rotated position
      isource_y = int(source_x*sin(-angle_rad) + source_y*cos(-angle_rad)+.5) + source_cy;
      r = *(mData+miWidthStep*isource_y + isource_x*miChannels);
      g = *(mData+miWidthStep*isource_y + isource_x*miChannels+1);
      b = *(mData+miWidthStep*isource_y + isource_x*miChannels+2);
      RGBtoHSV(r, g, b, h, s, v);
      //RGBtoYCbCr(r, g, b, h, s, v);
      //RGBtoXYZ(r, g, b, h, s, v);
      dest[0][i][j] = h * factor + offset;
      dest[1][i][j] = s * factor + offset;
      dest[2][i][j] = v * factor + offset;
    }
  }
}

template<class Type>
void Image<Type>::rotateAndResizeToHSV(Rectangle roi, Image<Type>* dest, float angle)
{
  ASSERT(miChannels==3, "Image::resize() requires a 3-channel (colour) image.");
  roi.intersection(mBB);
  if (roi.area()==0)
    return;

  float source_x, source_y, source_pixsize_x = (float)roi.miWidth/dest->miWidth, source_pixsize_y = (float)roi.miHeight/dest->miHeight, angle_rad = angle*M_PI/180.0;
  int isource_x, isource_y, source_cx = roi.centerX(), source_cy = roi.centerY();;
  unsigned char r, g, b, h, s, v;

  // no interpolation !!!
  for(int i=0; i<dest->miHeight; i++)
  {
    for(int j=0; j<dest->miWidth; j++)
    {
      source_x = roi.miFirstColumn + j*source_pixsize_x - source_cx; // un-rotated centered position
      source_y = roi.miFirstLine + i*source_pixsize_y - source_cy;
      isource_x = int(source_x*cos(-angle_rad) - source_y*sin(-angle_rad)+.5) + source_cx; // rotated position
      isource_y = int(source_x*sin(-angle_rad) + source_y*cos(-angle_rad)+.5) + source_cy;
      r = *(mData+miWidthStep*isource_y + isource_x*miChannels);
      g = *(mData+miWidthStep*isource_y + isource_x*miChannels+1);
      b = *(mData+miWidthStep*isource_y + isource_x*miChannels+2);
      RGBtoHSV(r, g, b, h, s, v);
      //RGBtoYCbCr(r, g, b, h, s, v);
      //RGBtoXYZ(r, g, b, h, s, v);
      dest->setColourBGR(j, i, Colour(b, g, r));
    }
  }
}


template<class Type>
Image<Type>* Image<Type>::resizeBilinear(Rectangle srcroi, int dest_width, int dest_height)
{
  Image<Type>* dest = new Image<Type>(dest_width, dest_height, miChannels);

  srcroi.intersection(mBB);
  if (srcroi.area()==0)
    return dest;

  Type* psrc = data();
    Type* pdest = dest->data();
    Type a, b, c, d; 
    int x, y, index ;
    float x_ratio = ((float)(srcroi.miWidth-1))/dest_width ;
    float y_ratio = ((float)(srcroi.miHeight-1))/dest_height ;
    float x_diff, y_diff, blue, red, green ;
    int src_wstep=widthStep();
    int dst_wstep=dest->widthStep();
    int dst_xstep = dest->widthStep() - dest->width()*dest->nChannels();
    float xrj, yri;
    for (int i=0;i<dest_height;i++) {
      yri = y_ratio*i;
        for (int j=0;j<dest_width;j++) {
	  xrj = x_ratio*j;
            x = (int)(xrj);
            y = (int)(yri);
            x_diff = xrj - x ;
            y_diff = yri - y ;
            index = (y*src_wstep+x*miChannels); // TODO optimise

	    for(int ch=0; ch<dest->nChannels(); ch++)
	    {
	      a = psrc[index] ;
	      b = psrc[index+1*miChannels] ;
	      c = psrc[index+src_wstep] ;
	      d = psrc[index+src_wstep+1*miChannels] ;
	      *pdest++ = (a)*(1-x_diff)*(1-y_diff) + (b)*(x_diff)*(1-y_diff) + (c)*(y_diff)*(1-x_diff)   + (d)*(x_diff*y_diff);
	      index++;
	    }
        }
	pdest+=dst_xstep;
    }

  return dest;
}


template<class Type>
void Image<Type>::resizeBilinear(Rectangle srcroi, Image<Type>* dest)
{
  if (dest==this)
    return;

  if (miWidth==dest->width() && miHeight==dest->height())
  {
    copy(dest);
    return;
  }

  srcroi.intersection(mBB);
  if (srcroi.area()==0)
    return;

  Type* psrc = data();
  Rectangle destroi;
    Type a, b, c, d; 
    int x, y, index ;
    float x_ratio = ((float)(miWidth))/dest->width();
    float y_ratio = ((float)(miHeight))/dest->height();
    destroi = srcroi;
    destroi.scale(1.0/x_ratio, 1.0/y_ratio);
    float x_diff, y_diff, blue, red, green ;
    int src_wstep=widthStep();
    int dst_wstep=dest->widthStep();
    int dst_xstep = dest->widthStep() - destroi.miWidth*dest->nChannels();
    Type* pdest = dest->data() + destroi.miFirstLine*dest->widthStep() + destroi.miFirstColumn*dest->nChannels();;
    float xrj, yri;
    for (int i=0;i<destroi.miHeight;i++) {
      yri = y_ratio*(i+destroi.miFirstLine);
        for (int j=0;j<destroi.miWidth;j++) {
	  xrj = x_ratio*(j+destroi.miFirstColumn);
            x = (int)(xrj);
            y = (int)(yri);
            x_diff = xrj - x ;
            y_diff = yri - y ;
            index = (y*src_wstep+x*miChannels); // TODO optimise

	    for(int ch=0; ch<dest->nChannels(); ch++)
	    {
	      a = psrc[index] ;
	      b = psrc[index+1*miChannels] ;
	      c = psrc[index+src_wstep] ;
	      d = psrc[index+src_wstep+1*miChannels] ;
	      *pdest++ = (a)*(1-x_diff)*(1-y_diff) + (b)*(x_diff)*(1-y_diff) + (c)*(y_diff)*(1-x_diff)   + (d)*(x_diff*y_diff);
	      index++;
	    }
        }
	pdest+=dst_xstep;
    }
}



template<class Type>
Image<Type>* Image<Type>::flip()
{
  Image<Type>* res = new Image<Type>(miWidth, miHeight, miChannels);
  Type* ptr;
  Type* resptr = res->data();
  int xstep = miWidthStep - miWidth*miChannels;
  int resxstep = res->widthStep() - res->width()*res->nChannels();

  for(int i=1; i<=miHeight; i++)
  {
    ptr = mData+(i*miWidthStep) - xstep;
    for(int j=0; j<miWidth; j++)
    {
      *resptr++ = *ptr--;
    }
    resptr+= resxstep;
  }
  return res;
}


template<class Type>
Image<Type>* Image<Type>::equaliseHist()
{
  ASSERT(miChannels==1, "Image::equaliseHist() requires a single-channel image.");

  Image<Type>* res = new Image<Type>(miWidth, miHeight, miChannels);
  Type* ptr = mData;;
  Type* resptr = res->data();
  int xstep = miWidthStep - miWidth*miChannels;
  int resxstep = res->widthStep() - res->width()*res->nChannels();

  float orig_hist[256];
  float factor = 255.0/(miWidth*miHeight);
  float cum_hist[256];
  int i, j;

  for(i=1; i<256; i++)
    orig_hist[i] = 0;
  for(i=1; i<=miHeight; i++)
  {
    for(j=0; j<miWidth; j++)
      orig_hist[(int)*ptr++]+=factor;
    ptr += xstep;
  }
  cum_hist[0]=orig_hist[0];
  for(i=1; i<256; i++)
    cum_hist[i] = cum_hist[i-1] + orig_hist[i];

  ptr = mData;
  for(i=1; i<=miHeight; i++)
  {
    for(j=0; j<miWidth; j++)
      *resptr++ = (Type)cum_hist[*ptr++];
    ptr += xstep;
    resptr+= resxstep;
  }
  return res;
}


template<class Type>
float Image<Type>::uniformity(Rectangle roi)
{
  ASSERT(miChannels==1, "Image::uniformity() requires a single-channel image.");

  roi.intersection(mBB);
  if (roi.area()==0)
    return 1.0;

  float res=0;
  Type* ptr = mData + miWidthStep*roi.miFirstLine + roi.miFirstColumn;
  //int xstep = miWidthStep - miWidth*miChannels;
  Type prevval = *ptr;;
  int linestep = miWidthStep-roi.miWidth;
  float changes=0;
  int nonzeros=0;
  int i,j;

  for(i=1; i<=roi.miHeight; i++)
  {
    for(j=0; j<roi.miWidth; j++)
    {
      if (*ptr!=prevval)
        changes++;
      if (*ptr>0.01)
	nonzeros++;
      prevval = *ptr++;
    }
    ptr += linestep;
  }
  res = 1.0-changes/roi.area();
  //res = max(0.0, 1.0-changes/nonzeros);
  return res;
}

template<class Type>
int Image<Type>::sumGreaterThanThreshold(Rectangle roi, float thresh)
{
  ASSERT(miChannels==1, "Image::maxLoc() requires a single-channel image.");

  roi.intersection(mBB);

  Type* ptr = mData + miWidthStep*roi.miFirstLine + roi.miFirstColumn;
  int linestep = miWidthStep-roi.miWidth;
  Type val;
  int fl, fc, ll, lc;
  fl = roi.miFirstLine;
  fc = roi.miFirstColumn;
  ll = roi.lastLine();
  lc = roi.lastColumn();
  int result=0;

  for(int i=fl; i<=ll; i++)
  {
    for(int j=fc; j<=lc; j++)
    {
      if (*ptr++>thresh)
	result++;
    }
    ptr+=linestep;
  }
  return result;
}


template<class Type>
float Image<Type>::percentageChanged(Rectangle roi, int offx, int offy, Image<Type>* img)
{
  ASSERT(miChannels==1, "Image::percentageChanged() requires a single-channel image.");
  roi.intersection(mBB);
  roi.translate(offx, offy);
  roi.intersection(mBB);
  roi.translate(-offx, -offy);

  float res=0;
  Type* ptr = mData + miWidthStep*(roi.miFirstLine+offy) + roi.miFirstColumn+offx;
  Type* imgptr = img->mData + img->miWidthStep*roi.miFirstLine + roi.miFirstColumn;
  int linestep = miWidthStep-roi.miWidth;
  int imglinestep = img->miWidthStep-roi.miWidth;
  float changes=0;
  int i,j;

  for(i=1; i<=roi.miHeight; i++)
  {
    for(j=0; j<roi.miWidth; j++)
    {
      if (fabs(*ptr-*imgptr)>0.5)
	changes++;
      ptr++;
      imgptr++;
    }
    ptr += linestep;
    imgptr += imglinestep;
  }
  res = changes/roi.area();
  return res;
}


template<class Type>
void Image<Type>::integralImage(Image<double>* result)
{
  double* out = result->data();  // pointer to current pixel
  double *out_1;    // pointer to previous pixel
  double *out_firstcol; // pointer to first col of current line
  double *out_firstcol_1; // pointer to first col of previous line
  double *out_line_1;  // pointer to element one line above the current element
  double line_sum;  // cumulating the values of one line
  Type* in = mData;
  Type* in_line;
  
  // first line;
  *out = *in;
  in++;
  
  out_1 = result->data();
  out = out_1+1;
  for(int ij=1; ij<miWidth; ij++)
  {
    *out = *out_1 + *in;
    out++;
    out_1++;
    in++;
  }
  // end first line   (out points to first element of second line)
    
  out_firstcol_1=result->data();;
  out_firstcol = out_firstcol_1 + result->widthStep();
  out_line_1=out_firstcol_1;
  in_line = mData + miWidthStep;
  for(int ii=1; ii<miHeight; ii++)
  {
    // compute first element of line
    in = in_line;
    line_sum = *in;  // initialise cumulative sum over line
    *out_firstcol = *out_firstcol_1 + line_sum;

    out_1 =out_firstcol;
    out = out_1+1;
    out_line_1 = out_firstcol_1+1;

    for(int ij=1; ij<miWidth; ij++)
    {
      line_sum += *in;
      *out = *out_line_1 + line_sum;

      out++; // incrementing pointers 
      out_1++;
      out_line_1++;
      in++;
    }
    out_firstcol_1 = out_firstcol;
    out_firstcol += result->widthStep();
    in_line += miWidthStep;
  }
}

template<class Type>
Rectangle Image<Type>::sampleRegion(int width, int height)
{
  if (width>miWidth)
    width=miWidth;
  if (height>miHeight)
    height=miHeight;

  Rectangle cur_rect(miWidth-width, miHeight-height, miWidth-1, miHeight-1);
  double max_prob = get(cur_rect.centerX(), cur_rect.centerY())
    //+ get(cur_rect.lastColumn(), cur_rect.lastLine())
    //- get(cur_rect.miFirstColumn, cur_rect.lastLine())
    //- get(cur_rect.lastColumn(), cur_rect.miFirstLine);
    ;
  double cur_prob;
  double sampled_prob;

  if (max_prob<=0)
  {
    // sample uniformly from whole image
    int x = int(((float)rand())/RAND_MAX*(miWidth-width)); 
    int y = int(((float)rand())/RAND_MAX*(miHeight-height)); 
    cur_rect.miFirstColumn=x;
    cur_rect.miFirstLine=y;
  }
  else
  {
    sampled_prob = ((float)rand())/RAND_MAX*max_prob;
    // traverse image until the sampled probability (TODO binary search)
    cur_rect.miFirstColumn=0;
    cur_rect.miFirstLine=0;
    do
    {
      cur_prob = get(cur_rect.centerX(), cur_rect.centerY()) 
      //+ get(cur_rect.lastColumn(), cur_rect.lastLine())
      //- get(cur_rect.miFirstColumn, cur_rect.lastLine())
      //- get(cur_rect.lastColumn(), cur_rect.miFirstLine);
      ;
      cur_rect.miFirstColumn++;
      if (cur_rect.miFirstColumn+cur_rect.miWidth>miWidth)
      {
	cur_rect.miFirstColumn=0;
	cur_rect.miFirstLine++;
	if (cur_rect.miFirstLine+cur_rect.miHeight>miHeight)
	{
	  cur_rect.miFirstColumn=0;
	  cur_rect.miFirstLine=0;
	  break;
	}
      }
    } while (cur_prob < sampled_prob);
  }

  return cur_rect;
}

template<class Type>
void Image<Type>::sample(int& x, int& y)
{
  int curx=0, cury=0;
  double max_prob = get(miWidth-1, miHeight-1);
    //+ get(cur_rect.lastColumn(), cur_rect.lastLine())
    //- get(cur_rect.miFirstColumn, cur_rect.lastLine())
    //- get(cur_rect.lastColumn(), cur_rect.miFirstLine);
    ;
  double cur_prob;
  double sampled_prob;

  if (max_prob<=0)
  {
    // sample uniformly from whole image
    curx = std::min(miWidth-1, int(((float)rand())/RAND_MAX*miWidth)); 
    cury = std::min(miHeight-1,int(((float)rand())/RAND_MAX*miHeight)); 
  }
  else
  {
    sampled_prob = ((float)rand())/RAND_MAX*max_prob;
    // traverse image until the sampled probability (TODO binary search)
    curx=cury=0;
    do
    {
      cur_prob = get(curx, cury);
      //+ get(cur_rect.lastColumn(), cur_rect.lastLine())
      //- get(cur_rect.miFirstColumn, cur_rect.lastLine())
      //- get(cur_rect.lastColumn(), cur_rect.miFirstLine);
      ;
      curx++;
      if (curx>=miWidth)
      {
	curx=0;
	cury++;
	if (cury>=miHeight)
	{
	  curx=cury=0;
	  break;
	}
      }
    } while (cur_prob < sampled_prob);
  }
  x=curx;
  y=cury;
}

template<class Type>
void Image<Type>::cumulativeSum(Image<double>* result)
{
  ASSERT(miChannels==1, "Image::cumulativeSum() requires a single-channel image.");

  Type* ptr = mData;
  double* resptr = result->data();
  double zero=0;
  double* prevresptr = &zero;
  int linestep = miWidthStep-miWidth;
  int reslinestep = result->widthStep()-result->width();
  Type val;
  Type sum=0;

  for(int i=0; i<miHeight; i++)
  {
    for(int j=0; j<miWidth; j++)
    {
      *resptr = *prevresptr + *ptr;
      prevresptr = resptr;
      resptr++;
      ptr++;
    }
    ptr+=linestep;
    resptr+=reslinestep;
  }
}


template<class Type>
void Image<Type>::cumulativeSum(Image<double>* int_img, Rectangle scan_win, Image<double>* result)
{
  ASSERT(miChannels==1, "Image::cumulativeSum() requires a single-channel image.");

  //Type* ptr = mData;
  double* resptr;
  double zero=0;
  double* prevresptr = &zero;
  /*
  int linestep = miWidthStep-miWidth+2*w2;
  int reslinestep = result->widthStep()-result->width()+;
  */
  double val;
  Type sum=0;
  int w2=scan_win.miWidth/2, h2=scan_win.miHeight/2;

  result->setZero();
  for(int i=h2; i<miHeight-h2; i++)
  {
    resptr = result->data() + result->widthStep()*i + w2;
    for(int j=w2; j<miWidth-w2; j++)
    {
      val = int_img->get(j-w2, i-h2)+int_img->get(j+w2, i+h2)-int_img->get(j-w2, i+h2)-int_img->get(j+w2, i-h2);
      *resptr = *prevresptr + val;
      prevresptr = resptr;
      resptr++;
      //ptr++;
    }
    /*
    ptr+=linestep;
    resptr+=reslinestep;
    */
  }
}

template<class Type>
void Image<Type>::sumSlidingWindow(Image<double>* int_img, Rectangle scan_win, Image<double>* result)
{
  ASSERT(miChannels==1, "Image::sumSlidingWindow() requires a single-channel image.");

  //Type* ptr = mData;
  double* resptr;
  double zero=0;
  //double* prevresptr = &zero;
  /*
  int linestep = miWidthStep-miWidth+2*w2;
  int reslinestep = result->widthStep()-result->width()+;
  */
  double val;
  Type sum=0;
  int w2=scan_win.miWidth/2, h2=scan_win.miHeight/2;

  result->setZero();
  for(int i=h2; i<miHeight-h2; i++)
  {
    resptr = result->data() + result->widthStep()*i + w2;
    for(int j=w2; j<miWidth-w2; j++)
    {
      val = int_img->get(j-w2, i-h2)+int_img->get(j+w2, i+h2)-int_img->get(j-w2, i+h2)-int_img->get(j+w2, i-h2);
      *resptr = val;
      //prevresptr = resptr;
      resptr++;
      //ptr++;
    }
    /*
    ptr+=linestep;
    resptr+=reslinestep;
    */
  }
}


template<class Type>
void Image<Type>::normaliseByMax()
{
  int maxx, maxy;
  Type maxval = maxLoc(mBB, maxx, maxy);
  multiply(1.0/maxval);
}


template<class Type>
float Image<Type>::crossCorrelation(Image<float>* pattern, Rectangle roi, float scale_pattern_x, float scale_pattern_y, Rectangle* pattern_roi, int& maxx, int& maxy)
{
  ASSERT(miChannels==pattern->nChannels(), "Image::crossCorrelation(): Image types not compatible.");

  // The correlation is performed in the image coordinate system.
  // So the image coordinates need to be scaled (multiplied by scale_to_pattern_x/y) to access pattern pixels.
  
  int roiw = roi.miWidth;
  int roih = roi.miHeight;
  // pattern_roi, pw, ph, pattern_offset_x, pattern_offset_y are in the image (not pattern) coordinate system 
  int pw, ph;  // pattern width/height in the image coordinate system
  int pattern_offset_x, pattern_offset_y;

  if (pattern_roi)
  {
    pw = pattern_roi->miWidth;
    ph = pattern_roi->miHeight;
    pattern_offset_x = pattern_roi->miFirstColumn;
    pattern_offset_y = pattern_roi->miFirstLine;
  }
  else
  {
    pw = pattern->width()*scale_pattern_x; 
    ph = pattern->height()*scale_pattern_y;
    pattern_offset_x = pattern_offset_y = 0;
  }
  float scale_to_pattern_x = 1.0/scale_pattern_x;
  float scale_to_pattern_y = 1.0/scale_pattern_y;

  /*
  if (roiw<pw*scale_pattern_x || roih<ph*scale_pattern_y)
  {
    maxx=maxy=0;
    return;
  }
  */

  int sh, sw, i, j;
  float ccval, maxccval=-FLT_MAX;
  int stepx = miWidthStep - pw*miChannels;
  //int pstepx = pattern->widthStep() - pw*pattern->nChannels();
  Type* ptr;

  for(sh=0; sh<roih-ph+1; sh++)
    for(sw=0; sw<roiw-pw+1; sw++)
    {
      ccval = 0;

      ptr = mData + miWidthStep*(roi.miFirstLine+sh) + roi.miFirstColumn+sw;
      for(i=pattern_offset_y; i<pattern_offset_y+ph; i++)
      {
	for(j=pattern_offset_x; j<pattern_offset_x+pw; j++)
	{
	  ccval += *ptr * pattern->get(j*scale_to_pattern_x, i*scale_to_pattern_y);
	  ptr++;
	}
	ptr+=stepx;
      }
      if (ccval>maxccval)
      {
	maxccval = ccval;
	maxx = sw;
	maxy = sh;
      }
    }
  maxx += roi.miFirstColumn + pw/2;
  maxy += roi.miFirstLine + ph/2;
  return maxccval;
}


template<class Type>
float Image<Type>::fastCrossCorrelation(Image<float>* pattern, Rectangle roi, float scale_pattern_x, float scale_pattern_y, Rectangle* pattern_roi, int& maxx, int& maxy)
{
  ASSERT(miChannels==pattern->nChannels(), "Image::crossCorrelation(): Image types not compatible.");

  int roiw = roi.miWidth;
  int roih = roi.miHeight;
  // pattern_roi, pw, ph, pattern_offset_x, pattern_offset_y are in the image (not pattern) coordinate system 
  int pw, ph;
  int pattern_offset_x, pattern_offset_y;
  Rectangle proi;

  if (pattern_roi)
  {
    pw = pattern_roi->miWidth;   
    ph = pattern_roi->miHeight;
    pattern_offset_x = pattern_roi->miFirstColumn;
    pattern_offset_y = pattern_roi->miFirstLine;
    proi = *pattern_roi;
  }
  else
  {
    pw = pattern->width()*scale_pattern_x;
    ph = pattern->height()*scale_pattern_y;
    pattern_offset_x = pattern_offset_y = 0;
    proi.initPosAndSize(0, 0, pw, ph);
  }
  float scale_to_pattern_x = 1.0/scale_pattern_x;
  float scale_to_pattern_y = 1.0/scale_pattern_y;
  //int roistep=1; //int(std::max(1.0f, (float)roiw/pw));
  //MESSAGE(2, "roiw: " << roiw << "  pw: " << pw << "  roistep: " << roistep);
  Image<float>* pat = new Image<float>(pw, ph, 1);
  pattern->resize(proi, pat);

  int sh, sw, i, j;
  float ccval, maxccval=-FLT_MAX;
  int stepx = miWidthStep - pw*miChannels;
  //int pstepx = pattern->widthStep() - pw*pattern->nChannels();
  Type *ptr, *patptr;
  Type *roi_baseptr=mData + miWidthStep*(roi.miFirstLine+pattern_offset_y) + (roi.miFirstColumn+pattern_offset_x)*miChannels;
  int roi_baseptr_stepx = miWidthStep - (roiw-pw+1-pattern_offset_x)*miChannels;
  int patstepx = pat->miWidthStep - pw*miChannels;

  for(sh=pattern_offset_y; sh<roih-ph+1; sh++)
  {
    for(sw=pattern_offset_x; sw<roiw-pw+1; sw++)
    {
      ccval = 0;
      ptr = roi_baseptr;
      patptr = pat->data();
      for(i=0; i<ph; i++)
      {
	for(j=0; j<pw; j++)
	{
	  ccval += *ptr * *patptr;
	  ptr++;
	  patptr++;
	}
	ptr+=stepx;
	patptr+=patstepx;
      }
      if (ccval>maxccval)
      {
	maxccval = ccval;
	maxx = sw;
	maxy = sh;
      }
      roi_baseptr++;
    }
    roi_baseptr+=roi_baseptr_stepx;
  }
  maxx += roi.miFirstColumn + pw/2; // + pattern_offset_x*scale_pattern_x;
  maxy += roi.miFirstLine + ph/2; // + pattern_offset_y*scale_pattern_y;

  delete pat;
  return maxccval;
}


template<class Type>
void Image<Type>::translate(int dx, int dy)
{
  copy(this, dx, dy);
  /*  // slow and only works for single-channel images
  int startx, starty, endx, endy, stepx, stepy;
  if (dx==0 && dy==0)
    return;

  if (dx<0)
  {
    startx=0;
    stepx=1;
    endx=miWidth-1;
  }
  else
  {
    startx=miWidth-1;
    stepx=-1;
    endx=0;
  }
  if (dy<0)
  {
    starty=0;
    stepy=1;
    endy=miWidth-1;
  }
  else
  {
    starty=miHeight-1;
    stepy=-1;
    endy=0;
  }
  for(int i=starty; i!=endy; i+=stepy)
    for(int j=startx; j!=endx; j+=stepx)
    {
      if (j-dx>0 && j-dx<miWidth && i-dy>0 && i-dy<miHeight)
	set(j, i, get(j-dx, i-dy));
      else
	set(j, i, 0);
    }
    */
}





/*
// generic case
template<class Type>
void Image<Type>::writeToFile(const std::string& filename)
{
	std::cout << "Image<Type>::writeToFile: not implemented for this type" << std::endl;
}

// specialization for unsigned char
template<>
inline void Image<unsigned char>::writeToFile(const std::string& filename)
{
	cv::Mat m( this->height(), this->width(), this->nChannels()==3?CV_8UC3:CV_8UC1, this->data(), this->widthStep());
	cv::imwrite( filename, m);
}

// specialization for float
template<>
inline void Image<float>::writeToFile(const std::string& filename)
{
	cv::Mat m( this->height(), this->width(), CV_32F, this->data(), this->widthStep()*sizeof(float));

	// use xml or yml extension only
	cv::FileStorage file( filename, cv::FileStorage::WRITE);
	file << "image";
	file << m;
}
*/




} // namespace


#endif
