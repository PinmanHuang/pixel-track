#include <algorithm>
#include "tltypes.h"
#include "HSVPixelModel.h"
#include "BGR2HSVdistLUT.h"

using namespace std;


bool disp_less(displacement_t lhs, displacement_t rhs)
{
  if (lhs.y<rhs.y)
    return true;
  else if (lhs.y>rhs.y)
    return false;
  else return (lhs.x<rhs.x);
};

bool disp_less_count(displacement_t lhs, displacement_t rhs)
{
  return (lhs.count<rhs.count);
};


HSVPixelModel::HSVPixelModel(int nb_hsbins, int nb_vbins)
{
  h_bins = nb_hsbins;
  s_bins = nb_hsbins;
  v_bins = nb_vbins;
  totalbins = h_bins*s_bins+v_bins;

  hsv_disp = new list<displacement_t>*[totalbins];
  for(int i=0; i<totalbins; i++)
    hsv_disp[i] = new list<displacement_t>;

  m_LUT = new BGR2HSVdistLUT(h_bins, s_bins, v_bins); 
  reset();
}

HSVPixelModel::~HSVPixelModel()
{
  for(int i=0; i<totalbins; i++)
    delete hsv_disp[i];
  delete [] hsv_disp;
  delete m_LUT;
}

void HSVPixelModel::reset()
{
  for(int i=0; i<totalbins; i++)
    hsv_disp[i]->clear();
}


void HSVPixelModel::learn(Image8U* img, Rectangle bb, Image<float>* segmentation)
{
  int curline, curcol;
  int index;
  displacement_t disp;
  list<displacement_t>::iterator it;
  int pi, pj;
  int ll=bb.lastLine(), lc = bb.lastColumn();
  Colour pix;
  int ii, jj;

  ii=0;
  for(int i=bb.miFirstLine; i<ll; i++)
  {
    jj=0;
    for(int j=bb.miFirstColumn; j<lc; j++)
    {
      if (segmentation->get(j, i)>0.5)
      {
	pix = img->getColourBGR(j, i);

	index = m_LUT->hsv_bin(pix.r, pix.g, pix.b);
	disp.y=ii-bb.miHeight/2; 
	disp.x=jj-bb.miWidth/2; 

	it = find(hsv_disp[index]->begin(), hsv_disp[index]->end(), disp);
	if (it==hsv_disp[index]->end())
	{
	  disp.count=1;
	  hsv_disp[index]->push_back(disp);
	}
	else
	  it->count++;
      }
      jj++;
    }
    ii++;
  }
  for(int i=0; i<totalbins; i++)
    hsv_disp[i]->sort(disp_less_count); 
}


void HSVPixelModel::vote(Image8U* img, Rectangle bb, Image<float>* voting_map)
{
  int lastline, lastcol;
  int index; 
  displacement_t disp;
  int width, height;
  width = img->width();
  height = img->height();
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
  int ws = img->widthStep();
  int vmws = voting_map->widthStep();
  int roinx = bb.miWidth/xgrid_step;
  int roiny = bb.miHeight/ygrid_step;
  int pixelstep = (xgrid_step-1)*3;
  int xstep = ws-xgrid_step*roinx*3 + (ygrid_step-1)*ws;
  iptr = (unsigned char*)img->data()+bb.miFirstLine*ws + bb.miFirstColumn*3;
  float* vmd = voting_map->data();

  for(si=0; si<roiny; si++)
  {
    i=bb.miFirstLine+si*ygrid_step;
    for(sj=0; sj<roinx; sj++)
    {
      j=bb.miFirstColumn+sj*xgrid_step;
      b = *iptr++; 
      g = *iptr++;
      r = *iptr++;

      index = m_LUT->hsv_bin(r,g,b);
      it=hsv_disp[index]->end();
      beg=hsv_disp[index]->begin();
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
      iptr+=pixelstep;
    }
    iptr+=xstep;
  }
}


void HSVPixelModel::backproject(Image8U* img, Rectangle search_window, Image<float>* bpimg, int maxlocx, int maxlocy)
{
  int lastline, lastcol;
  int index; 
  displacement_t disp;
  int width, height;
  width = img->width();
  height = img->height();
  Rectangle imgBB(0, 0, width, height);
  search_window.intersection(imgBB); 
  lastline = search_window.lastLine();
  lastcol = search_window.lastColumn();
  list<displacement_t>::iterator it;
  list<displacement_t>::iterator beg;
  int counter;
  unsigned char r, g, b;
  unsigned char bpr, bpg, bpb;
  int i, j, si, sj;
  int i_minus_maxlocy, j_minus_maxlocx;
  int xisw_minus_maxlocx, yisw_minus_maxlocy;
  int xtrans, ytrans;
  float center_dist;
  int ws = img->widthStep();
  int xgrid_step = max(1,search_window.miWidth/300);
  int ygrid_step = max(1,search_window.miHeight/300);
  int roinx = search_window.miWidth/xgrid_step;
  int roiny = search_window.miHeight/ygrid_step;
  int pixelstep = (xgrid_step-1)*3;
  int xstep = ws-xgrid_step*roinx*3 + (ygrid_step-1)*ws;
  unsigned char* iptr = (unsigned char*)img->data()+search_window.miFirstLine*ws + search_window.miFirstColumn*3;

  for(si=0; si<roiny; si++)
  {
    i=search_window.miFirstLine+si*ygrid_step;
    i_minus_maxlocy = i-maxlocy;
    for(sj=0; sj<roinx; sj++)
    {
      j=search_window.miFirstColumn+sj*xgrid_step;
      j_minus_maxlocx = j-maxlocx;

      b = *(iptr++); 
      g = *(iptr++);
      r = *(iptr++);
      index = m_LUT->hsv_bin(r,g,b);
      it=hsv_disp[index]->end();
      beg=hsv_disp[index]->begin();
      counter=0;
      while (counter<MAXVOTES && it!=beg)
      {
	--it;
	xtrans = it->x; 
	ytrans = it->y; 
	center_dist = sqrtf((ytrans-i_minus_maxlocy)*(ytrans-i_minus_maxlocy) + (xtrans-j_minus_maxlocx)*(xtrans-j_minus_maxlocx));
	bpimg->add(j, i, it->count/(1+center_dist));
	counter++;
      }
      iptr+=pixelstep;
    }
    iptr+=xstep;
  }
}


void HSVPixelModel::backproject(Image8U* img, Rectangle search_window, Image<float>* bpimg, int maxlocx, int maxlocy, Image<float>* segmentation, 
    float& mean_pos, float& variance_pos, float& mean_neg, float& variance_neg)
{
  int lastline, lastcol;
  int index; 
  displacement_t disp;
  int width, height;
  width = img->width();
  height = img->height();
  Rectangle imgBB(0, 0, width, height);
  search_window.intersection(imgBB); 
  lastline = search_window.lastLine();
  lastcol = search_window.lastColumn();
  list<displacement_t>::iterator it;
  list<displacement_t>::iterator beg;
  int counter;
  unsigned char r, g, b;
  unsigned char bpr, bpg, bpb;
  int i, j, si, sj;
  int x_in_searchwin, y_in_searchwin;
  int i_minus_maxlocy, j_minus_maxlocx;
  int xtrans, ytrans;
  float center_dist;
  int ws = img->widthStep();
  int xgrid_step = max(1,search_window.miWidth/300);
  int ygrid_step = max(1,search_window.miHeight/300);
  int roinx = search_window.miWidth/xgrid_step;
  int roiny = search_window.miHeight/ygrid_step;
  int pixelstep = (xgrid_step-1)*3;
  int xstep = ws-xgrid_step*roinx*3 + (ygrid_step-1)*ws;
  unsigned char* iptr = (unsigned char*)img->data()+search_window.miFirstLine*ws + search_window.miFirstColumn*3;
  float vote_err_pos=0, vote_err_pos2=0;
  float vote_err_neg=0, vote_err_neg2=0;
  float fg, bg, fg_sum=0, bg_sum=0;

  for(si=0; si<roiny; si++)
  {
    i=search_window.miFirstLine+si*ygrid_step;
    i_minus_maxlocy = i-maxlocy;
    for(sj=0; sj<roinx; sj++)
    {
      j=search_window.miFirstColumn+sj*xgrid_step;
      j_minus_maxlocx = j-maxlocx;
      fg = segmentation->get(j, i);
      bg = 1.0-fg;

      b = *(iptr++); 
      g = *(iptr++);
      r = *(iptr++);
      index = m_LUT->hsv_bin(r,g,b);
      it=hsv_disp[index]->end();
      beg=hsv_disp[index]->begin();
      counter=0;
      if (fg>0.5)
      {
	while (counter<MAXVOTES && it!=beg)
	{
	  --it;
	  xtrans = it->x; 
	  ytrans = it->y; 
	  center_dist = sqrtf((ytrans-i_minus_maxlocy)*(ytrans-i_minus_maxlocy) + (xtrans-j_minus_maxlocx)*(xtrans-j_minus_maxlocx));
	  vote_err_pos += fg*center_dist;
	  vote_err_pos2 += fg*center_dist*center_dist;
	  fg_sum += fg;

	  bpimg->add(j, i, it->count/(1+center_dist));
	  counter++;
	}
      }
      else
      {
	while (counter<MAXVOTES && it!=beg)
	{
	  --it;
	  xtrans = it->x; 
	  ytrans = it->y; 
	  center_dist = sqrtf((ytrans-i_minus_maxlocy)*(ytrans-i_minus_maxlocy) + (xtrans-j_minus_maxlocx)*(xtrans-j_minus_maxlocx));
	  vote_err_neg += bg*center_dist;
	  vote_err_neg2 += bg*center_dist*center_dist;
	  bg_sum += bg;

	  bpimg->add(j, i, it->count/(1+center_dist));
	  counter++;
	}
      }
      iptr+=pixelstep;
    }
    iptr+=xstep;
  }
  mean_pos = vote_err_pos/fg_sum;
  variance_pos = vote_err_pos2/fg_sum - mean_pos*mean_pos;
  mean_neg = vote_err_neg/bg_sum;
  variance_neg = vote_err_neg2/bg_sum - mean_neg*mean_neg;
  cout << "voting error positive:  mean: " << mean_pos << endl;
  cout << "voting error positive:  var:  " << variance_pos << endl;
  cout << "voting error negative:  mean: " << mean_neg << endl;
  cout << "voting error negative:  var:  " << variance_neg << endl;
}



