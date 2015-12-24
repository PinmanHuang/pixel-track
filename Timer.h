#ifndef TL_TIMER_H
#define TL_TIMER_H


namespace TLUtil {

class Timer 
{
	long long mStartTime;

public:
	/// hours
	int hours;
	/// minutes
	int minutes;
	/// seconds
	int seconds;
	/// milliseconds
	int mseconds;
	/// microseconds
	int useconds;
	/// nanoseconds
	int nseconds;
	/// create the timer
	Timer();

	~Timer();

	/** reset the timer 
	
	    The timer will count time starting from now, and the accumulated time is erased.
	*/
	void reset();

	/// stop the timer and return the total accumulated time
	long long stop();

	/// return the total accumulated time
	long long getRunTime();

	/// compute the time difference
	long long computeDelta(long long current_time_, long long previous_time_);
};

}

#endif
