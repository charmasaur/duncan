#ifndef SERVER_WORLD_H_
#define SERVER_WORLD_H_

extern bool g_serverpath;

#include <algorithm>
#include <tr1/memory>
#include <cstring>
#include <queue>
#include <iostream>
#include <sstream>
#include "charlist.h"
#include "player.h"
#include "npc.h"
#include "playerpacket.h"
#include "map.h"
#include "pathworld.h"
#include "pathfinder.h"

namespace server {

/*!
 * Store information about a world.
 *
 * Contains a map, players, etc...
 *
 * \author Harry Slatyer
 * \author Malcolm Gill
 *
 * \copyright see LICENSE
 */
class World : public PathWorld {
	public:
		typedef CharList<Character> CList;
	private:
		/// How often (seconds) to send position update data to each client.
		const static double kPositionPacketInterval=0.1;

		/// How often (seconds) to request new paths for moving NPCs.
		const static double kPathPacketInterval=1.5;

		/// How often (seconds) to rebuild the Character tree.
		const static double kTreeRebuildInterval=1;

		/// How long (seconds) since the Character tree was last rebuilt.
		double tree_age_;
		
		/// Data to be sent to clients.
		std::queue<PlayerPacket::pointer_t> outgoing_;

		/// Gives random access to all Characters currently inside this World.
		Character* charlist_[net::kMaxCharacters];

		/// Unused indices in playerlist_
		std::queue<net::index_t> available_;

		/// Tree containing all Characters, mainly for spatial lookups.
		CList chars_;

		typedef std::vector<Player*> PList;
		/// Quick access to all Players in this World (could s/vector/list/ if that's how you roll).
		PList players_;

		typedef std::vector<std::tr1::shared_ptr<NPC> > NPCList;
		/// Quick access to all NPCs in this World
		NPCList npcs_;
	
		std::queue<Player*> to_join_;
		std::pair<Character*,std::pair<double,double> > to_move_[net::kMaxCharacters];

		PlayerPacket::pointer_t player_packet(Player*);
		PlayerPacket::pointer_t demand_player_packet(Player*);

		std::string mapname_;
		ServerMap map_;
		net::maphash_t maphash_;

		PathFinder path_finder_;

		void send_disconnect(net::index_t);
		void send_names(Player*);	
		void send_positions(Player*);
		void send_path_request(Player*,NPC*);

		void chat(Player*,const std::string&);

		void next_join();
	public:
		World(const std::string&);
		World(const World&);
		const World& operator=(const World&);

		void name_changed(Player*);
		const PlayerPacket::pointer_t next_outgoing();
		void pop_outgoing();
		void update(double);
		void join(Player*);
		void disconnect(Player*);
		void packet(net::data_t*, size_t, Player*);

		net::maphash_t maphash();
		const std::string& mapname();
		bool is_vacant(double,double,Character*) const;
		size_t width() const;
		size_t height() const;
};

}

#endif
