#ifndef HSVPIXELGRADIENTMODEL_H
#define HSVPIXELGRADIENTMODEL_H

#include <list>
#include "Image.h"
#include "Rectangle.h"
#include "GradDispLUT.h"
#include "HSVPixelModel.h"

#define CLUSTER_SIZE 3

using namespace TLImageProc;
using namespace std;

class HSVPixelGradientModel
{
  public:
    HSVPixelGradientModel(int nb_hsbins, int nb_vbins, int nb_obins, int nb_mbins, float mag_thresh);
    ~HSVPixelGradientModel();

    void reset();
    void learn(Image8U* img, Image<short>* xgradimg, Image<short>* ygradimg, Rectangle bb, Image<float>* segmentation);
    void vote(Image8U* img, Image<short>* xgradimg, Image<short>* ygradimg, Rectangle bb, Image<float>* voting_map);
    void vote(Image8U* img, Image<short>* xgradimg, Image<short>* ygradimg, Rectangle bb, Image<float>* voting_map, float scale, float angle);
    void backproject(Image8U* img, Image<short>* xgradimg, Image<short>* ygradimg, Rectangle bb, Image<float>* bpimg, int maxlocx, int maxlocy);
    void backproject(Image8U* img, Image<short>* xgradimg, Image<short>* ygradimg, Rectangle bb, Image<float>* bpimg, int maxlocx, int maxlocy, Image<float>* segmentation, float& mean_pos, float& variance_pos, float& mean_neg, float& variance_neg);
    void update(Image8U* img, Image<short>* xgradimg, Image<short>* ygradimg, Rectangle bb, Image<float>* segmentation, float update_factor);

  private:
    BGR2HSVdistLUT* m_LUTColour;
    GradDispLUT* m_LUTGradient;
    list<displacement_t>** disp;
    int h_bins;        
    int s_bins;  
    int v_bins;
    int o_bins;
    int m_bins;
    int maxcolourbin;
    int totalbins;
    float magnitude_threshold;
};

#endif
