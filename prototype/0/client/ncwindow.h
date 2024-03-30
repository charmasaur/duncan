#ifndef CLIENT_NCWINDOW_H_
#define CLIENT_NCWINDOW_H_

#include <ncurses.h>

namespace client {

/*!
 * Store information about an ncurses window. 
 *
 * \author Harry Slatyer
 *
 * \copyright see LICENSE
 */
class NCWindow {
	private:
		WINDOW* win_;
		unsigned int width_,height_,startr_,startc_;
		bool changed_;
	public:
		NCWindow();
		~NCWindow();

		void create(unsigned int, unsigned int, unsigned int, unsigned int);
		void set_changed(bool);
		bool changed();
		unsigned int width();
		unsigned int height();
		unsigned int startr();
		unsigned int startc();
		void clear();
		void refresh();
		WINDOW* win();
};

} // namespace

#endif
