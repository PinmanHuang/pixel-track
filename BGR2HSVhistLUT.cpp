#include <math.h>
#include <float.h>
//#include <fstream>
#include <sstream>
#include "Error.h"
#include "util.h"
#include "BGR2HSVhistLUT.h"
#include "Convert.h"

using namespace std;

namespace TLImageProc
{

BGR2HSVhistLUT::BGR2HSVhistLUT(int _h_bins,
			       int _s_bins,
			       int _v_bins,
			       float s_threshold,
			       float v_threshold)
	: ColourHistLUT(256*256*256)
{
  cache_filename = "cache_BGR2HSVhistLUT";
  stringstream s(cache_filename);
  s.seekp(0, ios::end);

  h_bins = _h_bins;
  s_bins = _s_bins;
  v_bins = _v_bins;

  s << "_" << h_bins << "_" << s_bins << "_" << v_bins << "_" << s_threshold << "_" << v_threshold;
  string cf = TLUtil::getExePath() + "/" + s.str();

  if (!read(cf))
  {
    MESSAGE(1, "Re-computing " << s.str());
    computeLUT(s_threshold, v_threshold);
    save(cf);
  }
}

BGR2HSVhistLUT::~BGR2HSVhistLUT(){
}


// Compute LUTs for histogram computation
// directly from the RGB pixel values
void BGR2HSVhistLUT::computeLUT(float s_threshold, float v_threshold)
{
  double h, s, v;

  double h_step = 360./h_bins;
  double s_step = 1./s_bins;
  double v_step = 1./v_bins;
  double norm = 1.0/255;
  int indtmp1, indtmp2, ind;

  // changed to order BGR 
  for(int b=0;b<256;b++){
    indtmp1 = b*256;
    for(int g=0;g<256;g++){
      indtmp2 = (indtmp1 + g)*256;
      for(int r=0;r<256;r++){
	RGBtoHSV(b*norm, g*norm, r*norm, h, s, v);

	ind = indtmp2 + r;

	int h_index = floor(h/h_step);
	if(h_index == h_bins) h_index = h_bins-1;
	int s_index = floor(s/s_step);
	if(s_index == s_bins) s_index = s_bins-1;
	//bgr_to_hs_bin[ind] = h_index*s_bins + s_index;
	int v_index = floor(v/v_step);
	if(v_index == v_bins) v_index = v_bins-1;
	//bgr_to_v_bin[ind]  = v_index;

	if (s<s_threshold || v<v_threshold)
	  bgr_to_colour_bin[ind] = h_bins*s_bins + v_index;
	else
	  bgr_to_colour_bin[ind] = h_index*s_bins + s_index;

      }
    }
  }
}


}

