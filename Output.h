#ifndef TL_OUTPUT_H
#define TL_OUTPUT_H

//#include "Parameters.h"

namespace TLImageProc{
class Rectangle;
}


char *get_time_string(int mode24, int *hh, int *mm, int *ss);


namespace TLInOut
{

//class ip_BoundingBox; // forward declaration

typedef struct faceinfo_
{
  int id;			// person/track ID
  int tl_x, tl_y, br_x, br_y;	// for straight bounding boxes or rotated CFA rectangles
  int tr_x, tr_y, bl_x, bl_y;	// for rotated CFA rectangles
  float pan;			// head pan angle [-90,+90] degrees
  float confidence;		// confidence value [0, 1.0]
} faceinfo_t;

enum OutputType { OutputFile=1 };
int string2Output(char* str, int& result, int& nb_outputs);

class Output
{
  public:
    float mfWidthScale;
    float mfHeightScale;

    Output();
    virtual ~Output() {};

    virtual int sendBB(TLImageProc::Rectangle* bb, int identifier, float pan, float confidence)=0;
    virtual int sendBB(int tl_x, int tl_y, int tr_x, int tr_y, int bl_x, int bl_y, int br_x, int br_y, int identifier, float pan, float confidence) { return 0; };
    virtual int commit(int iter, unsigned long long timestamp, long long vce_timestamp)=0;
    void setCoordinateScale(float ws, float hs) { mfWidthScale=ws; mfHeightScale=hs; };
};



}

#endif

