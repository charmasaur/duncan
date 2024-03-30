#ifndef CLIENT_CLIENT_H_
#define CLIENT_CLIENT_H_

#include <cstdlib>  // for exit
#include <iostream> // for errors
#include <stack>
#include <ncurses.h>
#include "input.h"
#include "state.h"
#include "startstate.h"
#include "col.h"

namespace client {

/*!
 * Handle a stack of possible game 'states', and create a base state
 * from which the game runs.
 *
 * \author Harry Slatyer
 *
 * \copyright see LICENSE
*/
class Client {
	private:
		std::stack<std::tr1::shared_ptr<State> > state_stack_;

		unsigned int rows_, cols_;

		Input input_;
			
		void pop();
		void push(std::tr1::shared_ptr<State>);
	public:
		Client();
		~Client();

		bool update();
};

} // namespace

#endif
