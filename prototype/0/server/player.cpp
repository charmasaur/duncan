#include "player.h"
using namespace server;

/**
 * Create a Player with a given enet index.
 * \param ind enet index
 */
Player::Player(net::index_t ind) : enet_index_(ind), packets_(kPacketPoolSize), connected(false), windex(0), changeworld(""), last_pos_send(0.0) {
	set_speed(15.0);
	for(size_t i=0; i<kPacketPoolSize; ++i) available_.push(i);
}

/**
 * Gets a pointer to a Packet to be filled with data to be sent to this Player.
 * \returns pointer to next available Packet, or nullptr if none is available.
 */
net::Packet* Player::next_packet() {
	if(available_.empty()) return 0; // nullptr
	size_t idx = available_.top();
	available_.pop();
	used_.push_back(idx);
	return clean_packet(idx);
}

/**
 * Gets a pointer to a usable Packet.
 * If none is available, allow an already-used Packet to be overwritten;
 * tries to find a non-reliable packet first, but if that fails, then the
 * most recently used packet is reused.
 * \returns (pointer to discarded packet (or null if none), pointer to usable packet)
 */
Player::pppair_t Player::demand_packet() {
	if(net::Packet* p = next_packet()) {
		// found a spare packet
		return pppair_t(0,p);
	}

	// no spare packets
	for(std::list<size_t>::iterator it=used_.begin(); it!=used_.end(); ++it) {
		if(!packets_[*it].reliable) {
			// found an unreliable packet to reuse
			net::Packet* ret = clean_packet(*it);
			return pppair_t(ret,ret);
		}
	}

	// no unreliable packets; take the most recently used
	size_t idx = used_.back();

	// move it to the front of the list
	// so successive calls to demand_packet will not reuse the same packet
	used_.pop_back();
	used_.push_front(idx);
	
	net::Packet* ret = clean_packet(idx);
	return pppair_t(ret,ret);
}

/**
 * Marks the specified packet as once again available to be used to send data.
 */
void Player::pop_packet(net::Packet* p) {
	// use some pointer arithmetic to find the offset into the packets array
	// (this wouldn't work if packets were not stored contiguously)
	size_t idx = p - &packets_[0];
	available_.push(idx);
	used_.erase(remove(used_.begin(), used_.end(), idx));
}

/**
 * Get player's enet index.
 * \returns enet index of player
 */
net::index_t Player::enet_index() {
	return enet_index_;
}

/**
 * Resets any data stored in the specified packet.
 * \param idx index in packets_ of desired Packet.
 * \returns pointer to Packet.
 * \note Packets are marked as reliable by default.
 */
net::Packet* Player::clean_packet(size_t idx) {
	packets_[idx].size = 0;
	packets_[idx].reliable = true;
	return &packets_[idx];
}

/**
 * Players can be used for pathfinding.
 */
bool Player::can_find_path() {
	return true;
}
