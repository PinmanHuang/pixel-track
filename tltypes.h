#ifndef TL_TYPES_H
#define TL_TYPES_H

namespace TLImageProc
{

typedef struct Colour
{
  int r;
  int g;
  int b;
  Colour() { r=0; g=0; b=0; };
  Colour(int _r, int _g, int _b) { r=_r; g=_g; b=_b; };
} Colour;

}

#endif
