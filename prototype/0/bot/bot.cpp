#include "bot.h"

using namespace bot;

/**
 * Create a bot with a given name, and connect to a given server.
 * \param name name of bot
 * \param ip ip of server
 */
Bot::Bot(const std::string& name, const std::string& ip) : running_(true), last_dir_change_(0.0), timer_(1000), last_key_(0), name_(name) {
	game_.set_name(name);
	game_.set_ip(ip);
	game_.on_push();
	timer_.reset();
	dir_keys_[0]=client::Input::kUp;
	dir_keys_[1]=client::Input::kDown;
	dir_keys_[2]=client::Input::kLeft;
	dir_keys_[3]=client::Input::kRight;
}

/**
 * Disconnect from the server.
 */
Bot::~Bot() {
	game_.on_pop();
}

/**
 * Handle a message entered at the terminal.
 * \param msg message
 */
void Bot::message(const std::string& msg) {
	if(msg=="quit") running_=false;
}

/**
 * Update the bot.
 * \returns whether or not the bot should continue to run
 */
bool Bot::update() {
	double dt=(double)timer_.elapsed()/1000.0;
	if(dt<kSecondsPerUpdate) {
		usleep(1000000.0*(kSecondsPerUpdate-dt));
		dt=kSecondsPerUpdate;
	}
	else std::cout << "slow! ("<<name_<<")"<<std::endl;
	timer_.reset();
	last_dir_change_+=dt;
	if(last_dir_change_>kChangeInterval) {
		last_dir_change_=0.0;
		game_.on_release(last_key_,false);
		last_key_=dir_keys_[rand()%4];
		game_.on_press(last_key_,false);
	}
	if(game_.update()==client::State::POP_ME) running_=false;
	return running_;
}
