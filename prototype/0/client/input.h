#ifndef CLIENT_INPUT_H_
#define CLIENT_INPUT_H_

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <tr1/memory>
#include <iostream>
#include <stdexcept>
#include <ncurses.h>
#include "inputlistener.h"

namespace client {

/*!
 * Get input (key presses and releases) from X, and notify an InputListener 
 * object when such events occur.
 *
 * \author Harry Slatyer
 *
 * \copyright see LICENSE
 */
class Input {
	private:
		std::tr1::shared_ptr<InputListener> listener_;
		Display* display_;
		XEvent event_;
		
		bool grabbed_, repeat_;

		char caps_, shift_;
		unsigned int keycode_, keysym_, repeat_code_;

		void grab();
		void ungrab();
	public:
		Input(std::tr1::shared_ptr<InputListener> l=std::tr1::shared_ptr<InputListener>());
		~Input();

		void set_listener(std::tr1::shared_ptr<InputListener>);
		void update();

		static unsigned int kLeft, kRight, kUp, kDown, kEnter, kSpace, kBackspace, kCaps, kLShift, kRShift, kEscape, kF1;
};

} // namespace

#endif
