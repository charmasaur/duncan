#ifndef SERVER_PLAYERPACKET_H_
#define SERVER_PLAYERPACKET_H_

#include <tr1/memory>
#include "player.h"
#include "../network/network.h"

namespace server {

/*!
 * Associates a net::Packet with a player index,
 * representing a packet to be sent to a particular player.
 *
 * \author Malcolm Gill
 *
 * \copyright see LICENSE
 */
struct PlayerPacket {
	public:
		/// Pointer to PlayerPacket
		typedef std::tr1::shared_ptr<PlayerPacket> pointer_t;

		/// Player in server::World to whom this packet belongs.
		Player* player;

		/// Data to be sent to Player.
		net::Packet* packet;

		/**
		 * Trivial constructor.
		 * \param i value for ind.
		 * \param p pointer to Packet.
		 */
		PlayerPacket(Player* pl, net::Packet* p) : player(pl), packet(p) {}
};

} // namespace

#endif
