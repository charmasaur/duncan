#include "ncwindow.h"

using namespace client;

NCWindow::NCWindow() : win_(0), width_(0), height_(0), startr_(0), startc_(0) {
}

NCWindow::~NCWindow() {
	if(win_) delwin(win_);
	win_=0;
}

/**
 * Create a new window.
 * \param h height of new window
 * \param w width of new window
 * \param r vertical position of top-left corner of new window
 * \param c horizontal position of top-left corner of new window
 */
void NCWindow::create(unsigned int h, unsigned int w, unsigned int r, unsigned int c) {
	if(win_) delwin(win_);
	width_=w; height_=h; startr_=r; startc_=c;
	win_=newwin(height_,width_,startr_,startc_);
	changed_=true;
}

/**
 * Set whether or not the window has been changed.
 * \param b whether the window has been changed
 */
void NCWindow::set_changed(bool b) {
	changed_=b;
}

/**
 * Clear the window.
 */
void NCWindow::clear() {
	werase(win_);
}

/**
 * Refresh the window.
 */
void NCWindow::refresh() {
	wrefresh(win_);
}

/**
 * Get the values of changed, width, height, startr and startc.
 */
bool NCWindow::changed() {
	return changed_;
}
unsigned int NCWindow::width() {
	return width_;
}
unsigned int NCWindow::height() {
	return height_;
}
unsigned int NCWindow::startr() {
	return startr_;
}
unsigned int NCWindow::startc() {
	return startc_;
}

/**
 * Get a pointer to the ncurses WINDOW structure
 */
WINDOW* NCWindow::win() {
	return win_;
}
