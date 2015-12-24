#include <string>

/*
#include "../defined_constants.h"

//#define DUMP_DEFINES for(int _ddi=0; _ddi<sizeof(defined_constants)/sizeof(defined_constants[0]); _ddi++) cout << defined_constants[_ddi] << endl; 
#define DUMP_DEFINES(log_level) do{ for(int _ddi=0; _ddi<sizeof(defined_constants)/sizeof(defined_constants[0]); _ddi++) MESSAGE(log_level,defined_constants[_ddi]); } while(0)
*/


namespace TLUtil
{

/*
float sqrt_approx(float z)
float InvSqrt(float x)  
float InvSqrt( float f )
*/


#define ROUND_TO_INT(x) ( ((x)>0)? (int((x)+.5)) : (int((x)-.5)) )
#define ROUND_TO_INTP(x) (int((x)+.5))
#define ROUND_TO_INTN(x) (int((x)-.5))

  std::string getExePath(char* arg0=NULL);

}
