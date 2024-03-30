#ifndef SERVER_SERVER_H_
#define SERVER_SERVER_H_

#include <cstring>
#include <string>
#include <iostream>
#include <sstream>
#include <tr1/memory>
#include <vector>
#include <map>
#include "../network/network.h"
#include "../network/connection.h"
#include "../network/packetlistener.h"
#include "world.h"
#include "../timer/timer.h"

namespace server {

/*!
 * The main server class.
 *
 * Maintain a list of connected clients and a collection of worlds.
 *
 * \author Harry Slatyer
 * \author Malcolm Gill
 *
 * \copyright see LICENSE
 */
class Server : public net::PacketListener {
	private:
		static const size_t kMaxWorlds=10;
		static const double kSecondsPerUpdate;
		static const size_t kSecondsPerInfoPrint=1;

		std::string curl_url_;

		timer::Timer timer_, second_timer_;

		unsigned int frame_count_;

		bool running_;
		size_t windex_;
		std::vector<std::tr1::shared_ptr<World> > worlds_;
		std::vector<Player> players_;
		std::map<const std::string,size_t> world_ids_;

		net::Connection con_;

		void sendcanchange(net::index_t,size_t);
	public:
		Server();

		void on_packet(net::data_t* buf, size_t size, net::index_t peer);
		void on_connect(net::index_t peer);
		void on_disconnect(net::index_t peer);

		void message(const std::string& str);

		bool update();
};

} // namespace

#endif
