#ifndef NET_PACKETLISTENER_H_
#define NET_PACKETLISTENER_H_

#include "network.h"

namespace net {

/*!
 * Interface for listening to network activity.
 *
 * \author Malcolm Gill
 *
 * \copyright see LICENSE
*/
class PacketListener {
	public:
		virtual ~PacketListener() {}

		/**
		 * Called when a packet is received.
		 * \param buf raw data received
		 * \param size buffer length
		 * \param peer index of peer from which data was received
		 *             (for a client, this will be 0; for the server,
		 *             it specifies which client has sent the data)
		 */
		virtual void on_packet(data_t* buf, size_t size, index_t peer) {}

		/**
		 * Called when a client has connected to this host.
		 * \param peer index of newly joined peer
		 */
		virtual void on_connect(index_t peer) {}

		/**
		 * Called when a client has disconnected from this host.
		 * \param peer index of departing peer
		 */
		virtual void on_disconnect(index_t peer) {}
};

} // namespace

#endif
