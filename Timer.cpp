#include "Timer.h"
#include <sys/time.h>
#include <stdlib.h>

namespace TLUtil {

Timer::Timer()
{
	hours = 0;    // hours
	minutes = 0;  // minutes
	seconds = 0;  // seconds
	mseconds = 0; // milliseconds
	useconds = 0; // microseconds
	nseconds = 0; // nanoseconds
}

void Timer::reset()
{
	mStartTime = getRunTime();
}

long long Timer::stop()
{
  	long long current_time;

  	current_time = getRunTime();

	return computeDelta(current_time, mStartTime);
}

long long Timer::computeDelta(long long current_time_, long long previous_time_)
{
   	long long delta;

  	delta = current_time_ - previous_time_;
 
	long long hours_, minutes_, seconds_, mseconds_, useconds_;

	useconds_ = delta;
	mseconds_ = useconds_ / 1000;
	useconds_ -= mseconds_ * 1000;
	seconds_ = mseconds_ / 1000;
	mseconds_ -= seconds_ * 1000;
	minutes_ = seconds_ / 60;
	seconds_ -= minutes_ * 60;
	hours_ = minutes_ / 60;		
	minutes_ -= hours_ * 60;

	hours    = (int) hours_;
	minutes  = (int) minutes_;
	seconds  = (int) seconds_;
	mseconds = (int) mseconds_;
	useconds = (int) useconds_;
	nseconds = 0;

	return delta;
}

/// returns timestamp in microseconds
long long Timer::getRunTime()
{
   	timeval tv;
	long long ts;
	
    	gettimeofday(&tv, NULL);
	
    	ts  = tv.tv_sec;
    	ts *= 1000000;
    	ts += tv.tv_usec;

	return ts;
}

Timer::~Timer()
{
}

}
