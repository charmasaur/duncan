#include "character.h"

using namespace server;

Character::Character() : row_(0.0), col_(0.0) {
}

Character::~Character() {
}

/**
 * Set the position of the character.
 * \param r row
 * \param c column
 */
void Character::set_pos(double r, double c) {
	row_=r; col_=c;
}

/**
 * Set the character's name.
 * \param name character's name
 */
void Character::set_name(const std::string& name) {
	name_=name;
}

/**
 * Get the character's row, column or name.
 */
double Character::row() {
	return row_;
}
double Character::col() {
	return col_;
}
const std::string& Character::name() {
	return name_;
}

/**
 * \returns true iff this Character can be called upon for pathfinding,
 *          which is the case for Players but not for NPCs.
 * \note please only return true if you are a Player please
 */
bool Character::can_find_path() {
	return false;
}
