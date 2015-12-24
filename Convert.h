#ifndef TL_CONVERT_INC
#define TL_CONVERT_INC


namespace TLImageProc
{

void swap_(unsigned char *a, unsigned char *b);

/// Pixmap convertion from RGB24 to BGR24
void rgb24_to_bgr24(int width, int height, unsigned char *data_in, int in_width, unsigned char *data_out, int out_width);

void rgb24_to_rgb24(int width, int height, unsigned char *data_in, int in_width, unsigned char *data_out, int out_width);

void rgb24_to_rgb24_max(int width, int height, unsigned char *data_in, int in_widthstep, unsigned char *data_out, int out_widthstep);
void rgb24_to_bgr24_max(int width, int height, unsigned char *data_in, int in_widthstep, unsigned char *data_out, int out_widthstep);
void rgb32_to_bgr24_max(int width, int height, unsigned char *data_in, int in_widthstep, unsigned char *data_out, int out_widthstep);
void rgb32_to_bgr24_3channelmax(int width, int height, unsigned char *data_in, int in_widthstep, unsigned char *data_out, int out_widthstep);

void brg24_to_bgr24(int width, int height, unsigned char *data_in, int in_width, unsigned char *data_out, int out_width);

void brg24_to_rgb24(int width, int height, unsigned char *data_in, int in_width, unsigned char *data_out, int out_width);

static inline void rgb24_to_bgr24_inline(int width, int height, unsigned char *data_in, int in_widthstep, unsigned char *data_out, int out_widthstep)
{
  /*
  int h, w, base_in, base_out;
  int h_iws, h_ows;
  */
  int h, w, w3;
  int wh;

  unsigned char *din, *dout;
  int padin, padout;

  din = data_in;
  dout = data_out;
  w3 = width*3;

  if (in_widthstep==w3 && out_widthstep==w3)
  {
    wh=width*height;
    din+=2;
    //for(h = 0; h < height ; h++)
    for(int i=0; i<wh; i++)
    {
      /*
      h_iws = h * in_widthstep;
      h_ows = h * out_widthstep;
      */

      //for(w = 0; w < width ; w++)
      {
	/*
	base_in = w*3 + h_iws;
	base_out = w*3 + h_ows;
	*/

	//din+=2;
	*dout = *din;
	dout++;
	din--;
	*dout = *din;
	dout++;
	din--;
	*dout = *din;
	dout++;
        //din+=3;
        din+=5;
      }
    }
  }
  else
  {
    padin = in_widthstep-w3;
    padout = out_widthstep-w3;

    din+=2;
    for(h = 0; h < height ; h++)
    {
      for(w = 0; w < width ; w++)
      {
	//din+=2;
	*dout = *din;
	dout++;
	din--;
	*dout = *din;
	dout++;
	din--;
	*dout = *din;
	dout++;
        //din+=3;
        din+=5;
      }
      din+=padin;
      dout+=padout;
    }
  }
}

//void BGR_to_normalizedRGB(IplImage* src, IplImage* dst);

void gray_to_rgb24(int width, int height, unsigned char *data_in, int in_width, unsigned char *data_out, int out_width);

void yuv_2_rgb(int y, int u, int v, unsigned char* r, unsigned char* g, unsigned char* b);
void yuv_to_rgb24(int width, int height, unsigned char *data_in, int in_widthstep, unsigned char *data_out, int out_widthstep);
void yuv_to_bgr24(int width, int height, unsigned char *data_in, int in_widthstep, unsigned char *data_out, int out_widthstep);

void RGBtoHSV( const double r, const double g, const double b, double& h, double& s, double& v);
void RGBtoHSV( const unsigned char r, const unsigned char g, const unsigned char b, unsigned char& h, unsigned char& s, unsigned char& v);
void RGBtoXYZ( const unsigned char r, const unsigned char g, const unsigned char b, unsigned char& x, unsigned char& y, unsigned char& z);
void RGBtoYCbCr( const unsigned char r, const unsigned char g, const unsigned char b, unsigned char& y, unsigned char& cb, unsigned char& cr);

}


#endif

