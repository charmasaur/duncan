#ifndef CLIENT_TARTSTATE_H_
#define CLIENT_TARTSTATE_H_

#include <ncurses.h>
#include <string>
#include "inputline.h"
#include "state.h"
#include "input.h"
#include "gamestate.h"

namespace client {

/*!
 * Base state from which the game runs.
 * 
 * Get the player and server details.
 * 
 * \author Harry Slatyer
 *
 * \copyright see LICENSE
 */
class StartState : public State {
	private:
		State::Action state_;
		InputLine name_, ip_;
		bool changed_, getip_;
		std::tr1::shared_ptr<GameState> child_;

		void on_press(unsigned int,bool);
	public:
		StartState();

		std::tr1::shared_ptr<State> child();
		void resize(unsigned int,unsigned int);
		void on_push();
		void on_active();
		State::Action update();
		void draw();
};

} // namespace

#endif
