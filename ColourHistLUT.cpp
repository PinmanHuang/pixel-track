#include <fstream>
#include "ColourHistLUT.h"

namespace TLImageProc
{

ColourHistLUT::ColourHistLUT(int nbbins)
{
  miNBins = nbbins;
  bgr_to_colour_bin = new unsigned short int[nbbins];
  mbSeparate3rdChannel=true;
}

ColourHistLUT::~ColourHistLUT()
{
  delete[] bgr_to_colour_bin;
}


void ColourHistLUT::save(std::string filename)
{
  std::ofstream fout(filename.c_str(), std::ios::out | std::ios::binary);
  if (!fout)
    return;

  fout.write((char*)bgr_to_colour_bin, miNBins*sizeof(unsigned short int));
}

bool ColourHistLUT::read(std::string filename)
{
  std::ifstream fin(filename.c_str(), std::ios::in | std::ios::binary);
  if (!fin)
    return false;

  fin.read((char*)bgr_to_colour_bin, miNBins*sizeof(unsigned short int));
  return true;
}

}
