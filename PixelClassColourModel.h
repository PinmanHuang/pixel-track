#ifndef PIXELCLASSCOLOURMODEL_H
#define PIXELCLASSCOLOURMODEL_H

#include "Histogram.h"
#include "Image.h"
#include "BGR2HSVhistLUT.h"

//#define SPATIAL_PRIOR
#define SPATIAL_PRIOR_CREATE

#define FG_PRIOR_PROBABILITY 0.3

using namespace TLImageProc;


class PixelClassColourModel
{
  private:
    Histogram* mHist[3]; 
    Histogram* mUpdateHist[3]; 
    ColourHistLUT** mLUT;
    float mfMeanFGVoteErr;
    float mfVarFGVoteErr;
    float mfMeanBGVoteErr;
    float mfVarBGVoteErr;
    Rectangle mImageBB;

  public:
    PixelClassColourModel(ColourHistLUT** lut, int h_bins, int s_bins, int v_bins, int _niScales, int imgw, int imgh);
    ~PixelClassColourModel();

    void setVoteErrParameters(float mean_pos, float var_pos, float mean_neg, float var_neg);
    void create(Image8U* img, Rectangle* outer_bb, Rectangle* object_region, float enlarge_factor_for_bg=2.0);
    void update(Image8U* img, Rectangle* outer_bb, Image<float>* segmentation, Image<float>* bp_img, float update_factor, float enlarge_factor_for_bg=2.0);
    void evaluateColour(Image8U* img, Rectangle* roi, bool use_spatial_prior, Image<float>* result); 
    void evaluateColourWithPrior(Image8U* img, Rectangle* roi, bool use_spatial_prior, Image<float>* prior, Image<float>* result);
    void evaluate(Image8U* img, Rectangle* roi, Image<float>* bp_img, Image<float>* result); 
};


#endif
