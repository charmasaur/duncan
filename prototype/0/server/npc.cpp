#include "npc.h"
using namespace server;

const double NPC::kTolerance = net::kTolerance/2;

/**
 * Set the two route waypoints for this NPC.
 * \param start initial waypoint
 * \param end final waypoint
 */
NPC::NPC(std::pair<size_t,size_t> start, std::pair<size_t,size_t> end) : p_(0), r_(0), desired_r_(-1), desired_c_(-1), last_path_send(0.0) {
	desire_pos(start.first,start.second);
	set_speed(10.0);
	route_.push_back(end);
	route_.push_back(start);
}

/**
 * Makes this NPC follow his path.
 * Must be called frequently enough to prevent overshooting by >kTolerance,
 * but as long as NPCs are quite slow, this shouldn't be very demanding...
 */
void NPC::update() {
	if(p_+1 >= path_.size()) {
		// nowhere to go :(
		return;
	}

	// calculate distance remaining to next step in the path
	double Dr = row()-path_[p_+1].first;
	double Dc = col()-path_[p_+1].second;
	double dist = Dr*Dr+Dc*Dc;

	if(dist <= kTolerance) {
		// reached next node in path
		p_++;

		// set position exactly (avoids horrible failure)
		desire_pos(path_[p_].first,path_[p_].second);

		if(p_+1 == path_.size()) {
			// end of path; go to next step in route
			r_ = (r_+1)%route_.size();
			set_dir(0);
		} else {
			// move on to the next step in the path
			update_dir();
		}
	}
}

/**
 * \return row to which this NPC currently wants to get
 */
size_t NPC::destr() {
	return route_[r_].first;
}

/**
 * \return column to which this NPC currently wants to get
 */
size_t NPC::destc() {
	return route_[r_].second;
}

/**
 * Tells this NPC to start following the given path.
 */
void NPC::set_path(const path_t& path) {
	if(path.empty()) return;
	// this could be done more efficiently, if that would really be beneficial...
	path_ = path;
	p_ = 0;
	update_dir();
}

/**
 * Tells this NPC to start following the given path.
 */
void NPC::set_path(const std::list<std::pair<size_t,size_t> >& path) {
	if(path.empty()) return;
	path_=path_t(path.begin(),path.end());
	p_=0;
	update_dir();
}

/**
 * Calculates the direction this NPC ought to move to get to the next point on his path.
 * Assumes that the NPC is from Manhattan.
 */
void NPC::update_dir() {
	size_t r1 = path_[p_].first, c1 = path_[p_].second, r2 = path_[p_+1].first, c2 = path_[p_+1].second;
	set_dir(0);
	if(r1==r2) {
		if(c1<c2) {
			set_dir(net::direction::right);
		} else if(c2<c1) {
			set_dir(net::direction::left);
		}
	} else if(c1==c2) {
		if(r1<r2) {
			set_dir(net::direction::down);
		} else if(r2<r1) {
			set_dir(net::direction::up);
		}
	}
}

/**
 * \returns true iff this npc wants his position to be set specially by the server
 */
bool NPC::should_snap() {
	return desired_r_>0 || desired_c_>0;
}

/**
 * \returns desired position coordinates (r,c) for this NPC, if should_snap().
 */
std::pair<double,double> NPC::desired_pos() {
	return std::pair<double,double>(desired_r_,desired_c_);
}

/**
 * Please call this after setting this NPC's position to desired_pos() please.
 */
void NPC::snapped() {
	desired_r_=desired_c_=-1;
}

/**
 * Sets the desired position for this NPC.
 * \param r row coordinate
 * \param c column coordinate
 */
void NPC::desire_pos(double r,double c) {
	desired_r_=r;
	desired_c_=c;
}
