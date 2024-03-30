#include "timer.h"

using namespace timer;

/**
 * Initialise the timer.
 * \param micros number of microseconds in a 'unit' of time
 */
Timer::Timer(unsigned int micros) : microseconds_(micros) {
	reset();
}

/**
 * Update either the current time or the last reset time.
 * \param wh 0 to update the reset time, 1 to update the current time
 */
void Timer::get_time(unsigned char wh) {
	gettimeofday(last_time_+wh,0);
}

/**
 * Find the elapsed 'units' of time.
 * \returns units of time elapsed since last reset
 */
unsigned int Timer::elapsed() {
	get_time(1);
	return ((last_time_[1].tv_sec-last_time_[0].tv_sec)*1000000+(last_time_[1].tv_usec-last_time_[0].tv_usec))/microseconds_;
}

/**
 * Reset the timer.
 */
void Timer::reset() {
	get_time(0);
}
