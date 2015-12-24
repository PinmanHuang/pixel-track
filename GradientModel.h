#ifndef GRADIENTMODEL_H
#define GRADIENTMODEL_H

#include <list>
#include "Image.h"
#include "Rectangle.h"
#include "GradDispLUT.h"
#include "HSVPixelModel.h"

//#define MAXVOTES 1000

using namespace TLImageProc;
using namespace std;

class GradientModel
{
  public:
    GradientModel(int nb_obins, int nb_mbins, float mag_thresh);
    ~GradientModel();

    void reset();
    void learn(Image<short>* xgradimg, Image<short>* ygradimg, Rectangle bb, Image<float>* segmentation);
    void vote(Image<short>* xgradimg, Image<short>* ygradimg, Rectangle bb, Image<float>* voting_map);
    void backproject(Image<short>* xgradimg, Image<short>* ygradimg, Rectangle search_window, Image<float>* bpimg, int maxlocx, int maxlocy);

  private:
    GradDispLUT* m_LUT;
    list<displacement_t>** disp;
    int o_bins;
    int m_bins;
    int totalbins;
    float magnitude_threshold;
};

#endif
