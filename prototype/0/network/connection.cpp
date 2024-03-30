#include "connection.h"

using namespace net;

/**
 * ENet initialisation status.
 * Positive for uninitialised, negative for an ENet error, or zero for success.
 */
int Connection::enetinit = 1337;

/**
 * Ensures that ENet has been initialised.
 * Can safely be called multiple times.
 */
void Connection::init() {
	if(enetinit > 0) {
		// need to initialise enet
		enetinit = enet_initialize();
		atexit(enet_deinitialize);
	}
	if(enetinit < 0) {
		// failed to initialise enet
		throw std::runtime_error("Could not initialise network subsystem.");
	}
}

/**
 * Server-like constructor.
 * Creates a Connection to listen locally.
 * \param port port on which to listen
 * \throw std::runtime_error if ENet fails
 */
Connection::Connection(port_t port)
	: host_(0)
	, listener_(0)
	, peers_(kMaxUsers)
	, ind_(kMaxUsers)
	, connected_(false)
{
	// make sure enet has been initialised
	init();

	// making server
	ENetAddress addr;
	addr.port = port;
	addr.host = ENET_HOST_ANY;
	
	host_ = enet_host_create(&addr,
		                     kMaxUsers,
		                     kNumChannels,
		                     0,  // no incoming bandwidth limit
		                     0); // no outgoing bandwidth limit
	if(! host_) {
		throw std::runtime_error("Could not create host.");
	}
}

/**
 * Client-like constructor.
 * Creates a Connection and connects to the specified server.
 * \param address hostname or ip to which to connect
 * \param lis pointer to listener object
 * \param port port on which to connect
 * \throw std::runtime_error if ENet fails
 * \throw std::runtime_error if ENet cannot resolve the address
 * \throw std::runtime_error if ENet cannot connect to the server, or connect times out
 */
Connection::Connection(const std::string& address, PacketListener* lis, port_t port)
	: host_(0)
	, listener_(lis)
	, ind_(kMaxUsers)
	, connected_(false)
{
	// make sure enet has been initialised
	init();

	// want to connect to a remote server
	host_ = enet_host_create(0,  // nullptr for client host
	                         1,  // only needs to talk to one server
	                         kNumChannels,
	                         0,  // no incoming bandwidth limit
	                         0); // no outgoing bandwidth limit
	if(! host_) {
		throw std::runtime_error("Could not create host.");
	}

	// try to connect
	ENetAddress addr;
	addr.port = port;
	if(enet_address_set_host(&addr, address.c_str()) < 0) {
		enet_host_destroy(host_);
		throw std::runtime_error("Could not resolve remote address.");
	}
	ENetPeer* server = enet_host_connect(host_, &addr, kNumChannels, 0);
	if(! server) {
		enet_host_destroy(host_);
		throw std::runtime_error("Could not connect to "+address);
	}
	ENetEvent event;
	if(enet_host_service(host_, &event, kConnectTimeout) > 0) {
		switch(event.type) {
			case ENET_EVENT_TYPE_CONNECT:
				connected_ = true;
				ind_ = event.peer->outgoingPeerID;
				if(listener_) listener_->on_connect(event.peer->incomingPeerID);
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				enet_packet_destroy(event.packet);
				break;
			default:
				break;
		}
	}
	if(!connected_) {
		enet_host_destroy(host_);
		throw std::runtime_error("Connecting timed out to "+address);
	}
	peers_.push_back(server);
}

/**
 * Disconnects all peers.
 */
Connection::~Connection() {
	for(size_t i=0; i<peers_.size(); i++) {
		if(peers_[i]) {
			enet_peer_disconnect_now(peers_[i], 0/* user data */);
		}
	}
}

/**
 * Send some data to a peer.
 * \param buf buffer of data
 * \param size length of buffer
 * \param peer index of peer to whom to send data
 * \todo (maybe) benchmark flushing once per server frame vs once per message
 */
void Connection::send(const void* buf, size_t size, index_t peer, bool reliable) {
	enet_peer_send(peers_[peer], 0 /* channel id */,
	               enet_packet_create(buf, size, reliable ? ENET_PACKET_FLAG_RELIABLE : 0));
	enet_host_flush(host_);
}

void Connection::set_listener(PacketListener* l) {
	listener_ = l;
}

bool Connection::connected() {
	return connected_;
}

/**
 * Processes any events sent to this Connection.
 * Connect, disconnect and receive events are forwarded to the listener.
 */
void Connection::receive() {
	ENetEvent event;
	while(enet_host_service(host_, &event, 0/* timeout */) > 0) {
		if(event.type==ENET_EVENT_TYPE_NONE) continue;
		const index_t ind = event.peer->incomingPeerID;
		switch(event.type) {
			case ENET_EVENT_TYPE_CONNECT:
				if(ind >= kMaxUsers) {
					// server is full
					break; // TODO send back "full server" packet
				}
				peers_[ind] = event.peer;
				if(listener_) listener_->on_connect(ind);
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				if(listener_) listener_->on_packet(event.packet->data, event.packet->dataLength, ind);
				enet_packet_destroy(event.packet);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				if(listener_) listener_->on_disconnect(ind);
				peers_[ind] = 0;
				break;
			default:
				break;
		}
	}
}

/**
 * Get the number of bytes sent from the host since the 
 * last call to this function.
 * \returns bytes sent since last call to this function
 */
unsigned int Connection::bytes_sent() {
	unsigned int ret=host_->totalSentData;
	host_->totalSentData=0;
	return ret;
}

/**
 * Get the number of bytes received by the host since the 
 * last call to this function.
 * \returns bytes received since last call to this function
 */
unsigned int Connection::bytes_received() {
	unsigned int ret=host_->totalReceivedData;
	host_->totalReceivedData=0;
	return ret;
}
