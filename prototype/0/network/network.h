#ifndef NET_NETWORK_H_
#define NET_NETWORK_H_

#include <enet/enet.h>

/*!
 * Constants and typedefs for network subsystem.
 *
 * \author Malcolm Gill
 *
 * \copyright see LICENSE
 */
namespace net {
	/// type for port numbers
	typedef enet_uint16 port_t;

	/// type for peer indices
	typedef enet_uint16 index_t;

	/// type for packet data
	typedef enet_uint8  data_t;

	/// type for positions
	typedef enet_uint16 pos_t;

	/// type for directions
	typedef enet_uint8 dir_t;

	/// type for map hashes
	typedef enet_uint32 maphash_t;

	/// maximum simultaneously connected clients per server
	const size_t kMaxUsers = 1337;

	/// maximum characters (including NPCs) per world
	const size_t kMaxCharacters = 1337;

	/// how many channels of network traffic
	const size_t kNumChannels = 1;

	/// flag for ensuring a packet arrives at its destination
	const bool kReliable = true;

	/// port used by game network traffic
	const port_t kPort = 13370;

	/// how long (milliseconds) to try to connect to a server
	const enet_uint32 kConnectTimeout = 5000;

	/// maximum bytes of user data to be sent in a single packet
	const size_t kMaxPacketSize = 2048;

	/// maximum `distance' a player can see
	const size_t kViewRange = 10;

	/**
	 * maximum bytes of data to be sent (for all clients/worlds) in a server frame
	 * \note server can hang if kMaxFrameSize < kMaxPacketSize
	 */
	const size_t kMaxFrameSize = 3000; // for 60 Hz this would be a bit under 1.5 Mbit/sec

	/// minimum squared distance two players may be from one another
	const double kTolerance = 0.8;

	namespace mnum {
		typedef data_t t;
		
		/// a new player sends her name to the server
		const t name = 0;
	
		/// inter-player communication
		const t chat = 1;

		/// the server sends the names of players to a client
		const t playernames = 2;

		/// the server sends the positions of players to a client
		const t playerpositions = 3;

		/// the server tells clients when a player has disconnected
		const t playerdisconnected = 4;

		/// notify the server that the player has changed their movement direction
		const t direction = 5;

		/// tell the client which worlds to which she can change (if she pleases)
		const t canchange = 6;

		/// tell the client the URL of the server from which to download maps
		const t curlurl = 7;

		/// tell the server that the client wants to change world
		const t requestchangeworld = 8;

		/// tell the client that her world has been changed
		const t changeworld = 9;

		/// tell the client to calculate a path for an NPC, or tell the server about such a path
		const t path = 10;

		/// temporary
		const t morespam = 254;
		const t spam = 255;
	};

	namespace direction {
		typedef dir_t t;

		/// up
		const t up = 1;

		/// down
		const t down = 2;

		/// left
		const t left = 4;

		/// right
		const t right = 8;
	};

	/// Represents a packet to be sent over the network.
	struct Packet {
		/// packet data
		data_t data[kMaxPacketSize];

		/// length of data in this packet
		size_t size;

		/// whether or not ENet should ensure this packet arrives
		bool reliable;
	};
};

#endif
