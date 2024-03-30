#include "input.h"

using namespace client;

/**
 * X keysyms for common special keys
 */
unsigned int Input::kLeft=XStringToKeysym("Left");
unsigned int Input::kRight=XStringToKeysym("Right");
unsigned int Input::kUp=XStringToKeysym("Up");
unsigned int Input::kDown=XStringToKeysym("Down");
unsigned int Input::kEnter=XStringToKeysym("Return");
unsigned int Input::kSpace=XStringToKeysym("space");
unsigned int Input::kBackspace=XStringToKeysym("BackSpace");
unsigned int Input::kCaps=XStringToKeysym("Caps_Lock");
unsigned int Input::kLShift=XStringToKeysym("Shift_L");
unsigned int Input::kRShift=XStringToKeysym("Shift_R");
unsigned int Input::kEscape=XStringToKeysym("Escape");
unsigned int Input::kF1=XStringToKeysym("F1");

/**
 * Set the listener, open the X display and grab the keyboard.
 * \param l pointer to listener object
 * \throw std::runtime_error if initialisation fails
 */ 
Input::Input(std::tr1::shared_ptr<InputListener> l) : grabbed_(false), repeat_(false), caps_(0), shift_(0), keycode_(0), keysym_(0), repeat_code_(0) {
	set_listener(l);
	display_=XOpenDisplay(0);
	if(display_==0) {
		throw std::runtime_error("Could not initialise X display.");
	}
	int caps;
	XkbGetNamedIndicator(display_,XInternAtom(display_,"Caps Lock",false),0,&caps,0,0);
	if(caps) caps_=true;
	grab();
}

/**
 * Ungrab the keyboard and close the X display.
 */
Input::~Input() {
	if(grabbed_) ungrab();
	if(XCloseDisplay(display_)) {
		throw std::runtime_error("Could not close X display.");
	}
}

/**
 * Grab the keyboard
 */
void Input::grab() {
	grabbed_=XGrabKeyboard(display_,DefaultRootWindow(display_),true,GrabModeAsync,GrabModeAsync,CurrentTime)==GrabSuccess;
}

/**
 * Ungrab the keyboard
 */
void Input::ungrab() {
	XUngrabKeyboard(display_,CurrentTime);
	grabbed_=false;
}

/** 
 * Set the listener.
 * \param l pointer to the listener object
*/
void Input::set_listener(std::tr1::shared_ptr<InputListener> l) {
	listener_=l;
}

/**
 * Handle any key press/release events.
 */
void Input::update() {
	if(grabbed_) {
		while(XPending(display_)) {
			XNextEvent(display_,&event_);
			switch(event_.type) {
				case KeyPress:
					//if(!grabbed_) break;
					keycode_=((XKeyPressedEvent*)&event_)->keycode;
					keysym_=XKeycodeToKeysym(display_,keycode_,0);
					if(keysym_==kCaps) caps_^=1;
					if(keysym_==kLShift || keysym_==kRShift) shift_=1;
					if(keysym_==kF1) {
						ungrab();
					//	XGrabKey(display_,keycode_,0,DefaultRootWindow(display_),true,GrabModeAsync,GrabModeAsync);
					}	
					if(listener_) listener_->on_press(XKeycodeToKeysym(display_,keycode_,(shift_+caps_)&1),repeat_ && keycode_==repeat_code_);
					break;
				case KeyRelease:
					/*if(!grabbed_) {
						grab();
						break;
					}*/
					keycode_=((XKeyPressedEvent*)&event_)->keycode;
					keysym_=XKeycodeToKeysym(display_,keycode_,0);
					if(keysym_==kLShift || keysym_==kRShift) shift_=0;
					if(listener_) {
						repeat_=false;
						if(XPending(display_)) {
							XPeekEvent(display_,&event_);
							if(event_.type==KeyPress && keycode_==((XKeyPressedEvent*)&event_)->keycode) { repeat_=true; repeat_code_=keycode_; }
							else repeat_=false;
						}
						listener_->on_release(XKeycodeToKeysym(display_,keycode_,(shift_+caps_)&1),repeat_);
					}
					break;
			}
		}
	}
	else if(getch()>=0) grab();
}
