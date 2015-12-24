#ifndef TL_BGR2HSV_DIST_HISTOGRAM 
#define TL_BGR2HSV_DIST_HISTOGRAM

void RGBtoHSV( const double r, const double g, const double b, double& h, double& s, double& v); 

class BGR2HSVdistLUT {
  //private: 
  public:
    // LUT
    unsigned short int* bgr_to_dist;

  public:
    int h_bins;
    int s_bins;
    int v_bins;
    
    BGR2HSVdistLUT(int h_bins, 
                   int s_bins, 
                   int v_bins,
                   float s_threshold=0.1,
                   float v_threshold=0.2);

    ~BGR2HSVdistLUT();

    void compute_luts(float h_threshold,
                      float v_threshold);
    inline int hsv_bin(int r, int g, int b){
	  return bgr_to_dist[(((b<<8) + g)<<8) + r];
    }
};


#endif
