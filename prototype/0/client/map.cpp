#include "map.h"

using namespace client;

/**
 * Draw part of the map.
 * \param wnd window in which to draw the map
 * \param row top row to be drawn
 * \param col leftmost column to be drawn
 */
void DrawableMap::draw(NCWindow& wnd, int row, int col) {
	int ubr=std::min(row+(int)wnd.height(),(int)height_), ubc=std::min(col+(int)wnd.width(),(int)width_);
	for(int r=std::max(0,row); r<ubr; r++)
		for(int c=std::max(0,col); c<ubc; c++) {
			wattron(wnd.win(),COLOR_PAIR(tile(r,c)));
			mvwaddch(wnd.win(),r-row,c-col,tile(r,c));
		}
}
