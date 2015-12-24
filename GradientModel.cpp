#include <algorithm>
#include "tltypes.h"
#include "GradientModel.h"

using namespace std;


GradientModel::GradientModel(int nb_obins, int nb_mbins, float mag_thresh)
{
  o_bins = nb_obins;
  m_bins = nb_mbins;
  totalbins = o_bins*m_bins+1;
  magnitude_threshold = mag_thresh;

  disp = new list<displacement_t>*[totalbins];
  for(int i=0; i<totalbins; i++)
    disp[i] = new list<displacement_t>;

  m_LUT = new GradDispLUT(o_bins, m_bins); 
  reset();
}

GradientModel::~GradientModel()
{
  for(int i=0; i<totalbins; i++)
    delete disp[i];
  delete [] disp;
  delete m_LUT;
}

void GradientModel::reset()
{
  for(int i=0; i<totalbins; i++)
    disp[i]->clear();
}


void GradientModel::learn(Image<short>* xgradimg, Image<short>* ygradimg, Rectangle bb, Image<float>* segmentation)
{
  int curline, curcol;
  unsigned int index;
  displacement_t cur_disp;
  list<displacement_t>::iterator it;
  int pi, pj;
  int ll=bb.lastLine(), lc = bb.lastColumn();
  Colour pix;
  short gx, gy;
  int ii, jj;

  ii=0;
  for(int i=bb.miFirstLine; i<ll; i++)
  {
    jj=0;
    for(int j=bb.miFirstColumn; j<lc; j++)
    {
      if (segmentation->get(j, i)>0.5)
      {
      gx = xgradimg->get(j, i);
      gy = ygradimg->get(j, i);
      
      index= m_LUT->get_bin(gx, gy);
      cur_disp.y=ii-bb.miHeight/2; 
      cur_disp.x=jj-bb.miWidth/2; 

      it = find(disp[index]->begin(), disp[index]->end(), cur_disp);
      if (it==disp[index]->end())
      {
	cur_disp.count=1;
	disp[index]->push_back(cur_disp);
      }
      else
        it->count++;
      }
      jj++;
    }
    ii++;
  }
  for(int i=0; i<totalbins; i++)
    disp[i]->sort(disp_less_count); 
}


void GradientModel::vote(Image<short>* xgradimg, Image<short>* ygradimg, Rectangle bb, Image<float>* voting_map)
{
  int lastline, lastcol;
  int index;
  displacement_t cur_disp;
  int width, height;
  width = xgradimg->width();
  height = xgradimg->height();
  Rectangle imgBB(0, 0, width, height);
  bb.intersection(imgBB); 
  lastline = bb.lastLine();
  lastcol = bb.lastColumn();
  list<displacement_t>::iterator it;
  list<displacement_t>::iterator beg;
  int counter;
  unsigned char r, g, b;
  float* voting_ptr;
  int nx, ny;
  int ii, jj;
  float xtrans, ytrans;
  int i, j;
  int si, sj;
  int xgrid_step = max(1,bb.miWidth/30);
  int ygrid_step = max(1,bb.miHeight/30);
  unsigned char* iptr;
  short *xptr, *yptr;
  int vmws = voting_map->widthStep();
  int roinx = bb.miWidth/xgrid_step;
  int roiny = bb.miHeight/ygrid_step;
  int gradpixelstep = (xgrid_step);
  int gradxstep = xgradimg->widthStep()-xgrid_step*roinx + (ygrid_step-1)*xgradimg->widthStep();
  xptr = xgradimg->data()+bb.miFirstLine*xgradimg->widthStep() + bb.miFirstColumn;
  yptr = ygradimg->data()+bb.miFirstLine*ygradimg->widthStep() + bb.miFirstColumn;
  float* vmd = voting_map->data();
  short gx, gy;

  for(si=0; si<roiny; si++)
  {
    i=bb.miFirstLine+si*ygrid_step;
    for(sj=0; sj<roinx; sj++)
    {
      j=bb.miFirstColumn+sj*xgrid_step;
      gx = *xptr;
      gy = *yptr;

      index = m_LUT->get_bin(gx, gy);
      it=disp[index]->end();
      beg=disp[index]->begin();
      counter=0;
      while (counter<MAXVOTES && it!=beg)
      {
	--it;
	ny = int(i-it->y);
	nx = int(j-it->x);
	if (ny>0 && nx>0 && ny<height && nx<width)
	{
	  voting_ptr = &(( (float*)(vmd + vmws*(ny)) )[nx]);
	  *voting_ptr += it->count;
	}
	counter++;
      }
      xptr+=gradpixelstep;
      yptr+=gradpixelstep;
    }
    xptr+=gradxstep;
    yptr+=gradxstep;
  }
}


void GradientModel::backproject(Image<short>* xgradimg, Image<short>* ygradimg, Rectangle bb, Image<float>* bpimg, int maxlocx, int maxlocy)
{
  int lastline, lastcol;
  int index;
  displacement_t cur_disp;
  int width, height;
  width = xgradimg->width();
  height = xgradimg->height();
  Rectangle imgBB(0, 0, width, height);
  bb.intersection(imgBB); 
  lastline = bb.lastLine();
  lastcol = bb.lastColumn();
  list<displacement_t>::iterator it;
  list<displacement_t>::iterator beg;
  int counter;
  unsigned char r, g, b;
  float* voting_ptr;
  int nx, ny;
  int ii, jj;
  float xtrans, ytrans;
  int i, j;
  int si, sj;
  int xgrid_step = max(1,bb.miWidth/30);
  int ygrid_step = max(1,bb.miHeight/30);
  unsigned char* iptr;
  short *xptr, *yptr;
  int roinx = bb.miWidth/xgrid_step;
  int roiny = bb.miHeight/ygrid_step;
  int gradpixelstep = (xgrid_step-1);
  int gradxstep = xgradimg->widthStep()-xgrid_step*roinx + (ygrid_step-1)*xgradimg->widthStep();
  xptr = xgradimg->data()+bb.miFirstLine*xgradimg->widthStep() + bb.miFirstColumn;
  yptr = ygradimg->data()+bb.miFirstLine*ygradimg->widthStep() + bb.miFirstColumn;
  short gx, gy;
  int i_minus_maxlocy, j_minus_maxlocx;
  float center_dist;

  for(si=0; si<roiny; si++)
  {
    i=bb.miFirstLine+si*ygrid_step;
    i_minus_maxlocy = i-maxlocy;
    for(sj=0; sj<roinx; sj++)
    {
      j=bb.miFirstColumn+sj*xgrid_step;
      j_minus_maxlocx = j-maxlocx;

      gx = *xptr++;
      gy = *yptr++;

      index = m_LUT->get_bin(gx, gy);
      it=disp[index]->end();
      beg=disp[index]->begin();
      counter=0;
      while (counter<MAXVOTES && it!=beg)
      {
	--it;
	ny = int(i-it->y);
	nx = int(j-it->x);
	center_dist = sqrtf((ytrans-i_minus_maxlocy)*(ytrans-i_minus_maxlocy) + (xtrans-j_minus_maxlocx)*(xtrans-j_minus_maxlocx));
	bpimg->add(j, i, it->count/(1+center_dist));
	counter++;
      }
      xptr+=gradpixelstep;
      yptr+=gradpixelstep;
    }
    xptr+=gradxstep;
    yptr+=gradxstep;
  }
}


