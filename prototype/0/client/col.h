#ifndef CLIENT_COL_H_
#define CLIENT_COL_H_
#include <ncurses.h>

namespace client {

/*!
 * ncurses colour constants
 * \author Malcolm Gill
 *
 * \copyright see LICENSE
*/
namespace col {
	const int kWall = '#';
	const int kGrass = '.';
	const int kCharacter = 3;
	void init();
}
}

#endif
