#include "client.h"

using namespace client;

/**
 * Create and push the base state, and initialise curses.
*/
Client::Client() : rows_(0), cols_(0) {
	initscr();
	noecho();
	cbreak();
	getmaxyx(stdscr,rows_,cols_);
	keypad(stdscr,true);
	timeout(0);
	if(has_colors()) {
		start_color();
		col::init();
	} else {
		throw std::runtime_error("non-colourful terminals are not supported");
	}
	push(std::tr1::shared_ptr<State>(new StartState()));
}

/**
 * Shut down curses.
 */
Client::~Client() {
	endwin();
}

/**
 * Pop the top state.
*/
void Client::pop() {
	state_stack_.top()->on_pop();
	state_stack_.pop();
	if(!state_stack_.empty()) {
		state_stack_.top()->on_active();
		input_.set_listener(state_stack_.top());
	}
	else input_.set_listener(std::tr1::shared_ptr<State>());
}

/**
 * Push a new state.
 * \param state pointer to state to push
*/
void Client::push(std::tr1::shared_ptr<State> state) {
	if(!state) return;
	input_.set_listener(state);
	state_stack_.push(state);
	state_stack_.top()->on_push();
}

/**
 * Update and draw the top state, and pop or push states if necessary.
 * \return whether or not the program should continue to run
*/
bool Client::update() {
	if(state_stack_.empty()) return false;
	if(is_term_resized(rows_,cols_)) {
		getmaxyx(stdscr,rows_,cols_);
		state_stack_.top()->resize(rows_,cols_);
	}
	State::Action action = state_stack_.top()->update();
	state_stack_.top()->draw();
	if(action==State::POP_ME) pop();
	else if(action==State::PUSH_CHILD) push(state_stack_.top()->child());
	input_.update();
	return true;
}
