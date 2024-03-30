#ifndef SERVER_PATHWORLD_H_
#define SERVER_PATHWORLD_H_

#include <iostream>
#include "character.h"

namespace server {

/*!
 * Interface for a world which can do path-finding.
 * 
 * \author Harry Slatyer
 *
 * \copyright see LICENSE
 */
class PathWorld {
	public:
		virtual ~PathWorld() {}
		/**
		 * Get the width of the world's map.
		 * \returns map width
		 */
		virtual size_t width() const {std::cout << "pw width\n"; return 0;}
		/**
		 * Get the height of the world's map.
		 * \returns map height
		 */
		virtual size_t height() const {std::cout << "pw height\n"; return 0;}
		/**
		 * Determine whether a given tile is vacantoccupied.
		 * \param r row of tile
		 * \param c col of tile
		 * \param ignore character to ingore
		 * \returns true iff the tile is vacant or occupied only by the ignored character
		 */
		virtual bool is_vacant(double r, double c, Character* ignore) const {std::cout << "pw vacant\n"; return true;}
};

} // namespace

#endif
