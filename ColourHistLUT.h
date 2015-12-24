#ifndef COLOUR_HIST_LUT
#define COLOUR_HIST_LUT

#include <string>

namespace TLImageProc
{

class ColourHistLUT
{
  public:
    ColourHistLUT(int nbbins);
    virtual ~ColourHistLUT();

    int miNBins;
    unsigned short int* bgr_to_colour_bin; // LUT
    std::string cache_filename; 

    virtual void computeLUT(float thresh1, float thresh2)=0;
    virtual unsigned short getBin(int r, int g, int b)=0;

    void save(std::string filename);
    bool read(std::string filename);

    bool hasSeparate3rdChannel() { return mbSeparate3rdChannel; };
  protected:
    bool mbSeparate3rdChannel;
};


}


#endif
