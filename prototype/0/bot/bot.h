#ifndef BOT_BOT_H_
#define BOT_BOT_H_

#include <string>
#include <cstdlib>
#include "../client/gamestate.h"
#include "../timer/timer.h"

namespace bot {

/*!
 * Create a bot which plays duncan automatically.
 * 
 * \author Harry Slatyer
 *
 * \copyright see LICENSE
 */
class Bot {
	private:
		const static double kChangeInterval=1;
		const static double kSecondsPerUpdate=0.1;

		client::GameState game_;
		bool running_;
		double last_dir_change_;
		timer::Timer timer_;
		unsigned int last_key_;
		unsigned int dir_keys_[4];
		std::string name_; // hello i am duplicating data and there's NOTHING you can do to stop me
	public:
		Bot(const std::string&, const std::string&);
		~Bot();
	
		void message(const std::string&);
		bool update();
};

} // namespace

#endif
