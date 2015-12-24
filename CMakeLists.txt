cmake_minimum_required (VERSION 2.6)
#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)
project(pixeltrack)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_conf)


FIND_PACKAGE(OpenCV REQUIRED )
FIND_PACKAGE(FFMPEG REQUIRED)
#SET(CMAKE_CXX_FLAGS "-fpermissive")
SET(CMAKE_CXX_FLAGS "-fpermissive -DVOT_CHALLENGE")

include_directories(/home/stefan/code/trackinglib ${FFMPEG_INCLUDE_DIR})

file(GLOB SRCS "*.cpp")
#add_executable(pixeltrack pixeltrack.cpp HSVPixelModel.cpp BGR2HSVdistLUT.cpp HSVPixelGradientModel.cpp GradDispLUT.cpp PixelClassColourModel.cpp GradientModel.cpp)
add_executable(pixeltrack pixeltrack.cpp ${SRCS})


target_link_libraries(pixeltrack
  #/home/stefan/code/trackinglib/build/inout/libinout.a 
  #/home/stefan/code/trackinglib/build/imageproc/libimageproc.a 
  #/home/stefan/code/trackinglib/build/util/libutil.a  
  ${OpenCV_LIBS} ${FFMPEG_LIBRARIES})


