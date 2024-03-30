#include "server.h"

using namespace server;

const double Server::kSecondsPerUpdate=0.01;

/**
 * Initialise the connection, and set this object to be the listener.
 * \throw std::runtime_error if all worlds cannot be loaded
 */
Server::Server() : curl_url_("http://zeroclan.nexfiles.com/"), timer_(1), second_timer_(1000000), frame_count_(0), running_(true), windex_(0), con_(net::kPort) {
	std::ifstream input("data/worlds.hi");
	size_t numworlds=0;
	if(!input.fail()) {
		input.setf(std::ios::skipws);
		std::string name;
		input >> numworlds;
		for(size_t i=0;i<numworlds;i++) {
			input >> name;
			std::cout << "Loading " << name << "\n";
			world_ids_[name]=worlds_.size();
			worlds_.push_back(std::tr1::shared_ptr<World>(new World(name)));
		}
		input.close();
	}
	if(numworlds==0 || numworlds!=worlds_.size()) throw std::runtime_error("Could not load worlds");
	for(net::index_t i=0;i<net::kMaxUsers;i++) {
		players_.push_back(Player(i));
		players_[i].index=net::kMaxCharacters;
	}
	con_.set_listener(this);
}

/**
 * Receive a packet from a client.
 * \param buf packet data
 * \param size size (in bytes) of the packet
 * \param peer peer from whom the packet was sent
 */
void Server::on_packet(net::data_t* buf, size_t size, net::index_t peer) {
	if(size==0) return;
	// see if it's something that Server cares about (changing world, 
	// for example). if not, let the world handle it
	if(buf[0]==net::mnum::requestchangeworld) {
		if(!players_[peer].changeworld.length()) return;
		if(players_[peer].windex!=worlds_.size()) worlds_[players_[peer].windex]->disconnect(&players_[peer]);
		net::data_t buf=net::mnum::changeworld;
		con_.send(&buf,1,peer,true);
		players_[peer].windex=world_ids_[players_[peer].changeworld];
		players_[peer].changeworld="";
		worlds_[players_[peer].windex]->join(&players_[peer]);
		return;
	}
	else if(buf[0]==net::mnum::name) {
		players_[peer].set_name((char*)(buf+1));
		if(players_[peer].index!=net::kMaxCharacters) worlds_[0]->name_changed(&players_[peer]);
		return;
	}
	else if(buf[0]==net::mnum::morespam) {
		sendcanchange(peer,(players_[peer].windex+1)%worlds_.size());
		return;
	}
	if(players_[peer].windex==worlds_.size() || players_[peer].index==net::kMaxCharacters) return;
	worlds_[players_[peer].windex]->packet(buf,size,&players_[peer]);
}

/**
 * Handle a new connection.
 * \param peer newly connected peer
 */
void Server::on_connect(net::index_t peer) {
	std::cout << std::endl << peer << " connected";
	if(peer>=net::kMaxUsers) return;
	players_[peer].connected=true;
	players_[peer].windex=worlds_.size();
	// send her the curl URL
	net::data_t buf[1+curl_url_.length()+1];
	buf[0]=net::mnum::curlurl;
	std::memcpy(buf+1,curl_url_.c_str(),curl_url_.length()+1);
	con_.send(buf,2+curl_url_.length(),peer,true);
	// and send her the name and hash of the first world
	sendcanchange(peer,0);
}

/**
 * Tell a client they can change world.
 * \param peer index of peer
 * \param world id of world to which player can change
 */
void Server::sendcanchange(net::index_t peer, size_t world) {
	std::string map(worlds_[world]->mapname());
	net::maphash_t hash=worlds_[world]->maphash();
	players_[peer].changeworld=map;
	net::data_t buf[1+map.length()+1+sizeof(hash)];
	buf[0]=net::mnum::canchange;
	std::memcpy(buf+1,map.c_str(),map.length()+1);
	std::memcpy(buf+1+map.length()+1,&hash,sizeof(hash));
	con_.send(buf,2+map.length()+sizeof(hash),peer,true);
}

/**
 * Handle the disconnection of a client.
 * \param peer disconnected peer
 */
void Server::on_disconnect(net::index_t peer) {
	std::cout << std::endl << peer << " disconnected";
	if(players_[peer].windex!=worlds_.size()) worlds_[players_[peer].windex]->disconnect(&players_[peer]);
	players_[peer].connected=false;
	players_[peer].index=net::kMaxCharacters;
}

/**
 * Handle any messages entered into the server's console.
 * \param str string entered into console
 */
void Server::message(const std::string& str) {
	std::istringstream iss(str);
	std::string cmd;
	if(iss >> cmd) {
		if(cmd == "quit") running_=false;
		else if(cmd == "curl") {
			if(iss >> cmd) {
				curl_url_=cmd;
			}
			std::cout << "Curl URL is: " << curl_url_ << "\n";
		}
		else std::cout << "Console message: " << str << "\n";
	}
}

/**
 * Updates the connection and all worlds.
 * Sends any data to clients; per-frame data limit is set by net::kMaxFrameSize
 * and if this is exceeded before all worlds have had a chance to send their
 * data, the next frame will pick up where the last one left off.
 *
 * \returns whether or not the server should continue to run
 */
bool Server::update() {
	double dt=timer_.elapsed()*0.000001;
	static unsigned long packet_count=0;
	if(dt<kSecondsPerUpdate) {
		usleep(1000000.0*(kSecondsPerUpdate-dt));
		dt=kSecondsPerUpdate;
	}
	else std::cout << "slow!\n";
	timer_.reset();
	if(second_timer_.elapsed()>=kSecondsPerInfoPrint) {
		second_timer_.reset();
		std::cout << "FPS: " << double(frame_count_)/double(kSecondsPerInfoPrint) << ", Send speed: " << double(con_.bytes_sent())/double(kSecondsPerInfoPrint*1000) << "kb/s, Receive speed: " << double(con_.bytes_received())/double(kSecondsPerInfoPrint*1000) << "kb/s, " << double(packet_count)/double(kSecondsPerInfoPrint) << " packets/s\n";
		frame_count_=0;
		packet_count=0;
	}

	// get data
	con_.receive();

	// update worlds
	for(size_t i=0; i<worlds_.size(); i++) {
		worlds_[i]->update(dt);
	}

	// send as much data as allowed
	// not an optimal solution (knapsack problem i think?)
	// just stops when a packet is encountered that won't fit, to continue next frame
	size_t bytes_remaining = net::kMaxFrameSize;
	for(size_t i=0; i<worlds_.size() && bytes_remaining>0; i++,windex_=(windex_+1)%worlds_.size()) {
		while(const PlayerPacket::pointer_t ppp = worlds_[windex_]->next_outgoing()) {
			if(ppp->packet->size > bytes_remaining) {
				// next packet too big: wait until next frame
				// this could maybe be improved easily but i'm not sure
				bytes_remaining = 0; // will cause for loop to finish
				break; // will leave while loop
			}
			if(ppp->player->connected) {
				con_.send(ppp->packet->data, ppp->packet->size, ppp->player->enet_index(), ppp->packet->reliable);
				bytes_remaining -= ppp->packet->size;
				packet_count++;
			}
			worlds_[windex_]->pop_outgoing();
		}
	}
	frame_count_++;
	// might be better to flush connection here rather than after every individual call to send

	return running_;
}
