#include "world.h"

using namespace server;

/**
 * Initialise the queue of available indices, and load all the data.
 * \param filename name of file
 */
World::World(const std::string& filename) : tree_age_(0), mapname_(filename), map_("data/"+filename+".map"), path_finder_(this) {
	maphash_=map_.hash();

	// load NPCs
	size_t npccount = 400*(filename=="map0");//+(filename=="map1");
	const std::vector<std::pair<size_t,size_t> >& vacant=map_.spawn();
	for(size_t i=0; i<npccount; i++) {
		std::tr1::shared_ptr<NPC> npc(new NPC(vacant.at(i),vacant.at((i+rand())%vacant.size())));
		std::ostringstream oss;
		oss << "N" << i;
		npc->set_name(oss.str());
		npc->index = i;
		npc->last_path_send = rand()/double(RAND_MAX)*kPathPacketInterval;
		chars_.insert(npc.get());
		npcs_.push_back(npc);
	}

	// make space for player characters
	for(net::index_t i=npccount;i<net::kMaxCharacters;i++) available_.push(i);
	players_.reserve(net::kMaxCharacters-npccount);

	// make sure initialising tree_age_ to zero was not a lie
	chars_.optimise();
}

/**
 * Copy constructor, differing from the default one in two ways:
 *  1. copy-constructing a KDTree optimises it, so tree_age_ is set to 0
 *  2. path_finder_ is initialised using this World, rather than
 *     being copy-constructed and pointing to o
 */
World::World(const World& o)
	: tree_age_(0)
	, outgoing_(o.outgoing_)
	, available_(o.available_)
	, chars_(o.chars_)
	, players_(o.players_)
	, npcs_(o.npcs_)
	, to_join_(o.to_join_)
	, mapname_(o.mapname_)
	, map_(o.map_)
	, maphash_(o.maphash_)
	, path_finder_(this)
{
	// how good are static arrays
	std::copy(o.charlist_, o.charlist_+net::kMaxCharacters, charlist_);
	std::copy(o.to_move_, o.to_move_+net::kMaxCharacters, to_move_);
}

/**
 * Same differences from default as copy constructor
 * (since KDTree's operator= also optimises).
 */
const World& World::operator=(const World& o) {
	tree_age_    = 0;
	outgoing_    = o.outgoing_;
	std::copy(o.charlist_, o.charlist_+net::kMaxCharacters, charlist_);
	available_   = o.available_;
	chars_       = o.chars_;
	players_     = o.players_;
	npcs_        = o.npcs_;
	to_join_     = o.to_join_;
	std::copy(o.to_move_, o.to_move_+net::kMaxCharacters, to_move_);
	mapname_     = o.mapname_;
	map_         = o.map_;
	maphash_     = o.maphash_;
	path_finder_ = PathFinder(this);
	return *this;
}



/**
 * Gets a pointer to the next queued packet associated with this World.
 * \note does not remove the packet from the queue; call pop_outgoing for that.
 * \returns pointer to next PlayerPacket to be sent, or 0 (nullptr) if there is none.
 */
const PlayerPacket::pointer_t World::next_outgoing() {
	return outgoing_.empty() ? PlayerPacket::pointer_t() : outgoing_.front();
}

/**
 * Discards a packet from the front of the queue of packets to be sent.
 * Does nothing if the queue is empty.
 */
void World::pop_outgoing() {
	if(outgoing_.empty()) return;
	const PlayerPacket::pointer_t ppp = outgoing_.front();
	ppp->player->pop_packet(ppp->packet);
	outgoing_.pop();
}

/**
 * Finds if the player specified by ind has any spare outgoing packet slots.
 * \param player pointer to player from whom packet is requested.
 * \returns pointer to empty PlayerPacket if found, or null pointer if not.
 */
PlayerPacket::pointer_t World::player_packet(Player* player) {
	if(net::Packet* const packet = player->next_packet()) {
		// p is a pointer to the player's next available packet
		return PlayerPacket::pointer_t(new PlayerPacket(player, packet));
	} else {
		// this player has run out of slots :(
		return PlayerPacket::pointer_t();
	}
}

/**
 * Demands a pointer to a usable PlayerPacket.
 * Will discard one of the Player's packets if necessary.
 * This is an expensive operation so should only be used sparingly...
 * \param player pointer to player from whom packet is requested.
 * \returns pointer to empty PlayerPacket.
 */
PlayerPacket::pointer_t World::demand_player_packet(Player* player) {
	// pair = (pointer to discarded packet, pointer to new packet)
	// n.b. pppp.first==0 xor pppp.first==pppp.second
	Player::pppair_t pppair(player->demand_packet());
	if(pppair.first) {
		// had to discard a packet; get rid of it from the outgoing queue
		// N.B. this is awful (written to preserve packet order in outgoing_)
		std::queue<PlayerPacket::pointer_t> q;
		while(! outgoing_.empty()) {
			if(outgoing_.front()->packet != pppair.first) {
				q.push(outgoing_.front());
			}
			outgoing_.pop();
		}
		while(! q.empty()) {
			outgoing_.push(q.front());
			q.pop();
		}
	}
	return PlayerPacket::pointer_t(new PlayerPacket(player, pppair.second));
}

/**
 * Move all the players.
 */
void World::update(double dt) {
	size_t n_to_move=0;
	// update stuff that applies to all Characters (Players and NPCs)
	for(CList::const_iterator ch=chars_.begin();ch!=chars_.end();++ch) {
		Character *player = (*ch).first; //playerlist_[*ind];
		if(player->is_moving()) {
			double next_row = std::min((double)map_.height()-1.0,std::max(0.0,player->row()+dt*player->rspeed()));
			double next_col = std::min((double)map_.width()-1.0,std::max(0.0,player->col()+dt*player->cspeed()));
			to_move_[n_to_move].first=player;
			to_move_[n_to_move].second.first=next_row;
			to_move_[n_to_move].second.second=next_col;
			n_to_move++;
		}
	}

	for(size_t i=0;i<n_to_move;i++)
		if(is_vacant(to_move_[i].second.first,to_move_[i].second.second,to_move_[i].first)) chars_.move(to_move_[i].first,to_move_[i].second.first,to_move_[i].second.second);
	n_to_move=0;

	// Player-specific update stuff (doesn't apply to NPCs)
	for(PList::const_iterator pp=players_.begin(); pp!=players_.end(); ++pp) {
		(*pp)->last_pos_send+=dt;
		if((*pp)->last_pos_send>kPositionPacketInterval) send_positions(*pp);
	}

	// NPC-specific update stuff (doesn't apply to Players)
	for(NPCList::const_iterator npcp=npcs_.begin(); npcp!=npcs_.end(); ++npcp) {
		if((*npcp)->should_snap()) {
			std::pair<double,double> pos((*npcp)->desired_pos());
			to_move_[n_to_move].first=npcp->get();
			to_move_[n_to_move].second=pos;
			n_to_move++;
		}
		(*npcp)->update();
		(*npcp)->last_path_send+=dt;
		if((*npcp)->last_path_send>kPathPacketInterval) send_path_request(0,npcp->get());
	}
	
	for(size_t i=0;i<n_to_move;i++)
		if(is_vacant(to_move_[i].second.first,to_move_[i].second.second,to_move_[i].first)) {
			chars_.move(to_move_[i].first,to_move_[i].second.first,to_move_[i].second.second);
			((NPC*)(to_move_[i].first))->snapped();
		}
	n_to_move=0;

	// join new players one per frame
	next_join();

	// rebuild the Character tree every so often
	tree_age_+=dt;
	if(tree_age_ >= kTreeRebuildInterval) {
		chars_.optimise();
		tree_age_ = 0;
	}
}

/**
 * \return true iff the tile at the specified coordinate is not occupied
 *         by anything that would prevent a player from moving there.
 * \param row coordinate of top-left corner
 * \param col coordinate of top-left corner
 * \param ignore pointer to Player to ignore
 */
bool World::is_vacant(double row, double col, Character* ignore) const {
	// truncate yay
	size_t r(row), c(col);
	if(r>=height() || c>=width()) return false;

	// quickly check if the map itself is blocking the tile
	if(!map_.is_vacant(r,c)) {
		return false;
	}

	// now go through all characters in the surrounding area and check if any of them is occupying the space
	std::vector<CList::node_t> v;
	Character* cp;
	chars_.square(row,col,2.0,v);
	for(std::vector<CList::node_t>::const_iterator cpi=v.begin(); cpi!=v.end(); ++cpi) {
		cp=(*cpi).first;
		if(cp==ignore) continue; // can't block yourself LOL
		// calculate squared distance and check if it would cause a collision
		double dr = cp->row()-row, dc = cp->col()-col, dist = dr*dr+dc*dc;
		if(dist < net::kTolerance) {
			// occupied
			return false;
		}
	}

	return true;
}

/**
 * Notify other players that a player has changed her name.
 * \param p player whose name changed
 */
void World::name_changed(Player* p) {
	net::index_t senderind=p->index;
	PlayerPacket::pointer_t ppp;
	for(PList::const_iterator pp=players_.begin(); pp!=players_.end(); ++pp) {
		if(ppp = player_packet(*pp)) {
			ppp->packet->data[0] = net::mnum::playernames;
			std::memcpy(ppp->packet->data+1,&senderind,sizeof(senderind));
			std::memcpy(ppp->packet->data+1+sizeof(senderind),p->name().c_str(),p->name().length()+1);
			ppp->packet->size = 1+sizeof(senderind)+p->name().length()+1;
			ppp->packet->reliable = true;
			outgoing_.push(ppp);
		} else {
			std::cout << "Can't send name change of " << p->index << " to " << (*pp)->index << "\n";
		}
	}
}

/**
 * Send a player the positions of all the Characters she can see.
 * \param p player to whom positions will be sent
 */
void World::send_positions(Player* p) {
	if(PlayerPacket::pointer_t ppp = player_packet(p)) {
		size_t sz;
		net::pos_t pos;
		net::index_t ind;
		std::vector<CList::node_t> v;
		Character* cp;
		chars_.square(p,net::kViewRange,v);
		p->last_pos_send=0;
		ppp->packet->data[0]=net::mnum::playerpositions; sz=1;
		for(std::vector<CList::node_t>::const_iterator cpi=v.begin(); cpi!=v.end(); ++cpi) {
			cp=(*cpi).first;
			ind=cp->index;
			std::memcpy(ppp->packet->data+sz,&ind,sizeof(ind)); sz+=sizeof(ind);
			pos=(net::pos_t)cp->row();
			std::memcpy(ppp->packet->data+sz,&pos,sizeof(pos)); sz+=sizeof(pos);
			pos=(net::pos_t)cp->col();
			std::memcpy(ppp->packet->data+sz,&pos,sizeof(pos)); sz+=sizeof(pos);
		}
		ppp->packet->size=sz;
		ppp->packet->reliable=false;
		outgoing_.push(ppp);
	}
}

/**
 * Send a player the names of all Characters in the server.
 * \param p player to whom names will be sent
 */
void World::send_names(Player* p) {
	PlayerPacket::pointer_t ppp;
	CList::const_iterator chi;
	Character* ch;
	size_t sz(0);
	net::index_t ind;
	for(chi=chars_.begin(); chi!=chars_.end(); ++chi) {
		if(!ppp) {
			if(!(ppp=player_packet(p))) break;
			ppp->packet->data[0]=net::mnum::playernames; sz=1;
		}
		ch=(*chi).first;
		ind=ch->index;
		if(sz+sizeof(ind)+ch->name().length()+1>=net::kMaxPacketSize) {
			ppp->packet->size=sz;
			ppp->packet->reliable=true;
			outgoing_.push(ppp);
			ppp=PlayerPacket::pointer_t();
			--ch;
		} else {
			std::memcpy(ppp->packet->data+sz,&ind,sizeof(ind)); sz+=sizeof(ind);
			std::memcpy(ppp->packet->data+sz,ch->name().c_str(),ch->name().length()+1); sz+=ch->name().length()+1;
		}
	}	
	if(chi!=chars_.end()) std::cout << "Can't send all names to " << p->index << "\n";
	if(ppp) {
		ppp->packet->size=sz;
		ppp->packet->reliable=true;
		outgoing_.push(ppp);
	}
}

/**
 * Helper function.
 * \returns false iff the specified Character can be used for pathfinding.
 */
bool not_pathfinder(World::CList::node_t& c) {
	return !c.first->can_find_path();
}

/**
 * Ask a client to find a path for an NPC.
 * \param p ignored
 * \param npc npc whose path to find
 */
void World::send_path_request(Player* p, NPC* npc) {
	// get list of Players close enough to see the npc
	std::vector<CList::node_t> nearby;
	chars_.square(npc,net::kViewRange,nearby);

	// filter out non-pathfinders
	nearby.erase(std::remove_if(nearby.begin(),nearby.end(),not_pathfinder), nearby.end());

	// if there are none, let the npc keep going with his old path
	// he'll probably get stuck eventually but nobody will see him so who cares right?
	if(nearby.empty()) return;

	// if the server is meant to find the paths, no need to ask a client
	if(g_serverpath) {
		PathFinder::path_t path;
		npc->last_path_send-=kPathPacketInterval;
		if(!path_finder_.path(npc->row(),npc->col(),npc->destr(),npc->destc(),npc,path)) return;
		npc->set_path(path);
		std::cout << "(" << npc->row() << "," << npc->col() << ") -> (" << npc->destr() << "," << npc->destc() << ")\n";
		return;
	}

	// pick a random pathfinder; safe to cast Character to Player
	// (since can_find_path should only be overridden when this is the case)
	p = static_cast<Player*>(nearby[rand()%nearby.size()].first);

	// send pathfinding packet
	if(PlayerPacket::pointer_t ppp = player_packet(p)) {
		// running count of how many bytes have been put in the buffer
		size_t sz = 0;

		// magic number
		ppp->packet->data[0] = net::mnum::path; sz += sizeof(net::mnum::path);

		// npc's index (to be ignored by pathfinder)
		std::memcpy(ppp->packet->data+sz,&(npc->index),sizeof(npc->index)); sz+=sizeof(npc->index);

		// coordinates
		net::pos_t r1(npc->row());
		net::pos_t c1(npc->col());
		net::pos_t r2(npc->destr());
		net::pos_t c2(npc->destc());
		std::memcpy(ppp->packet->data+sz,&r1,sizeof(r1)); sz+=sizeof(r1);
		std::memcpy(ppp->packet->data+sz,&c1,sizeof(c1)); sz+=sizeof(c1);
		std::memcpy(ppp->packet->data+sz,&r2,sizeof(r2)); sz+=sizeof(r2);
		std::memcpy(ppp->packet->data+sz,&c2,sizeof(c2)); sz+=sizeof(c2);
		ppp->packet->size = sz;
		ppp->packet->reliable = false; // think this is okay?
		outgoing_.push(ppp);
		npc->last_path_send = 0;
	}
}

/**
 * Process chat messages from players.
 * \param p player from whom chat was sent
 * \param msg chat message
 */
void World::chat(Player* p, const std::string& msg) {
	net::index_t senderind=p->index;
	for(PList::const_iterator pp=players_.begin(); pp!=players_.end(); ++pp) {
		if(PlayerPacket::pointer_t ppp = player_packet(*pp)) {
			ppp->packet->data[0] = net::mnum::chat;
			std::memcpy(ppp->packet->data+1,&senderind,sizeof(senderind));
			std::memcpy(ppp->packet->data+1+sizeof(senderind),msg.c_str(),msg.length()+1);
			ppp->packet->size = 1+sizeof(senderind)+msg.length()+1;
			ppp->packet->reliable = true;
			outgoing_.push(ppp);
		} else {
			// TODO: what to do?
		}
	}
	// TODO: is there a better way to send the same packet to many people? 
}

/**
 * Notify clients that a player has left the world.
 * \param pind index of player who left
 */
void World::send_disconnect(net::index_t pind) {
	for(PList::const_iterator pp=players_.begin(); pp!=players_.end(); ++pp) {
		if(PlayerPacket::pointer_t ppp = player_packet(*pp)) {
			ppp->packet->data[0] = net::mnum::playerdisconnected;
			std::memcpy(ppp->packet->data+1,&pind,sizeof(pind));
			ppp->packet->size = 1+sizeof(pind);
			ppp->packet->reliable = true;
			outgoing_.push(ppp);
		} else {
			std::cout << "Can't send disconnect of " << pind << " to " << (*pp)->index << "\n"; // TODO: what to do?
		}
	}
}

/**
 * Process a packet from a client.
 * \param buf packet data
 * \param size size (in bytes) of packet
 * \param p peer from whom packet was sent
 */
void World::packet(net::data_t* buf, size_t size, Player* p) {
	if(buf[0]==net::mnum::chat) {
		chat(p,(char*)(buf+1));
		return;
	}
	else if(buf[0]==net::mnum::direction) {
		net::dir_t d;
		std::memcpy(&d,buf+1,sizeof(d));
		p->set_dir(d);
		return;
	}
	else if(buf[0]==net::mnum::path) {
		// ignore the magic number
		buf++; size--;
		
		// how big is each coordinate pair
		size_t bs = 2*sizeof(net::pos_t); // block size

		// index of NPC
		net::index_t ind;
		std::memcpy(&ind,buf,sizeof(ind));
		size-=sizeof(ind);
		buf+=sizeof(ind);

		if(ind>=net::kMaxCharacters) {
			std::cout << "NPC index " << ind << "out of range.\n";
			return;
		}

		// get all waypoints out (or pick a random direction for the npc if no path is given)
		std::vector<std::pair<size_t,size_t> > path;
		if(size>bs) {
			path.reserve(size/bs);
			while(size >= bs) {
				net::pos_t waypoint[2];
				std::memcpy(waypoint,buf,bs);
				path.push_back(std::pair<size_t,size_t>(waypoint[0],waypoint[1]));
				size-=bs;
				buf+=bs;
			}
		}
		else {
			int r=rand()%4;
			path.push_back(std::pair<size_t,size_t>(npcs_[ind]->row(),npcs_[ind]->col()));
			path.push_back(std::pair<size_t,size_t>(size_t(npcs_[ind]->row()+2*((r%2)*2-1)*(r/2)),size_t(npcs_[ind]->col()+2*((r%2)*2-1)*(1-(r/2)))));
		}
		npcs_[ind]->set_path(path);
	}
}

/**
 * Add the next player on the queue to this world.
 */
void World::next_join() {
	if(to_join_.empty() || available_.empty()) return;
	Player* p=to_join_.front();
	to_join_.pop();
	while(!p->connected) {
		if(to_join_.empty()) return;
		p=to_join_.front();
		to_join_.pop();
	}
	Character* ch;
	double dr,dc;
	for(std::vector<std::pair<size_t,size_t> >::const_iterator pos=map_.spawn().begin(); pos!=map_.spawn().end(); ++pos) {
		if((ch=chars_.nearest((*pos).first,(*pos).second))!=0) {
			dr=double((*pos).first)-ch->row();
			dc=double((*pos).second)-ch->col();
			if(dr*dr+dc*dc<net::kTolerance) continue;
		}
		p->set_pos((*pos).first,(*pos).second);
		break;
	}
	net::index_t ind=available_.front();
	available_.pop();
	chars_.insert(p);
	players_.push_back(p);
	p->index=ind;
	charlist_[ind]=p;
	send_names(p);
	name_changed(p);
}

/**
 * Add a player to the queue for joining the world.
 * \param p pointer to player
 */
void World::join(Player* p) {
	to_join_.push(p);
}

/**
 * Remove a Player from the World.
 * \param p pointer to the player to be removed
 */
void World::disconnect(Player* p) {
	// remove p from players_, preserving list-like structure (nogaps)
	players_.erase(std::remove(players_.begin(),players_.end(),p));
	
	chars_.erase(p);

	send_disconnect(p->index);
	available_.push(p->index);

}

/**
 * Get the map hash.
 * \returns map hash
 */
net::maphash_t World::maphash() {
	return maphash_;
}

/**
 * Get the map name.
 * \returns map name
 */
const std::string& World::mapname() {
	return mapname_;
}

/**
 * Get the map width.
 * \returns map width
 */
size_t World::width() const {
	return map_.width();
}

/**
 * Get the map height.
 * \returns map height
 */
size_t World::height() const {
	return map_.height();
}
