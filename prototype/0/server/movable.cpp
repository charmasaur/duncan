#include "movable.h"
using namespace server;
Movable::Movable() : dir_(0), speed_(0) {}

/**
 * Set the direction.
 * \param d direction
 */
void Movable::set_dir(net::dir_t d) {
	dir_=d;
}

/**
 * Set the speed.
 * \param s speed
 */
void Movable::set_speed(double s) {
	speed_=s;
}

/**
 * Get the vertical (row) velocity.
 * \returns vertical velocity
 */
double Movable::rspeed() {
	return speed_*(((dir_&net::direction::up)?-1.0:0.0)+((dir_&net::direction::down)?1.0:0.0));
}

/**
 * Get the horizontal (column) velocity.
 * \returns horizontal velocity
 */
double Movable::cspeed() {
	return speed_*(((dir_&net::direction::left)?-1.0:0.0)+((dir_&net::direction::right)?1.0:0.0));
}

/**
 * \returns true iff this Player is moving.
 */
bool Movable::is_moving() {
	return dir_;
}

/**
 * just in case we ever want to delete a pointer to a Movable...
 */
Movable::~Movable() {}
