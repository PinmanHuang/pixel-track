#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <algorithm>

#include "Convert.h"

namespace TLImageProc
{

void swap_(unsigned char *a, unsigned char *b)
{
  unsigned char tmp;

  tmp = *a;
  *a = *b;
  *b = tmp;
}


void rgb24_to_bgr24(int width, int height, unsigned char *data_in, int in_widthstep, unsigned char *data_out, int out_widthstep)
{
  int h, w, base_in, base_out;
  int h_iws, h_ows;
  unsigned char *d_out, *d_in;
  int w3;

  w3 = width*3;

  if(data_out == NULL)
  {
    // convert and swap in the same pixmap

    for(h = 0; h < height/2 ; h++)
    {
      h_iws = h*in_widthstep;
      h_ows = h*out_widthstep;

      for(w = 0; w < width ; w++)
      {
	base_in = w*3 + h_iws;
	base_out = w*3 + h_ows;

	swap_(&data_in[base_out], &data_in[base_in+2]);
	swap_(&data_in[base_out+1], &data_in[base_in+1]);
	swap_(&data_in[base_out+2], &data_in[base_in]);
      }
    }
  }
  else
  {
    for(h = 0; h < height ; h++)
    {
      h_iws = h * in_widthstep;
      h_ows = h * out_widthstep;

      for(w = 0; w < w3 ; w+=3)
      {
	base_in = w + h_iws;
	base_out = w + h_ows;

	d_out = data_out + base_out;
	d_in  = data_in + base_in;

	*(d_out) = *(d_in+2);
	*(d_out+1) = *(d_in+1);
	*(d_out+2) = *(d_in);
	/*
	*(data_out+base_out) = *(data_in+base_in+2);
	*(data_out+base_out+1) = *(data_in+base_in+1);
	*(data_out+base_out+2) = *(data_in+base_in);
	*/
      }
    }
  }
}


void brg24_to_bgr24(int width, int height, unsigned char *data_in, int in_widthstep, unsigned char *data_out, int out_widthstep)
{
  int h, w, base_in, base_out;
  int h_iws, h_ows;
  unsigned char *d_out, *d_in;
  int w3;

  w3 = width*3;

  if(data_out == NULL)
  {
    // convert and swap in the same pixmap

    for(h = 0; h < height/2 ; h++)
    {
      h_iws = h*in_widthstep;
      h_ows = h*out_widthstep;

      for(w = 0; w < width ; w++)
      {
	base_in = w*3 + h_iws;
	base_out = w*3 + h_ows;

	swap_(&data_in[base_out], &data_in[base_in+2]);
	swap_(&data_in[base_out+1], &data_in[base_in+0]);
	swap_(&data_in[base_out+2], &data_in[base_in+1]);
      }
    }
  }
  else
  {
    for(h = 0; h < height ; h++)
    {
      h_iws = h * in_widthstep;
      h_ows = h * out_widthstep;

      for(w = 0; w < w3 ; w+=3)
      {
	base_in = w + h_iws;
	base_out = w + h_ows;

	d_out = data_out + base_out;
	d_in  = data_in + base_in;

	*(d_out) = *(d_in+2);
	*(d_out+1) = *(d_in+0);
	*(d_out+2) = *(d_in+1);
	/*
	*(data_out+base_out) = *(data_in+base_in+2);
	*(data_out+base_out+1) = *(data_in+base_in+1);
	*(data_out+base_out+2) = *(data_in+base_in);
	*/
      }
    }
  }
}

void brg24_to_rgb24(int width, int height, unsigned char *data_in, int in_widthstep, unsigned char *data_out, int out_widthstep)
{
  int h, w, base_in, base_out;
  int h_iws, h_ows;
  unsigned char *d_out, *d_in;
  int w3;

  w3 = width*3;

  if(data_out == NULL)
  {
    // convert and swap in the same pixmap

    for(h = 0; h < height/2 ; h++)
    {
      h_iws = h*in_widthstep;
      h_ows = h*out_widthstep;

      for(w = 0; w < width ; w++)
      {
	base_in = w*3 + h_iws;
	base_out = w*3 + h_ows;

	swap_(&data_in[base_out], &data_in[base_in+1]);
	swap_(&data_in[base_out+1], &data_in[base_in+0]);
	swap_(&data_in[base_out+2], &data_in[base_in+2]);
      }
    }
  }
  else
  {
    for(h = 0; h < height ; h++)
    {
      h_iws = h * in_widthstep;
      h_ows = h * out_widthstep;

      for(w = 0; w < w3 ; w+=3)
      {
	base_in = w + h_iws;
	base_out = w + h_ows;

	d_out = data_out + base_out;
	d_in  = data_in + base_in;

	*(d_out) = *(d_in+1);
	*(d_out+1) = *(d_in+0);
	*(d_out+2) = *(d_in+2);
	/*
	*(data_out+base_out) = *(data_in+base_in+2);
	*(data_out+base_out+1) = *(data_in+base_in+1);
	*(data_out+base_out+2) = *(data_in+base_in);
	*/
      }
    }
  }
}
 
void rgb24_to_rgb24(int width, int height, unsigned char *data_in, int in_widthstep, unsigned char *data_out, int out_widthstep)
{
  int h;
  unsigned char *d_out, *d_in;
  int w3;

  w3 = width*3;

  for(h = 0; h < height ; h++)
  {
    d_out = data_out + h*out_widthstep;
    d_in  = data_in + h*in_widthstep;

    memcpy(d_out, d_in, w3);
  }
}
 

void rgb24_to_rgb24_max(int width, int height, unsigned char *data_in, int in_widthstep, unsigned char *data_out, int out_widthstep)
{
  int h, w, base_in, base_out;
  int h_iws, h_ows;
  unsigned char *d_out, *d_in;
  int w3;

  w3 = width*3;

  for(h = 0; h < height ; h++)
  {
    h_iws = h * in_widthstep;
    h_ows = h * out_widthstep;

    for(w = 0; w < w3 ; w+=3)
    {
      base_in = w + h_iws;
      base_out = w + h_ows;

      d_out = data_out + base_out;
      d_in  = data_in + base_in;

      /*
      if(*d_in==0 && *(d_in+1)==0 && *(d_in+2)==0)
      {
	*(d_out) = 0;
	*(d_out+1) = 0;
	*(d_out+2) = 0;
      }
      else
      */
      {
	if (*d_in>*(d_in+1) && *d_in>*(d_in+2))
	{
	  *(d_out) = 255; //*d_in;
	  *(d_out+1) = 0;
	  *(d_out+2) = 0;
	}
	else if (*(d_in+1)>*(d_in) && *(d_in+1)>*(d_in+2))
	{
	  *(d_out) = 0;
	  *(d_out+1) = 255; //*(d_in+1);
	  *(d_out+2) = 0;
	}
	else if (*(d_in+2)>*(d_in) && *(d_in+2)>*(d_in+1))
	{
	  *(d_out) = 0;
	  *(d_out+1) = 0;
	  *(d_out+2) = 255; //*(d_in+2);
	}
	else
	{
	  //printf("din: %d %d %d\n", *d_in, *(d_in+1), *(d_in+2));
	  *(d_out) = 0;
	  *(d_out+1) = 0;
	  *(d_out+2) = 0;
	}
      }
    }
  }
}
 
void rgb24_to_bgr24_max(int width, int height, unsigned char *data_in, int in_widthstep, unsigned char *data_out, int out_widthstep)
{
  int h, w, base_in, base_out;
  int h_iws, h_ows;
  unsigned char *d_out, *d_in;
  int w3;

  w3 = width*3;

  for(h = 0; h < height ; h++)
  {
    h_iws = h * in_widthstep;
    h_ows = h * out_widthstep;

    for(w = 0; w < w3 ; w+=3)
    {
      base_in = w + h_iws;
      base_out = w + h_ows;

      d_out = data_out + base_out;
      d_in  = data_in + base_in;

      /*
      if(*d_in==0 && *(d_in+1)==0 && *(d_in+2)==0)
      {
	*(d_out) = 0;
	*(d_out+1) = 0;
	*(d_out+2) = 0;
      }
      else
      */
      {
	if (*d_in>*(d_in+1) && *d_in>*(d_in+2))
	{
	  *(d_out) = 0; //*d_in;
	  *(d_out+1) = 0;
	  *(d_out+2) = 255;
	}
	else if (*(d_in+1)>*(d_in) && *(d_in+1)>*(d_in+2))
	{
	  *(d_out) = 0;
	  *(d_out+1) = 255; //*(d_in+1);
	  *(d_out+2) = 0;
	}
	else if (*(d_in+2)>*(d_in) && *(d_in+2)>*(d_in+1))
	{
	  *(d_out) = 255;
	  *(d_out+1) = 0;
	  *(d_out+2) = 0; //*(d_in+2);
	}
	else
	{
	  //printf("din: %d %d %d\n", *d_in, *(d_in+1), *(d_in+2));
	  *(d_out) = 0;
	  *(d_out+1) = 0;
	  *(d_out+2) = 0;
	}
      }
    }
  }
}
 

void rgb32_to_bgr24_max(int width, int height, unsigned char *data_in, int in_widthstep, unsigned char *data_out, int out_widthstep)
{
  int h, w, base_in, base_out;
  int h_iws, h_ows;
  unsigned char *d_out, *d_in;
  int w4;
  int wout;

  w4 = width*4;

  for(h = 0; h < height ; h++)
  {
    h_iws = h * in_widthstep;
    h_ows = h * out_widthstep;

    wout=0;
    for(w = 0; w < w4 ; w+=4)
    {
      base_in = w + h_iws;
      base_out = wout + h_ows;

      d_out = data_out + base_out;
      d_in  = data_in + base_in;

      /*
      if(*d_in==0 && *(d_in+1)==0 && *(d_in+2)==0)
      {
	*(d_out) = 0;
	*(d_out+1) = 0;
	*(d_out+2) = 0;
      }
      else
      */
      {
	if (*d_in>*(d_in+1) && *d_in>*(d_in+2) && *d_in>*(d_in+3))
	{
	  *(d_out) = 0; //*d_in;
	  *(d_out+1) = 0;
	  *(d_out+2) = 255;
	}
	else if (*(d_in+1)>*(d_in) && *(d_in+1)>*(d_in+2) && *(d_in+1)>*(d_in+3))
	{
	  *(d_out) = 0;
	  *(d_out+1) = 255; //*(d_in+1);
	  *(d_out+2) = 0;
	}
	else if (*(d_in+2)>*(d_in) && *(d_in+2)>*(d_in+1) && *(d_in+2)>*(d_in+3))
	{
	  *(d_out) = 255;
	  *(d_out+1) = 0;
	  *(d_out+2) = 0; //*(d_in+2);
	}
	else if (*(d_in+3)>*(d_in) && *(d_in+3)>*(d_in+1) && *(d_in+3)>*(d_in+2))
	{
	  *(d_out) = 0;
	  *(d_out+1) = 255;
	  *(d_out+2) = 255; //*(d_in+2);
	}
	else
	{
	  //printf("din: %d %d %d\n", *d_in, *(d_in+1), *(d_in+2));
	  *(d_out) = 0;
	  *(d_out+1) = 0;
	  *(d_out+2) = 0;
	}
      }
      wout+=3;
    }
  }
}


void rgb32_to_bgr24_3channelmax(int width, int height, unsigned char *data_in, int in_widthstep, unsigned char *data_out, int out_widthstep)
{
  int h, w, base_in, base_out;
  int h_iws, h_ows;
  unsigned char *d_out, *d_in;
  int w4;
  int wout;

  w4 = width*4;

  for(h = 0; h < height ; h++)
  {
    h_iws = h * in_widthstep;
    h_ows = h * out_widthstep;

    wout=0;
    for(w = 0; w < w4 ; w+=4)
    {
      base_in = w + h_iws;
      base_out = wout + h_ows;

      d_out = data_out + base_out;
      d_in  = data_in + base_in;

      /*
      if(*d_in==0 && *(d_in+1)==0 && *(d_in+2)==0)
      {
	*(d_out) = 0;
	*(d_out+1) = 0;
	*(d_out+2) = 0;
      }
      else
      */
      {
	if (*d_in>*(d_in+1) && *d_in>*(d_in+2)) // && *d_in>*(d_in+3))
	{
	  *(d_out) = 0; 
	  *(d_out+1) = 0;
	  *(d_out+2) = 255;
	  /*
	  *(d_out) = 255; 
	  *(d_out+1) = 0;
	  *(d_out+2) = 0;
	  */
	}
	else if (*(d_in+1)>*(d_in) && *(d_in+1)>*(d_in+2)) // && *(d_in+1)>*(d_in+3))
	{
	  *(d_out) = 0;
	  *(d_out+1) = 255; 
	  *(d_out+2) = 0;
	}
	else if (*(d_in+2)>*(d_in) && *(d_in+2)>*(d_in+1)) // && *(d_in+2)>*(d_in+3))
	{
	  *(d_out) = 255;
	  *(d_out+1) = 0;
	  *(d_out+2) = 0; 
	  /*
	  *(d_out) = 0;
	  *(d_out+1) = 0;
	  *(d_out+2) = 255; 
	  */
	}
	/*
	else if (*(d_in+3)>*(d_in) && *(d_in+3)>*(d_in+1) && *(d_in+3)>*(d_in+2))
	{
	  *(d_out) = 0;
	  *(d_out+1) = 255;
	  *(d_out+2) = 255; //*(d_in+2);
	}
	*/
	else
	{
	  //printf("din: %d %d %d\n", *d_in, *(d_in+1), *(d_in+2));
	  *(d_out) = 0;
	  *(d_out+1) = 0;
	  *(d_out+2) = 0;
	}
      }
      wout+=3;
    }
  }
}


 
void gray_to_rgb24(int width, int height, unsigned char *data_in, int in_width, unsigned char *data_out, int out_width)
{
  int h, w; //, base_in, base_out;
  //int h_iws, h_ows;
  unsigned char *d_out, *d_in;
  int w3;
  //int wg;

  w3 = width*3;

  d_in = data_in;
  d_out = data_out;

  if (in_width==width && out_width==w3)
  {
    for(h = 0; h < height ; h++)
    {
      /*
      h_iws = h * in_width;
      h_ows = h * out_width;
      */

      //wg=0;
      for(w = 0; w < width ; w++)
      {
	/*
	base_in = wg + h_iws;
	base_out = w + h_ows;

	d_out = data_out + base_out;
	d_in  = data_in + base_in;
	*/

	/* old
	*(d_out) = *(d_in);
	*(d_out+1) = *(d_in);
	*(d_out+2) = *(d_in);

	d_in++;
	d_out+=3;
	*/
	*d_out = *d_in;
	d_out++;
	*d_out = *d_in;
	d_out++;
	*d_out = *d_in;
	d_out++;
	d_in++;
	//wg++;
      }
      /*
      d_in += in_width;
      d_out += out_width;
      */
    }
  }
  else
  {
    int padin, padout;
    padin = in_width-width;
    padout = out_width-w3;

    for(h = 0; h < height ; h++)
    {
      for(w = 0; w < width ; w++)
      {
	/*
	*(d_out) = *(d_in);
	*(d_out+1) = *(d_in);
	*(d_out+2) = *(d_in);

	d_in++;
	d_out+=3;
	*/
	*d_out = *d_in;
	d_out++;
	*d_out = *d_in;
	d_out++;
	*d_out = *d_in;
	d_out++;
	d_in++;
      }
      d_in += padin;
      d_out += padout;
    }
  }
}


/*
void BGR_to_normalizedRGB(IplImage* src, IplImage* dst)
{
  IplImage* r_plane = cvCreateImage(  cvGetSize(src), 8, 1 );
  IplImage* g_plane = cvCreateImage(  cvGetSize(src), 8, 1 );                            
  IplImage* b_plane = cvCreateImage(  cvGetSize(src), 8, 1 );                            

  IplImage* B = cvCreateImage(  cvGetSize(src), 8, 1 );
  IplImage* G = cvCreateImage(  cvGetSize(src), 8, 1 );                          
  IplImage* R = cvCreateImage(  cvGetSize(src), 8, 1 );

  cvCvtPixToPlane( src, B, G, R, 0 );    

  IplImage* R32 = cvCreateImage(  cvGetSize(src), IPL_DEPTH_32F, 1 );
  IplImage* G32 = cvCreateImage(  cvGetSize(src), IPL_DEPTH_32F, 1 );
  IplImage* B32 = cvCreateImage(  cvGetSize(src), IPL_DEPTH_32F, 1 );
  IplImage* sum = cvCreateImage(  cvGetSize(src), IPL_DEPTH_32F, 1 );
  cvZero(sum);

  cvConvertScale(R, R32, 1, 0);
  cvConvertScale(G, G32, 1, 0);
  cvConvertScale(B, B32, 1, 0);

  cvAdd( R32, G32, sum, 0);
  cvAdd( B32, sum, sum, 0);// sum = R + G + B

  IplImage* tmp = cvCreateImage(cvGetSize(src), IPL_DEPTH_32F, 1);
  cvDiv( R32, sum, tmp, 1 );
  cvConvertScale( tmp, r_plane, 255, 0);
  cvDiv( G32, sum, tmp, 1 );
  cvConvertScale( tmp, g_plane, 255, 0);
  cvDiv( B32, sum, tmp, 1 );
  cvConvertScale( tmp, b_plane, 255, 0);

  // do nothing with b
  cvCvtPlaneToPix( r_plane, g_plane, b_plane, 0, dst);

  cvReleaseImage(&r_plane);
  cvReleaseImage(&g_plane);
  cvReleaseImage(&b_plane);
  cvReleaseImage(&R);
  cvReleaseImage(&G);
  cvReleaseImage(&B);
  cvReleaseImage(&R32);
  cvReleaseImage(&G32);
  cvReleaseImage(&B32);
  cvReleaseImage(&sum);
  cvReleaseImage(&tmp);

}
*/


void yuv_2_rgb(int y, int u, int v, unsigned char* r, unsigned char* g, unsigned char* b)
{
  int r_, g_, b_;
  u -= 128;
  v -= 128;

  r_ = (y + 1.370705 * v);
  g_ = (y - 0.698001 * v - 0.337633 * u);
  b_ = (y + 1.732446 * u);
  if (r_ < 0) r_ = 0;
  if (g_ < 0) g_ = 0;
  if (b_ < 0) b_ = 0;
  if (r_ > 255) r_ = 255;
  if (g_ > 255) g_ = 255;
  if (b_ > 255) b_ = 255;

  *r = (unsigned char)r_;
  *g = (unsigned char)g_;
  *b = (unsigned char)b_;
}

/*
void yuv_to_rgb24(int width, int height, unsigned char *data_in, int in_widthstep, unsigned char *data_out, int out_widthstep)
{
  int h, w, base_in, base_out;
  int base_in_u, base_in_v;
  int h_iws, h_ows;
  int ylength, ulength;
  unsigned char *d_out, *d_in;
  int w4, w3;
  unsigned char y1, y2, u, v;
  int w_rgb;

  w3 = width*3;
  w4 = width*4;
  ylength = width*height;
  ulength = width*height + width*height/4;

  {
    for(h = 0; h < height ; h++)
    {
      h_iws = h * in_widthstep;
      h_ows = h * out_widthstep;

      w_rgb=0;
      for(w = 0; w < width ; w+=2)
      {
	base_in = w + h_iws;
        base_in_u = ylength + (h/2*width/2) + w/2;
        base_in_v = ylength + ulength + (h/2*width/2) + w/2;
	base_out = w_rgb + h_ows;

	d_out = data_out + base_out;
	d_in  = data_in + base_in;

        u  = *(data_in + base_in_u);
        y1 = *(d_in+0);
        v  = *(data_in + base_in_v);
        y2 = *(d_in+1);

	yuv_2_rgb(y1, u, v, (d_out), (d_out+1), (d_out+2));
	yuv_2_rgb(y2, u, v, (d_out+3), (d_out+4), (d_out+5));
        
        w_rgb+=6;
      }
    }
  }
}
*/



void yuv_to_rgb24(int width, int height, unsigned char *data_in, int in_widthstep, unsigned char *data_out, int out_widthstep)
{
  int h, w, base_in, base_out;
  int h_iws, h_ows;
  unsigned char *d_out, *d_in;
  int w4, w3;
  unsigned char y1, y2, u, v;
  int w_rgb;

  w3 = width*3;
  w4 = width*4;

  {
    for(h = 0; h < height ; h++)
    {
      h_iws = h * in_widthstep;
      h_ows = h * out_widthstep;

      w_rgb=0;
      for(w = 0; w < w4 ; w+=4)
      {
	base_in = w + h_iws;
	base_out = w_rgb + h_ows;

	d_out = data_out + base_out;
	d_in  = data_in + base_in;

        u  = *(d_in+0);
        y1 = *(d_in+1);
        v  = *(d_in+2);
        y2 = *(d_in+3);

	yuv_2_rgb(y1, u, v, (d_out), (d_out+1), (d_out+2));
	yuv_2_rgb(y2, u, v, (d_out+3), (d_out+4), (d_out+5));
        
        w_rgb+=6;
      }
    }
  }
}


void yuv_to_bgr24(int width, int height, unsigned char *data_in, int in_widthstep, unsigned char *data_out, int out_widthstep)
{
  int h, w, base_in, base_out;
  int h_iws, h_ows;
  unsigned char *d_out, *d_in;
  int w4, w3;
  unsigned char y1, y2, u, v;
  int w_rgb;

  w3 = width*3;
  w4 = width*4;

  {
    for(h = 0; h < height ; h++)
    {
      h_iws = h * in_widthstep;
      h_ows = h * out_widthstep;

      w_rgb=0;
      for(w = 0; w < w4 ; w+=4)
      {
	base_in = w + h_iws;
	base_out = w_rgb + h_ows;

	d_out = data_out + base_out;
	d_in  = data_in + base_in;

        u  = *(d_in+0);
        y1 = *(d_in+1);
        v  = *(d_in+2);
        y2 = *(d_in+3);

	yuv_2_rgb(y1, u, v, (d_out+2), (d_out+1), (d_out+0));
	yuv_2_rgb(y2, u, v, (d_out+5), (d_out+4), (d_out+3));
        
        w_rgb+=6;
      }
    }
  }
}


// r,g,b values are from 0 to 1
// h = [0,360], s = [0,1], v = [0,1]
//              if s == 0, then h = -1 (undefined)
void RGBtoHSV( const double r, const double g, const double b, double& h, double& s, double& v) 
{
  double vmin, diff;

  v = vmin = r;
  if( v < g ) v = g;
  if( v < b ) v = b;
  if( vmin > g ) vmin = g;
  if( vmin > b ) vmin = b;

  diff = v - vmin;
  s = diff/(float)(fabs(v) + FLT_EPSILON);
  diff = (float)(60./(diff + FLT_EPSILON));
  if( v == r )
    h = (g - b)*diff;
  else if( v == g )
    h = (b - r)*diff + 120.f;
  else
    h = (r - g)*diff + 240.f;

   if( h < 0 ) h += 360.f;
}


void RGBtoHSV( const unsigned char r, const unsigned char g, const unsigned char b, unsigned char& h, unsigned char& s, unsigned char& v) 
{
  double dh=(double)h, ds=(double)s, dv=(double)v;
  RGBtoHSV((double)r, (double)g, (double)b, dh, ds, dv);
  // normalise values to [0,255]
  h = (unsigned char)std::min(std::max(0.0,(dh/360.0*255)),255.0);
  s = (unsigned char)std::min(std::max(0.0,ds*255),255.0);
  v = (unsigned char)dv;
}

void RGBtoXYZ( const unsigned char r, const unsigned char g, const unsigned char b, unsigned char& x, unsigned char& y, unsigned char& z) 
{
  x = (unsigned char)(   0.49*r +    0.31*g +     0.2*b);
  y = (unsigned char)(0.17697*r + 0.81240*g + 0.01063*b);
  z = (unsigned char)(               0.01*g +    0.99*b);
}

void RGBtoYCbCr( const unsigned char r, const unsigned char g, const unsigned char b, unsigned char& y, unsigned char& cb, unsigned char& cr) 
{
  y  = (unsigned char)( 16 +  (65.738*r)/256 + (129.057*g)/256 +  (25.064*b)/256);
  cb = (unsigned char)(128 -  (37.945*r)/256 -  (74.494*g)/256 + (112.439*b)/256);
  cr = (unsigned char)(128 + (112.439*r)/256 -  (94.154*g)/256 -  (18.285*b)/256);
}

}
