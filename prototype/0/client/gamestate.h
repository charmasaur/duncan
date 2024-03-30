#ifndef CLIENT_GAMESTATE_H_
#define CLIENT_GAMESTATE_H_

#include <ncurses.h>
#include <string>
#include <list>
#include <cstring>
#include <sstream>
#include "inputline.h"
#include "state.h"
#include "input.h"
#include "world.h"
#include "ncwindow.h"
#include "pathfinder.h"
#include "../network/connection.h"
#include "../network/packetlistener.h"
#include "../dl/downloader.h"
#include "../dl/downloadlistener.h"

namespace client {

/*!
 * The main game state.
 * 
 * Connect to a server and provide an interface for the user to player the game.
 * 
 * \author Harry Slatyer
 *
 * \copyright see LICENSE
 */
class GameState : public State, public net::PacketListener, public dl::DownloadListener {
	private:
		static const size_t kMaxMessages=10;
		unsigned int rows_,cols_;
		InputLine chat_;
		State::Action state_;
		std::string ip_, name_;
		std::list<std::string> messages_;
		bool changed_,chatting_;
		NCWindow worldwin_,chatwin_,dlwin_;

		dl::Downloader dl_;

		std::string next_world_;
		net::maphash_t next_hash_;

		net::dir_t move_direction_;

		World world_;

		PathFinder pathfinder_;

		net::Connection* con_;

		void new_message(const std::string&);
		void send_chat();
		void draw_chat();
		void draw_dl();

		bool check_hash();

		void request_change_world();

		void move(net::dir_t, bool);
	public:
		GameState();

		void resize(unsigned int, unsigned int);
		void set_ip(const std::string&);
		void set_name(const std::string&);
		void set_dim(unsigned int, unsigned int);
		void on_packet(net::data_t*, size_t, net::index_t);
		void on_connect(net::index_t);
		void on_disconnect(net::index_t);
		void on_press(unsigned int,bool);
		void on_release(unsigned int,bool);
		void on_progress(double,double);
		void on_push();
		void on_pop();
		void on_active();
		State::Action update();
		void draw();
};

} // namespace

#endif
