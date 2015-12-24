#ifndef TL_GRADDISP_HISTOGRAM 
#define TL_GRADDISP_HISTOGRAM

class GradDispLUT {
  //private: 
  public:
    // LUT
    unsigned int* grad_to_disp;

  public:
    int o_bins;
    int m_bins;
    
    GradDispLUT(int o_bins, int m_bins, float m_threshold=50);

    ~GradDispLUT();

    void compute_luts(float m_threshold);
    inline unsigned int get_bin(short gx, short gy){
	  return grad_to_disp[((gx+1025)<<12) + gy+1025];
    }
};


#endif
