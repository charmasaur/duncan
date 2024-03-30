#ifndef CLIENT_MAP_H_
#define CLIENT_MAP_H_

#include "../map/map.h"
#include "ncwindow.h"
#include "col.h"

namespace client {

/*!
 * Store information about a map, and draw it in a specified ncurses window.
 * \author Harry Slatyer
 * \author Malcolm Gill
 *
 * \copyright see LICENSE
 */
class DrawableMap : public map::Map {
	public:
		void draw(NCWindow&, int, int);
};

} // namespace

#endif
