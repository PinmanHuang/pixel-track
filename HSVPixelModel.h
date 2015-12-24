#ifndef HSVPIXELMODEL_H
#define HSVPIXELMODEL_H

#include <list>
#include "Image.h"
#include "Rectangle.h"
#include "BGR2HSVdistLUT.h"

#define MAXVOTES 20 // 1000

using namespace TLImageProc;
using namespace std;

typedef struct displacement_t
{
  public:
    displacement_t() {x=0; y=0; };
    displacement_t(int x_, int y_) {x=x_; y=y_; };
    bool operator==(const displacement_t& d1) { return (x==d1.x && y==d1.y); };
    int x;
    int y;
    float count;
} displacement_t;

bool disp_less(displacement_t lhs, displacement_t rhs);
bool disp_less_count(displacement_t lhs, displacement_t rhs);



class HSVPixelModel
{
  public:
    HSVPixelModel(int nb_hsbins, int nb_vbins);
    ~HSVPixelModel();

    void reset();
    void learn(Image8U* img, Rectangle bb, Image<float>* segmentation);
    void vote(Image8U* img, Rectangle bb, Image<float>* voting_map);
    void backproject(Image8U* img, Rectangle search_window, Image<float>* bpimg, int maxlocx, int maxlocy);
    void backproject(Image8U* img, Rectangle search_window, Image<float>* bpimg, int maxlocx, int maxlocy, Image<float>* segmentation, float& mean_pos, float& variance_pos, float& mean_neg, float& variance_neg);

  private:
    BGR2HSVdistLUT* m_LUT;
    list<displacement_t>** hsv_disp;
    int h_bins;        
    int s_bins;  
    int v_bins;
    int totalbins;
};

#endif
