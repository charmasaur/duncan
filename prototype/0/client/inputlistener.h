#ifndef CLIENT_INPUTLISTENER_H_
#define CLIENT_INPUTLISTENER_H_

namespace client {

/*!
 * Interface for handling keyboard input.
 * 
 * \author Harry Slatyer
 *
 * \copyright see LICENSE
 */
class InputListener {
	public:
		virtual ~InputListener() {}
		/**
		 * Called when a key is pressed.
		 * \param key keysym of pressed key
		 * \param repeat whether this press was due to auto-repeat
		 */
		virtual void on_press(unsigned int key, bool repeat) {}
		/**
		 * Called when a key is released.
		 * \param key keysym of released key
		 * \param repeat whether this release was due to auto-repeat
		 */
		virtual void on_release(unsigned int key, bool repeat) {}
};

} // namespace

#endif
