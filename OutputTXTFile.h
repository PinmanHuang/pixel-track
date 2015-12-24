#ifndef TL_OUTPUTTXTFILE_H
#define TL_OUTPUTTXTFILE_H

#include "Rectangle.h"
#include "Output.h"


namespace TLInOut
{

  class OutputTXTFile : public Output
  {
    public:
      char* m_cFileName;
      FILE* m_File;
      int m_iLastNbPersons;
      int m_iCurrentNbPersons;
      int m_iLastIter;
      //int m_iLogLevel;
      faceinfo_t faceinfo[200];

      OutputTXTFile(const char* filename, int skip_frames=0); //, int loglevel);
      ~OutputTXTFile();

      int sendBB(TLImageProc::Rectangle* bb, int identifier, float pan, float confidence);
      int sendBB(int tl_x, int tl_y, int tr_x, int tr_y, int bl_x, int bl_y, int br_x, int br_y, int identifier, float pan, float confidence);
      int commit(int iter, unsigned long long timestamp, long long vce_timestamp);
 
  };
}

#endif
