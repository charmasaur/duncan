#ifndef NETWORK_CONNECTION_H_
#define NETWORK_CONNECTION_H_

#include <enet/enet.h>
#include <stdexcept>
#include <string>
#include <vector>
#include "network.h"
#include "packetlistener.h"

namespace net {

/*!
 * Handles a network connection.
 * 
 * This class provides an abstract interface to a connection made over network
 * sockets. The connection is opened during object construction, and closed in
 * destruction. 
 *
 * \author Harry Slatyer
 * \author Malcolm Gill
 *
 * \copyright see LICENSE
 */
class Connection {
	private:
		ENetHost* host_;
		PacketListener* listener_;
		std::vector<ENetPeer*> peers_;
		index_t ind_;
		bool connected_;
		static int enetinit;
		void init();
	public:
		Connection(const std::string& address, PacketListener* lis = 0, port_t port = kPort);
		Connection(port_t port = kPort);
		~Connection();
		void send(const void* buf, size_t size, index_t peer = 0, bool reliable = kReliable);
		void set_listener(PacketListener*);
		bool connected();
		void receive();

		unsigned int bytes_sent();
		unsigned int bytes_received();
};

} // namespace

#endif
