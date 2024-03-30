#ifndef TIMER_TIMER_H_
#define TIMER_TIMER_H_

#include <sys/time.h>

namespace timer {

/*!
 * A wrapper around the gettimeofday function.
 *
 * \author Harry Slatyer
 *
 * \copyright see LICENSE
 */
class Timer {
	private:
		unsigned int microseconds_;
		timeval last_time_[2];

		void get_time(unsigned char);
	public:
		Timer(unsigned int micros=1000);

		unsigned int elapsed();
		void reset();
};

}

#endif
