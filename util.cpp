#include <cstring>
#include <unistd.h>
#include "util.h"

using namespace std;

namespace TLUtil
{

/*
float sqrt_approx(float z)
{
    union
    {
        int tmp;
        float f;
    } u;
    u.f = z;
 
    u.tmp = (1 << 29) + (u.tmp >> 1) - (1 << 22) + a;
 
    return u.f;
}


float InvSqrt(float x)  // depending on 32 bit IEEE implementation 
{
    float xhalf = 0.5f*x;
    int i = *(int*)&x;
    i = 0x5f37a86 - (i >> 1);
    x = *(float*)&i;
    x = x*(1.5f - xhalf*x*x);
    return x;
}



float InvSqrt( float f )
{
  float fhalf = 0.5f*f;
  asm
  (
    "mov %1, %%eax;"
    "sar %%eax;"
    "mov $0x5F3759DF, %%ebx;"
    "sub %%eax, %%ebx;"
    "mov %%ebx, %0"
    :"=g"(f)
    :"g"(f)
    : "%eax", "%ebx"
  );
  f *= 1.5f - fhalf*f*f;
  return f;
}
*/

string getExePath(char* arg0/*=NULL*/)
{
  int bufsize=512;
  char buf[512];
  char* slash;
  ssize_t len;

#ifdef WIN32
  // Will contain exe path
  HMODULE hModule = GetModuleHandle(NULL);
  if (hModule != NULL)
  {
    // When passing NULL to GetModuleHandle, it returns handle of exe itself
    GetModuleFileName(hModule,buf,(sizeof(buf)));

    return string(buf);
  }
#else // Linux
  if (arg0!=NULL)
    strcpy(buf, arg0);
  else
  {
    len = readlink("/proc/self/exe", buf, bufsize);
    if (len != -1)
      buf[len] = '\0';
    else
      return ".";
  }
  slash = strrchr(buf, '/');
  if (slash)
  {
    *slash = '\0';
    return string(buf);
  }
#endif
  else
    return ".";
}



}

