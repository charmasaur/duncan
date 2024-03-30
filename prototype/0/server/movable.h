#ifndef SERVER_MOVABLE_H_
#define SERVER_MOVABLE_H_

#include "../network/network.h"

namespace server {

/*!
 * Mix-in class for objects that can have velocity.
 *
 * \author Harry Slatyer
 * \author Malcolm Gill
 *
 * \copyright see LICENSE
 */
class Movable {
	private:
		net::dir_t dir_;
		double speed_;
	public:
		Movable();
		void set_dir(net::dir_t);
		void set_speed(double);
		double rspeed();
		double cspeed();
		bool is_moving();
		virtual ~Movable();
};

}

#endif
