#include "character.h"

using namespace client;

Character::Character() : row_(0), col_(0) {
}

/**
 * Set the position of the character.
 * \param r row
 * \param c column
 */
void Character::set_pos(size_t r, size_t c) {
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
size_t Character::row() {
	return row_;
}
size_t Character::col() {
	return col_;
}
const std::string& Character::name() {
	return name_;
}
