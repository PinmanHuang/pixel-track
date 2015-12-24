#ifndef TL_DRAW_H
#define TL_DRAW_H

#include "Image.h"
#include "Rectangle.h"
#include "tltypes.h"


namespace TLImageProc
{

void drawRectangle(Image8U* img, Rectangle r, Colour c, int Thickness=1);
void drawRectangle(Image8U* img, Rectangle r, double rotation, Colour c, int Thickness=1);
void drawCross(Image8U* img, int x, int y, Colour c, int Thickness=1);
void drawLine(Image8U* img, int x0, int y0, int x1, int y1, Colour c, int Thickness=1);

}

#endif
