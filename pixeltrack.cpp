//#define PROFILE


#include <getopt.h>
#ifdef PROFILE
#include <google/profiler.h>
#endif

#include "Image.h"
#include "BGR2HSVhistLUT.h"
#include "VideoInputFile.h"
#include "ImageOutput.h"
#include "utils_cv.h"
#include "OutputXMLFile.h"
#include "OutputTXTFile.h"
#include "VideoOutput.h"
#include "Timer.h"
#include "Error.h"
#include "HSVPixelModel.h"
#include "HSVPixelGradientModel.h"
#include "GradientModel.h"
#include "PixelClassColourModel.h"
#ifdef VOT_CHALLENGE
#include "vot.hpp"
#include "VideoInputVOT.h"
#endif

using namespace TLImageProc;
using namespace TLInOut;
using namespace TLUtil;

#define VERSION_MAJOR 0
#define VERSION_MINOR 3

//#define DEBUG_VERBOSITY 2

typedef struct params
{
  string filename;
  bool silent;
  int skip_frames;
  float search_size;
  string bbox;
  int from_frame;
  int to_frame;
  bool output;
  float detector_update_factor;
  float segmentation_update_factor;;
  bool vot_mode;
  bool pause;
} params;


void set_default_params(params* p)
{
  p->filename="";
  p->silent=false;
  p->skip_frames=0;
  p->search_size=2;
  p->bbox="";
  p->from_frame=0;
  p->to_frame=1e8;
  p->output=false;
  p->detector_update_factor=0.1;
  p->segmentation_update_factor=0.1;
  p->vot_mode=false;
  p->pause=false;
}


void print_usage(params* p)
{
  cout << "pixeltrack " << VERSION_MAJOR << "." << VERSION_MINOR << endl;
  cout << "© 2012 by Stefan Duffner, LIRIS/CNRS, France" << endl;

  cout << "Usage: pixeltrack <options> <video_file>" << endl;
  cout << "  options:" << endl;
  cout << "    -b (--bbox) x,y,w,h   initial bounding box parameters (default: " << p->bbox << ")" << endl;
  cout << "    -f (--from) N         start from frame number N (default: " << p->from_frame << ")" << endl;
  cout << "    -k (--skip_frames) N  skip N frames at each iteration(default: " << p->skip_frames << ")" << endl;
  cout << "    -o (--output)         output result in a video file (default: " << p->output << ")" << endl;
  cout << "    -p (--pause)          pause after each frame (default: " << p->pause << ")" << endl;
  cout << "    -s (--silent)         no image output  (default: " << p->silent << ")" << endl;
  cout << "    -t (--to) N           stop at frame number N (default: " << p->to_frame << ")" << endl;
  cout << "    -u (--det_update)     detector update factor (default: " << p->detector_update_factor << ")" << endl;
  cout << "    -v (--seg_update)     segmentation update factor (default: " << p->segmentation_update_factor << ")" << endl;
  cout << "    -w (--search_size)    relative search window size (default: " << p->search_size << ")" << endl;
  cout << "    -z (--vot_mode)       run in VOT mode (default: " << p->vot_mode << ")" << endl;
}


int main(int argc, char** argv)
{
  setlocale(LC_ALL, "C");
  params par;
  set_default_params(&par);

  int option_index=0;
  int opt;
  opterr=0;
  static struct option long_options[] =
  {
    {"bbox",          required_argument, 0, 'b'},
    {"from",          required_argument, 0, 'f'},
    {"skip_frames",   required_argument, 0, 'k'},
    {"output",        no_argument,       0, 'o'},
    {"pause",         no_argument,       0, 'p'},
    {"silent",        no_argument,       0, 's'},
    {"to",            required_argument, 0, 't'},
    {"det_update",    required_argument, 0, 'u'},
    {"seg_update",    required_argument, 0, 'v'},
    {"search_size",   required_argument, 0, 'w'},
    {"vot mode",      no_argument,       0, 'z'},
    {0, 0, 0, 0}
  };
  do
  {
    opt = getopt_long(argc, argv, "b:f:k:opst:u:v:w:z", long_options, &option_index);
    if (opt==-1)
      break;

    switch (opt)
    {
      case 'b':
        par.bbox = optarg;
        break;
      case 'f':
        par.from_frame = atoi(optarg);
        break;
      case 'k':
        par.skip_frames = atoi(optarg);
        break;
      case 'o':
        par.output = true;
        break;
      case 'p':
	par.pause = true;
	break;
      case 's':
        par.silent = true;
        break;
      case 't':
        par.to_frame = atoi(optarg);
        break;
      case 'u':
        par.detector_update_factor = atof(optarg);;
        break;
      case 'v':
        par.segmentation_update_factor = atof(optarg);;
        break;
      case 'w':
        par.search_size = atof(optarg);;
        break;
      case 'z':
        par.vot_mode = true;
        break;
      case '?':
        print_usage(&par);
        return -1;
    }
  } while (opt!=-1);

#ifdef VOT_CHALLENGE
  VOT* vot=NULL;
  if (par.vot_mode)
  {
    vot = new VOT("region.txt", "images.txt", "output.txt");
  }
#endif

  if (!par.vot_mode)
  {
    if (argc!=optind+1)
    {
      cerr << "Please specify video input file." << endl;
      print_usage(&par);
      return -1;
    }
    par.filename = argv[optind];
  }

  VideoInput* vinput;
#ifdef VOT_CHALLENGE
  if (par.vot_mode)
    vinput = new VideoInputVOT(vot);
  else
    vinput = new VideoInputFile(par.filename);
  float vot_scale_factor=0.8;
#else
  vinput = new VideoInputFile(par.filename);
#endif
  ImageOutput output_window("pixeltrack output");
  ImageOutput voting_window("voting map");
  ImageOutput bp_window("backprojection");
  ImageOutput seg_window("segmentation");
  OutputXMLFile xmlout("ptout.xml", par.skip_frames);
  OutputTXTFile txtout("ptout.txt", par.skip_frames);
  VideoOutput* voutput=NULL;
  Image<unsigned char>* cur_image; 
  Rectangle initial_rect;
  Rectangle bbox(par.bbox);
  int width, height;
  HSVPixelGradientModel* model = new HSVPixelGradientModel(16, 16, 8, 1, 60); // best
  Rectangle cur_bb, search_window;
  Rectangle outer_bb;
  int maxx, maxy;
  int cm_maxx, cm_maxy;
  float mean_fg_vote_err, mean_bg_vote_err, var_fg_vote_err, var_bg_vote_err;
  float initial_variance;
  float normalised_variance;
  float uncertainty;
  float mean_seg_uniformity, mean_votingmap_variance;
  float cur_seg_uniformity, cur_votingmap_variance;
  float cur_seg_change;
  float seg_ratio, vot_ratio;
  int prev_shift_x=0, prev_shift_y=0;
  Rectangle prev_bb;
  float update_factor_scale=1.0;
  
  int current_frame=0;
  int processed_frame=0;

  for(int i=0; i<par.from_frame; i++)
  {
    if (!vinput->nextImage())
      break;
    current_frame++;
  }

  // manual initialisation
  if (vinput->nextImageAvailable())
  {
    cur_image = vinput->nextImage();
#ifdef VOT_CHALLENGE
    if (par.vot_mode)
    {
      cv::Rect r = vot->getInitRectangle();
      initial_rect.initPosAndSize(r.x, r.y, r.width, r.height);
      initial_rect.enlarge(vot_scale_factor);
    }
    else
    {
      if (bbox.miWidth<=0 || bbox.miHeight<=0)
      {
	output_window.showImage(cur_image, 20);
	initial_rect = output_window.selectROI();
      }
      else
	initial_rect = bbox;
    }
#else
    if (bbox.miWidth<=0 || bbox.miHeight<=0)
    {
      output_window.showImage(cur_image, 20);
      initial_rect = output_window.selectROI();
    }
    else
      initial_rect = bbox;
#endif
  }
  else
  {
    cerr << "No input data." << endl;
    return EXIT_FAILURE;
  }

  MESSAGE(0, "FPS: " << vinput->getFPS());
  width = cur_image->width();
  height = cur_image->height();

  if (par.output)
  {
    voutput = new VideoOutput("out.avi", width, height, vinput->getFPS());
  }


  Image<unsigned char>* grey_img = new Image<unsigned char>(width, height, 1);

  Image<short>* xgrad_img = new Image<short>(width, height, 1);
  Image<short>* ygrad_img = new Image<short>(width, height, 1);
  xgrad_img->setZero();
  ygrad_img->setZero();
  cur_image->toGreyScale(grey_img);
  grey_img->sobelX(xgrad_img);
  grey_img->sobelY(ygrad_img);

  Image<float>* segmentation = new Image<float>(width, height, 1);
  Image<float>* seg_prior;
  segmentation->setZero();
  Image<float>* voting_map = new Image<float>(width, height, 1);
  Image<float>* voting_map_normalised = new Image<float>(width, height, 1);
  voting_map->setZero();
  Image<float>* bp_img = new Image<float>(width, height, 1);
  Image<float>* bp_img_normalised = new Image<float>(width, height, 1);
  bp_img->setZero();

  outer_bb = initial_rect;
  outer_bb.enlarge(1.5);
  cur_bb = initial_rect;
  search_window = cur_bb;
  search_window.enlarge(par.search_size);
  maxx = cur_bb.centerX();
  maxy = cur_bb.centerY();
  
  // learn pixel colour model (FG / BG)
  int nscales=2;
  int nbins=12;
  ColourHistLUT** lut = new ColourHistLUT*[nscales];
  for(int s=0;s<nscales;s++)
    lut[s] = new BGR2HSVhistLUT(nbins*(s+1), nbins*(s+1), nbins*(s+1));

  // create colour segmentation model
  PixelClassColourModel pccm(lut, nbins, nbins, nbins, nscales, width, height);
  pccm.create(cur_image, &outer_bb, &initial_rect);
  // do a first segmentation
  pccm.evaluateColour(cur_image, &outer_bb, false, segmentation);
  seg_prior = segmentation->clone();
  // set first tracking result to average of (outer) selected rectangle and centre of mass of segmentation
  segmentation->centreOfMass(outer_bb, cm_maxx, cm_maxy);
  //cur_bb.setCenter(0.5*(maxx+cm_maxx), 0.5*(maxy+cm_maxy));
  cur_bb.setCenter(cm_maxx, cm_maxy);
  prev_bb = cur_bb;

  Rectangle larger, smaller;
  int nbfg1, nbbg1, nbfg2, nbbg2, nbfg3, nbbg3;
  float fgbg_r1, fgbg_r2, fgbg_r3;

  larger = cur_bb;
  larger.enlarge(1.2);
  smaller = cur_bb;
  smaller.enlarge(1.0/1.2);
  nbfg1 = segmentation->sumGreaterThanThreshold(larger, 0.5);
  nbfg2 = segmentation->sumGreaterThanThreshold(cur_bb, 0.5);
  nbfg3 = segmentation->sumGreaterThanThreshold(smaller, 0.5);
  nbbg1 = larger.area()-nbfg1;
  nbbg2 = cur_bb.area()-nbfg2;
  nbbg3 = smaller.area()-nbfg3;
  fgbg_r1 = (float)nbfg1/nbbg1;
  fgbg_r2 = (float)nbfg2/nbbg2;
  fgbg_r3 = (float)nbfg3/nbbg3;
  MESSAGE(0, "bb1: " << fgbg_r1);
  MESSAGE(0, "bb2: " << fgbg_r2);
  MESSAGE(0, "bb3: " << fgbg_r3);

  if (fgbg_r1>1*fgbg_r2)
  {
    if (fgbg_r1>fgbg_r3)
      cur_bb = larger;
    else
      cur_bb = smaller;
  }
  else
  {
    if (fgbg_r2*1<fgbg_r3)
      cur_bb = smaller;
  }
  search_window = cur_bb;
  search_window.enlarge(par.search_size);

  

  // opening (not sure the first opening is good for model->learn())
  int erosion_type = cv::MORPH_ELLIPSE;
  int erosion_w = int(float(cur_bb.miWidth)/100+.5);
  int erosion_h = int(float(cur_bb.miHeight)/100+.5);
  MESSAGE(0, "erosion_w: " << erosion_w);
  /*
  cv::Mat element = cv::getStructuringElement( erosion_type, cv::Size( 2*erosion_w + 1, 2*erosion_h+1 ), cv::Point( erosion_w, erosion_h ) );
  cv::Mat mseg(segmentation->height(), segmentation->width(), CV_32F, segmentation->data(), segmentation->width()*sizeof(float));
  cv::erode( mseg, mseg, element );
  cv::dilate( mseg, mseg, element );
  */

  // learn pixel model
  model->learn(cur_image, xgrad_img, ygrad_img, search_window, segmentation);
  model->backproject(cur_image, xgrad_img, ygrad_img, search_window, bp_img, maxx, maxy);
  // do a first update to re-inforce pixels with "correct" backprojection
  pccm.update(cur_image, &cur_bb, segmentation, bp_img, 0.1);
  model->update(cur_image, xgrad_img, ygrad_img, search_window, bp_img, 0.2); 

  // output first tracking result to XML file
  xmlout.sendBB(&cur_bb, 0, 0, 1.0);
  txtout.sendBB(&cur_bb, 0, 0, 1.0);
  xmlout.commit(current_frame, vinput->getCurrentTimestampMs(), vinput->getCurrentTimestampMs());
  txtout.commit(current_frame, vinput->getCurrentTimestampMs(), vinput->getCurrentTimestampMs());
#ifdef VOT_CHALLENGE
  if (par.vot_mode)
  {
    Rectangle original_size_r = cur_bb;
    original_size_r.enlarge(1.0/vot_scale_factor);
    cv::Rect r(original_size_r.miFirstColumn, original_size_r.miFirstLine, original_size_r.miWidth, original_size_r.miHeight);
    vot->outputBoundingBox(r);
  }
#endif

  output_window.setCurrentImage(cur_image);
  // draw tracking result
  if (!par.silent || par.output)
  {
    output_window.draw(cur_bb, Colour(255,0,0), 2);
    output_window.draw(maxx, maxy, Colour(255,0,0), 2);
    output_window.draw(cm_maxx, cm_maxy, Colour(0,0,255), 2);
    output_window.draw(0.5*(maxx+cm_maxx), 0.5*(maxy+cm_maxy), Colour(0,255,0), 2);
  }

  if (par.output)
  {
    voutput->addFrame(cur_image);
  }

    if (!par.silent)
    {
      bp_img->multiply(1, bp_img_normalised);
      bp_window.showFloatImage(bp_img_normalised, 2);
      segmentation->binarise(0.5);
      seg_window.showFloatImage(segmentation, 2);
      if (par.pause)
	output_window.wait(0);

    }
    bp_img->setZero();
    segmentation->setZero();


  Timer avgfps_timer;
  avgfps_timer.reset();
  long long current_time, prev_time;
  float avgfps=0;
#ifdef PROFILE
  ProfilerStart("profile");
#endif

  // 
  // MAIN TRACKING LOOP
  // 
  current_time = avgfps_timer.getRunTime();
  do
  {
    current_frame++;
    MESSAGE(0, "+++++++++++ Frame: " << current_frame << "++++++++++++++++++++++++")

    // retrieve next image
    for(int i=0; i<par.skip_frames; i++)
    {
      if (!vinput->nextImage())
	break;
      current_frame++;
    }
    cur_image = vinput->nextImage();
    if (!cur_image)
      break;

    cur_image->toGreyScale(grey_img);
    grey_img->sobelX(xgrad_img);
    grey_img->sobelY(ygrad_img);

    // ***** do the Hough voting **********
    model->vote(cur_image, xgrad_img, ygrad_img, search_window, voting_map);
    voting_map->maxLoc(search_window, maxx, maxy);

    // segmentation
    pccm.evaluateColourWithPrior(cur_image, &search_window, false, seg_prior, segmentation);
    cur_seg_change = segmentation->percentageChanged(cur_bb, prev_shift_x, prev_shift_y, seg_prior);
    // set segmentation prior for next iteration
    delete seg_prior;
    seg_prior = segmentation->clone();

    if (current_frame<1)
    {
      mean_seg_uniformity = segmentation->uniformity(cur_bb);
      mean_votingmap_variance = voting_map->varianceFromCentre(search_window)*2.0/(search_window.miWidth+search_window.miHeight);
    }
    else
    {
      mean_seg_uniformity = 0.9*mean_seg_uniformity+ 0.1*segmentation->uniformity(cur_bb);
      mean_votingmap_variance = 0.9*mean_votingmap_variance + 0.1*voting_map->varianceFromCentre(search_window)*2.0/(search_window.miWidth+search_window.miHeight);
    }

    cur_seg_uniformity = segmentation->uniformity(cur_bb);
    cur_votingmap_variance = voting_map->varianceFromCentre(search_window)*2.0/(search_window.miWidth+search_window.miHeight);
    seg_ratio = cur_seg_uniformity/mean_seg_uniformity;
    vot_ratio = cur_votingmap_variance/mean_votingmap_variance;
    MESSAGE(0, "segmentation uniformity: " << cur_seg_uniformity);
    MESSAGE(0, "segmentation change: " << cur_seg_change);
    MESSAGE(0, "voting map variance: " << cur_votingmap_variance);
    MESSAGE(0, "mean segmentation uniformity: " << mean_seg_uniformity);
    MESSAGE(0, "mean voting map variance: " << mean_votingmap_variance);
    MESSAGE(0, "seg ratio: " << seg_ratio);
    MESSAGE(0, "vot ratio: " << vot_ratio);
    //uncertainty = 0;  // used for rigid object tracking (Babenko)
    //uncertainty = 0.5;
    //uncertainty = max(0.2f, min(0.8f, seg_ratio*vot_ratio/(seg_ratio+vot_ratio)));
    uncertainty = max(0.2f, min(0.8f, cur_seg_change));
    MESSAGE(0, "uncertainty: " << uncertainty);

    // opening
    cv::Mat element = cv::getStructuringElement( erosion_type, cv::Size( 2*erosion_w + 1, 2*erosion_h+1 ), cv::Point( erosion_w, erosion_h ) );
    cv::Mat mseg(segmentation->height(), segmentation->width(), CV_32F, segmentation->data(), segmentation->width()*sizeof(float));
    cv::erode( mseg, mseg, element );
    cv::dilate( mseg, mseg, element );

    // backprojection
    model->backproject(cur_image, xgrad_img, ygrad_img, search_window, bp_img, maxx, maxy);

    Rectangle reduced_sw = search_window;
    reduced_sw.enlarge(0.7);
    //segmentation->centreOfMass(search_window, cm_maxx, cm_maxy);
    segmentation->centreOfMass(reduced_sw, cm_maxx, cm_maxy);
    prev_bb = cur_bb;
    cur_bb.setCenter((1.0-uncertainty)*maxx+uncertainty*cm_maxx, (1.0-uncertainty)*maxy+uncertainty*cm_maxy);
    prev_shift_x = cur_bb.centerX()-prev_bb.centerX();
    prev_shift_y = cur_bb.centerY()-prev_bb.centerY();
    
    /*
  larger = cur_bb;
  larger.enlarge(1.2);
  smaller = cur_bb;
  smaller.enlarge(1.0/1.2);
  nbfg1 = segmentation->sumGreaterThanThreshold(larger, 0.5);
  nbfg2 = segmentation->sumGreaterThanThreshold(cur_bb, 0.5);
  nbfg3 = segmentation->sumGreaterThanThreshold(smaller, 0.5);
  nbbg1 = larger.area()-nbfg1;
  nbbg2 = cur_bb.area()-nbfg2;
  nbbg3 = smaller.area()-nbfg3;
  fgbg_r1 = (float)nbfg1/nbbg1;
  fgbg_r2 = (float)nbfg2/nbbg2;
  fgbg_r3 = (float)nbfg3/nbbg3;
  MESSAGE(0, "bb1: " << fgbg_r1);
  MESSAGE(0, "bb2: " << fgbg_r2);
  MESSAGE(0, "bb3: " << fgbg_r3);

  if (fgbg_r1>fgbg_r2*2)
  {
    if (fgbg_r1>fgbg_r3 && cur_bb.area()<5*initial_rect.area())
      cur_bb = larger;
    else if (fgbg_r1<=fgbg_r3 && cur_bb.area()>0.2*initial_rect.area())
      cur_bb = smaller;
  }
  else
  {
    if (fgbg_r2*2<fgbg_r3 && cur_bb.area()>0.2*initial_rect.area())
      cur_bb = smaller;
  }
  */

    update_factor_scale = sqrtf((maxx-cm_maxx)*(maxx-cm_maxx)+(maxy-cm_maxy)*(maxy-cm_maxy))/cur_bb.miWidth;
    search_window = cur_bb;
    search_window.enlarge(par.search_size);

    pccm.update(cur_image, &search_window, segmentation, bp_img, par.detector_update_factor);
    model->update(cur_image, xgrad_img, ygrad_img, search_window, segmentation, par.segmentation_update_factor); 

    // output tracking result to XML/TXT file
    xmlout.sendBB(&cur_bb, 0, 0, 1.0);
    txtout.sendBB(&cur_bb, 0, 0, 1.0);
    xmlout.commit(current_frame, vinput->getCurrentTimestampMs(), vinput->getCurrentTimestampMs());
    txtout.commit(current_frame, vinput->getCurrentTimestampMs(), vinput->getCurrentTimestampMs());
#ifdef VOT_CHALLENGE
    if (par.vot_mode)
    {
      Rectangle original_size_r = cur_bb;
      original_size_r.enlarge(1.0/vot_scale_factor);
      cv::Rect r(original_size_r.miFirstColumn, original_size_r.miFirstLine, original_size_r.miWidth, original_size_r.miHeight);
      vot->outputBoundingBox(r);
    }
#endif

    // display result
    output_window.setCurrentImage(cur_image);

    // draw tracking result
    if (!par.silent || par.output)
    {
      // draw result with read bounding box 
      output_window.draw(cur_bb, Colour(255,0,0), 4);
      output_window.draw(maxx, maxy, Colour(255,0,0), 3);
      output_window.draw(cm_maxx, cm_maxy, Colour(0,0,255), 3);
      output_window.draw(cur_bb.centerX(), cur_bb.centerY(), Colour(0,255,0), 3);
      // draw a red cross with centre position
      output_window.draw(cur_bb.centerX(), cur_bb.centerY(), Colour(255,0,0), 3);
    }

    // output video
    if (par.output)
    {
      voutput->addFrame(cur_image);
    }
    // display tracking result
    if (!par.silent)
    {
      output_window.showImage(cur_image, 2);
      voting_map->multiply(0.01, voting_map_normalised);
      voting_window.showFloatImage(voting_map_normalised, 2);
      bp_img->multiply(1, bp_img_normalised);
      bp_window.showFloatImage(bp_img_normalised, 2);
      segmentation->binarise(0.5);
      seg_window.showFloatImage(segmentation, 2);
      if (par.pause)
	output_window.wait(0);

    }


    voting_map->setZero();
    bp_img->setZero();
    segmentation->setZero();

    // calculate average processing time
    prev_time = current_time;
    current_time = avgfps_timer.getRunTime();

    if (current_frame<=3)
      avgfps = 1000000.0/avgfps_timer.computeDelta(current_time, prev_time);
    else
      avgfps = 0.99*avgfps + 0.01* 1000000.0/avgfps_timer.computeDelta(current_time, prev_time);

    MESSAGE(0, "Average processing speed: " << avgfps << " fps");

    processed_frame++;
  } while (vinput->nextImageAvailable() && current_frame<=par.to_frame);

#ifdef PROFILE
  ProfilerStop();
#endif

  delete segmentation;
  delete voting_map;
  delete voting_map_normalised;
  delete model;

  delete grey_img;
  delete xgrad_img;
  delete ygrad_img;
  delete bp_img;
  delete bp_img_normalised;

  for(int s=0;s<nscales;s++)
    delete lut[s];
  delete [] lut;

  return 0;
}
