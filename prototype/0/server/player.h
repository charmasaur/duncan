#ifndef SERVER_PLAYER_H_
#define SERVER_PLAYER_H_

#include "../network/network.h"
#include "character.h"
#include <algorithm>
#include <list>
#include <stack>
#include <string>
#include <tr1/memory>
#include <vector>

namespace server {

/*!
 * Represent a player.
 *
 * \author Harry Slatyer
 * \author Malcolm Gill
 *
 * \copyright see LICENSE
 */
class Player : public Character {
	private:
		net::index_t enet_index_;

		/// How many packets can simultaneously be queued for this Player.
		static const size_t kPacketPoolSize = 10;

		/// Statically-allocated container of Packets
		std::vector<net::Packet> packets_;

		/// Indices of unused slots in packets array.
		std::stack<size_t> available_;

		/// Indices of used slots in packets array...
		std::list<size_t> used_;

		net::Packet* clean_packet(size_t);
	public:
		Player();

		/// Packet pointer pair.
		typedef std::pair<net::Packet*,net::Packet*> pppair_t;

		bool connected;
		size_t windex;
		std::string changeworld;
		double last_pos_send;

		Player(net::index_t);
		net::Packet* next_packet();
		pppair_t demand_packet();
		void pop_packet(net::Packet*);

		net::index_t enet_index();

		virtual bool can_find_path();
};

}

#endif
