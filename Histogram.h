#ifndef TL_HISTOGRAM_H
#define TL_HISTOGRAM_H

#include <valarray>
#include "Image.h"
#include "Rectangle.h"
#include "BGR2HSVhistLUT.h"

using namespace std;
using namespace TLImageProc;

namespace TLImageProc
{

class Histogram {
  private:
    valarray<float>* mTmpres;
    int* miNHSBins;
    int* miNVBins;
    int* miNTotalBins;
    Rectangle mImageBB;

  public:
    int niScales;
    valarray<float>* hsv_count;
 
    Histogram(int h_bins, int s_bins, int v_bins, int _niScales, Rectangle imgBB, bool v_bin_separate=true);

    ~Histogram();

    int compute(Image<unsigned char>* image, ColourHistLUT** luts,
                Rectangle* roi, 
                Image<float>* mask=NULL,
                bool setZero=true,
                bool normalise_hist=true,
		bool grid=true);
    int compute(Image<unsigned char>* image, ColourHistLUT** luts,
                Rectangle* roi, 
		float kernel_sigma_x, float kernel_sigma_y,
                bool setZero=true,
                bool normalise_hist=true,
		bool grid=true);
    void update(Histogram* h, float factor, bool norm=true);

    float distanceUnnormalised(Histogram* h);
    float distance(Histogram* h);
    void setZero();
    void normalise();
};

}

#endif
