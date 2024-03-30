#ifndef SERVER_NPC_H_
#define SERVER_NPC_H_

#include "../network/network.h"
#include "character.h"
#include <vector>
#include <list>

namespace server {

/*!
 * Represents a non-player character.
 *
 * \author Malcolm Gill
 *
 * \copyright see LICENSE
 */
class NPC : public Character {
	public:
		/// Duplicated with client...!
		typedef std::vector<std::pair<size_t,size_t> > path_t;
	private:
		/// locations through which this NPC will cycle
		path_t route_;

		/// path to next destination on route
		path_t path_;

		/// index in path_ of NPC's most recently visited waypoint
		size_t p_;

		/// index in route_ of NPC's current journey
		size_t r_;

		/// how close (distance squared) this NPC must get to his destination before he decides he's reached it
		const static double kTolerance;

		/// desired row coordinate (or negative when this NPC does not care to have her position snapped)
		double desired_r_;

		/// desired column coordinate (or negative when this NPC does not care)
		double desired_c_;

		void update_dir();

		void desire_pos(double,double);
	public:
		NPC(std::pair<size_t,size_t>,std::pair<size_t,size_t>);
		void update();
		double last_path_send;
		size_t destr();
		size_t destc();
		void set_path(const path_t&);
		void set_path(const std::list<std::pair<size_t,size_t> >&);
		bool should_snap();
		std::pair<double,double> desired_pos();
		void snapped();
};

}

#endif
