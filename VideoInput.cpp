#include "VideoInput.h"

namespace TLInOut
{

VideoInput::VideoInput()
{
  miFramesRead = 0;
  mfFPS=25;
  miImageOffsetX=miImageOffsetY=0;
}

VideoInput::~VideoInput()
{
}

}
