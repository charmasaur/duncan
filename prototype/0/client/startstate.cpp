#include "startstate.h"

using namespace client;

/**
 * Initialise members.
 */
StartState::StartState() : state_(State::NOTHING), name_(16,16), ip_(16,16), changed_(true), getip_(false), child_(new GameState()) {
}

std::tr1::shared_ptr<State> StartState::child() {
	return child_;
}

/**
 * Send the pressed key to the InputLine storing either the name or the ip address.
 * \param wh the pressed key
 * \param repeat whether the keypress was due to auto-repeat
 */
void StartState::on_press(unsigned int wh, bool repeat) {
	if(wh==Input::kEscape) state_=State::POP_ME;
	else if(!getip_ && name_.new_char(wh) && name_.input().length()>0) {
		child_->set_name(name_.input());
		getip_=true;
	}
	else if(getip_ && ip_.new_char(wh)) {
		child_->set_ip(ip_.print());
		state_=State::PUSH_CHILD;
	}
	changed_=true;
}

/**
 * Handle a window resize.
 * \param r number of rows
 * \param c number of columns
 */
void StartState::resize(unsigned int r, unsigned int c) {
	child_->set_dim(r,c);
	changed_=true;
}

/**
 * Set the action to default.
 */
void StartState::on_push() {
	unsigned int r, c;
	getmaxyx(stdscr,r,c);
	child_->set_dim(r,c);
	state_=State::NOTHING;
	changed_=true;
}

/**
 * Set action to default.
 */
void StartState::on_active() {
	state_=State::NOTHING;
	changed_=true;
}

State::Action StartState::update() {
	return state_;
}

/**
 * Draw a friendly message prompting the user for her name, along
 * with the current name.
 */
void StartState::draw() {
	if(changed_) {
		changed_=false;
		erase();
	}
	mvprintw(0,0,"Hello welcome to duncan please enter your name: %s",name_.print().c_str());
	if(!getip_) move(0,48+name_.cursorpos());
	else {
		mvprintw(1,0,"Hello %s, to which IP address would you like to connect? %s",name_.print().c_str(),ip_.print().c_str());
		move(1,55+name_.print().length()+ip_.cursorpos());
	}
	refresh();
}
