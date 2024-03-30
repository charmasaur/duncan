#ifndef CLIENT_STATE_H_
#define CLIENT_STATE_H_

#include <tr1/memory>
#include "input.h"

namespace client {

/*!
 * Interface for a game state.
 * 
 * \author Harry Slatyer
 *
 * \copyright see LICENSE
 */
class State : public InputListener {
	protected:
		virtual void key_pressed(unsigned int wh) {}
		virtual void key_released(unsigned int wh) {}
	public:
		/**
		 * Action for the Client object to perform on this state.
		 */
		enum Action {NOTHING, POP_ME, PUSH_CHILD};

		virtual ~State() {}
		/**
		 * Get the child state (to be pushed on top of this).
		 * \returns child state
		 */
		virtual std::tr1::shared_ptr<State> child() { return std::tr1::shared_ptr<State>(); }

		/**
		 * Called when this state is pushed.
		 */
		virtual void on_push() {}

		/**
		 * Called when this state is popped.
		 */
		virtual void on_pop() {}

		/**
		 * Called when the state on top of this is popped.
		 */
		virtual void on_active() {}

		/**
		 * Called when the window is resized.
		 */
		virtual void resize(unsigned int,unsigned int) {}

		/**
		 * Update this state.
		 * \returns action to be performed on this state
		 */
		virtual Action update() { return NOTHING; }

		/**
		 * Draw this state.
		 */
		virtual void draw() {}
};

} // namespace

#endif
