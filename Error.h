#ifndef TL_ERROR_H
#define TL_ERROR_H

#include <stdexcept>
#include <iostream>
#include <cstdlib>


namespace TLUtil
{
#define DEBUG_VERBOSITY 2

// Debug
#ifndef DEBUG_VERBOSITY
#define DEBUG_VERBOSITY 0
  // 0 : high-level important information
  // 1 : intermediate run-time information
  // 2 : more detailed debugging information
  // 3 : debugging details (used for temporary debugging)
#endif

  /*
#ifdef NDEBUG
  #define DEBUG(LEVEL,MESSAGE)
#else
*/
  #define MESSAGE(LEVEL,MESSAGE) if(LEVEL <= DEBUG_VERBOSITY)  std::cout << MESSAGE << std::endl;
  #define MESSAGE_(LEVEL,MESSAGE) if(LEVEL <= DEBUG_VERBOSITY)  std::cout << MESSAGE;
//#endif


// Assert
#ifdef NDEBUG
  // release version
  #define ASSERT(condition,message) { \
      if (! (condition)) { \
        std::cerr << "Assertion failed: " << #condition << message << std::endl; \
      } \
  }
#else
  // debug version
  #define ASSERT(condition,message) { \
      if (! (condition)) { \
        std::cerr << "Assertion failed: " << #condition << message << std::endl; \
        exit(EXIT_FAILURE); \
      } \
  }
#endif




class FileOpen_Ex : public std::runtime_error
{
  public:
    FileOpen_Ex(std::string& message);
};

class CameraOpen_Ex : public std::runtime_error
{
  public:
    CameraOpen_Ex(std::string& id);
};


class OptionNotFound_Ex : public std::runtime_error
{
  public:
    OptionNotFound_Ex(std::string& message);
};
  




}

#endif
