#ifndef TL_BGR2HSV_HISTOGRAM 
#define TL_BGR2HSV_HISTOGRAM

#include <string>
#include "ColourHistLUT.h"

namespace TLImageProc
{

class BGR2HSVhistLUT : public ColourHistLUT 
{
  public:
    int h_bins;
    int s_bins;
    int v_bins;
    
    BGR2HSVhistLUT(int h_bins, int s_bins, int v_bins, float s_threshold=0.1, float v_threshold=0.2);

    ~BGR2HSVhistLUT();

    void computeLUT(float h_threshold, float v_threshold);
    inline unsigned short getBin(int r, int g, int b){ return bgr_to_colour_bin[(((b<<8) + g)<<8) + r];
    }
};

}

#endif
