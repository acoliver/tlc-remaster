#include <allegro.h>
#include "Timer.h"

// macOS uses POSIX APIs
#if defined(__APPLE__) || defined(__linux__)
#include <sys/time.h>
#endif

Timer::Timer(void)
{
	#if defined(__APPLE__) || defined(__linux__) || defined(_POSIX_SOURCE)
	gettimeofday(&initial, NULL);
	#endif

	reset();
}

Timer::~Timer(void){}

long Timer::getTimer()
{
	#if defined(_WIN32) || defined(_WIN64)
	return (long) clock();

	#elif defined(__APPLE__) || defined(__linux__) || defined(_POSIX_SOURCE)

	timeval current, delta;
	gettimeofday(&current, NULL);
	timersub(&current, &initial, &delta);
	return (long) (delta.tv_sec*1000 + delta.tv_usec/1000);

	#else
		#error Could not determine the function to get wall-clock time

	#endif
}

void Timer::setTimer(long value)
{
	timer_start = value;
}


long Timer::getStartTimeMillis()
{
	return getTimer() - timer_start;
}

//warning: this is a blocking sleep
void Timer::sleep(long ms)
{
	long start = getTimer();
	while (start + ms > getTimer());
}

void Timer::reset()
{
	timer_start = getTimer();
	stopwatch_start = timer_start;
}

bool Timer::stopwatch(long ms)
{
	if ( getTimer() > stopwatch_start + ms ) {
		stopwatch_start = getTimer();
		return true;
	}
	else return false;
}

