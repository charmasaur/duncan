#include "gamestate.h"

using namespace client;

/**
 * Initialise members.
 */
GameState::GameState() : rows_(10), cols_(10),chat_(100,10), state_(State::NOTHING), chatting_(false), dl_(this), move_direction_(0), pathfinder_(world_) {
}

/**
 * Set the dimensions of the window.
 * \param r number of rows
 * \param c number of columns
 */
void GameState::set_dim(unsigned int r, unsigned int c) {
	rows_=r;
	cols_=c;
	chat_.reset(100,cols_-2);
	worldwin_.create(rows_-kMaxMessages-4,cols_,0,0);
	chatwin_.create(kMaxMessages+3,cols_,rows_-kMaxMessages-4,0);
	dlwin_.create(1,cols_,rows_-1,0);
}

/**
 * Resize the windows.
 * \param r number of rows
 * \param c number of columns
 */
void GameState::resize(unsigned int r, unsigned int c) {
	set_dim(r,c);
}

/**
 * Set the IP address of the server.
 * \param str IP address of the server
 */
void GameState::set_ip(const std::string& str) {
	ip_=str;
}

/**
 * Set the player's name.
 * \param str name of player
 */
void GameState::set_name(const std::string& str) {
	name_=str;
}

/**
 * Add a new message to the list of messages.
 * \param msg message to add
 */
void GameState::new_message(const std::string& msg) {
	messages_.push_front(msg);
	if(messages_.size()>kMaxMessages) messages_.pop_back();
	chatwin_.set_changed(true);
}

/**
 * Send a chat message.
 */
void GameState::send_chat() {
	if(con_) {
		net::data_t buf[1+chat_.print().length()+1];
		buf[0]=net::mnum::chat;
		std::memcpy(buf+1,chat_.print().c_str(),chat_.print().length()+1);
		con_->send(buf,1+chat_.print().length()+1);
	}
}

/**
 * Update the direction in which the player is moving,
 * and send a message to the server if necessary. 
 * \param dir direction in which to move (or stop moving)
 * \param start whether to start (as opposed to stop) moving
 */
void GameState::move(net::dir_t dir, bool start) {
	if((bool)(move_direction_ & dir)==start) return;
	move_direction_^=dir;
	net::data_t buf[1+sizeof(move_direction_)];
	buf[0]=net::mnum::direction;
	std::memcpy(buf+1,&move_direction_,sizeof(move_direction_));
	con_->send(buf,1+sizeof(move_direction_));
}

/**
 * Handle key presses.
 * \param wh pressed key
 * \param repeat whether the keypress was due to auto-repeat
 */
void GameState::on_press(unsigned int wh, bool repeat) {
	if(chatting_) {
		if(wh==Input::kEscape) chatting_=false;
		else if(chat_.new_char(wh) && chat_.input().length()>0) {
			send_chat();
			chat_.reset();
			chatting_=false;
		}
		chatwin_.set_changed(true);
	}
	else {
		if(wh==Input::kEscape) state_=State::POP_ME;
		if(wh=='t') chatting_=true;
		if(!repeat) {
			if(wh==Input::kUp) move(net::direction::up,true);
			else if(wh==Input::kDown) move(net::direction::down,true);
			else if(wh==Input::kLeft) move(net::direction::left,true);
			else if(wh==Input::kRight) move(net::direction::right,true);
		}
		if(wh=='c') {
			net::data_t buf;
			buf=net::mnum::morespam;
			con_->send(&buf,1,0,true);
		}
	}
}

/**
 * Handle key releases.
 * \param wh released key
 * \param repeat whether the key release was due to auto-repeat
 */
void GameState::on_release(unsigned int wh, bool repeat) {
	if(!chatting_ && !repeat) {
		if(wh==Input::kUp) move(net::direction::up,false);
		else if(wh==Input::kDown) move(net::direction::down,false);
		else if(wh==Input::kLeft) move(net::direction::left,false);
		else if(wh==Input::kRight) move(net::direction::right,false);
	}
}

/**
 * Handle any packets sent from the server.
 * \param buf packet data
 * \param size size (in bytes) of the packet
 * \param peer peer id of server (should be 0)
 */
void GameState::on_packet(net::data_t* buf, size_t size, net::index_t peer) {
	if(buf[0]==net::mnum::chat) {
		net::index_t ind;
		std::memcpy(&ind,buf+1,sizeof(ind));
		new_message(world_.char_name(ind)+std::string(": ")+std::string((char*)(buf+1+sizeof(ind))));
	}
	else if(buf[0]==net::mnum::playernames) {
		// names of some players in the world
		size_t pos=1;
		net::index_t ind;
		std::string str;
		while(pos<size) {
			std::memcpy(&ind,buf+pos,sizeof(ind)); pos+=sizeof(ind);
			str=std::string((char*)(buf+pos)); pos+=sizeof(char)*(str.length()+1);
			world_.add_char(ind,str);
			if(str==name_) world_.set_follow(ind);
		}
	}
	else if(buf[0]==net::mnum::playerpositions) {
		// positions of some players in the world
		world_.reset_visible();
		size_t pos=1;
		net::index_t ind;
		net::pos_t cpos[2];
		while(pos<size) {
			std::memcpy(&ind,buf+pos,sizeof(ind)); pos+=sizeof(ind);
			std::memcpy(cpos,buf+pos,2*sizeof(cpos[0])); pos+=2*sizeof(cpos[0]);
			world_.set_char_pos(ind,cpos[0],cpos[1]);
			world_.add_visible(ind);
		}
		worldwin_.set_changed(true);
	}
	else if(buf[0]==net::mnum::playerdisconnected) {
		net::index_t ind;
		std::memcpy(&ind,buf+1,sizeof(ind));
		new_message(world_.char_name(ind)+" disconnected");
		world_.remove_char(ind);
	}
	else if(buf[0]==net::mnum::spam) { // temporary for testing
		std::ostringstream oss;
		oss << "YOU JUST GOT SPAMMED (" << int(buf[1]) << ')';
		new_message(oss.str());
	}
	else if(buf[0]==net::mnum::curlurl) {
		dl_.set_url((char*)(buf+1));
	}
	else if(buf[0]==net::mnum::canchange) {
		size_t pos=1;
		next_world_=std::string((char*)(buf+pos)); pos+=sizeof(char)*(next_world_.length()+1); 
		std::memcpy(&next_hash_,buf+pos,sizeof(next_hash_));
		if(!check_hash()) {
			if(!dl_.downloading()) {
				dl_.start_download(next_world_+".map","data/"+next_world_+".map");
				new_message("Downloading "+next_world_);
			}
		}
		else request_change_world();
	}
	else if(buf[0]==net::mnum::changeworld) {
		world_.load("data/"+next_world_+".map");
		worldwin_.set_changed(true);
		new_message("Changed world");
	}
	else if(buf[0]==net::mnum::path) {
		// index to ignore
		net::index_t ind;
		std::memcpy(&ind,buf+1,sizeof(ind));

		// coordinates
		net::pos_t cpos[4];
		std::memcpy(cpos,buf+1+sizeof(ind),sizeof(cpos[0])*4);
		PathFinder::path_t path;
		pathfinder_.path(cpos[0],cpos[1],cpos[2],cpos[3],ind,path);
		std::ostringstream oss;oss<<"finding path ("<<cpos[0]<<','<<cpos[1]<<") -> ("<<cpos[2]<<','<<cpos[3]<<')';new_message(oss.str());
		net::data_t buf2[1+sizeof(cpos[0])*path.size()*2];
		buf2[0]=net::mnum::path;
		size_t pos=1;
		std::memcpy(buf2+pos,&ind,sizeof(ind)); pos+=sizeof(ind);
		for(PathFinder::path_t::iterator iter=path.begin(); iter!=path.end(); ++iter) {
			cpos[0]=(*iter).first; cpos[1]=(*iter).second;
			std::memcpy(buf2+pos,cpos,sizeof(cpos[0])*2);
			pos+=sizeof(cpos[0])*2;
		}
		con_->send(buf2,1+sizeof(ind)+sizeof(cpos[0])*path.size()*2,0,true);
	}
}

/**
 * Check whether the hash for data/next_world_.map matches next_hash_
 * \returns whether or not the hashes match
 */
bool GameState::check_hash() {
	return next_hash_==DrawableMap::hash("data/"+next_world_+".map");
}

/**
 * Display the progress of any downloads.
 * \param dlnow current number of bytes downloaded
 * \param dltot total size of file to be downloaded
 */
void GameState::on_progress(double dlnow, double dltot) {
	dlwin_.set_changed(true);
}

/**
 * Request that the client is moved to a new world.
 */
void GameState::request_change_world() {
	net::data_t buf[1]; 
	buf[0]=net::mnum::requestchangeworld;
	con_->send(buf,1,0,true);
}

/** 
 * Handle a successful connection to the server.
 * \param peer peer id of server
 */
void GameState::on_connect(net::index_t peer) {
	new_message("Connected!");
}

/**
 * Handle a disconnection from the server.
 * \param peer peer id of server
 */
void GameState::on_disconnect(net::index_t peer) {
	state_=State::POP_ME;
}

/**
 * Attempt to connect to the server when this state is pushed.
 */
void GameState::on_push() {
	state_=State::NOTHING;
	curs_set(0);
	world_.reset();
	worldwin_.set_changed(true);
	chatwin_.set_changed(true);
	dlwin_.set_changed(true);
	try {
		con_=new net::Connection(ip_,this);
	} catch(const std::runtime_error& e) {
		state_=State::POP_ME;
		con_=0;
		return;
	}
	// send the player's name to the server
	net::data_t buf[1+name_.length()+1];
	buf[0]=net::mnum::name;
	std::memcpy(buf+1,name_.c_str(),name_.length()+1);
	con_->send(buf,1+name_.length()+1,0,true);
}

/**
 * Disconnect from the server when this state is popped.
 */
void GameState::on_pop() {
	delete con_;
	con_=0;
	curs_set(1);
	dl_.abort_download();
}

/**
 * Set action to default.
 */
void GameState::on_active() {
	state_=State::NOTHING;
	curs_set(0);
	worldwin_.set_changed(true);
	chatwin_.set_changed(true);
}

/**
 * Update the connection in order to receive any packets.
 */
State::Action GameState::update() {
	if(con_) con_->receive();
	if(dl_.downloading()) 
		if(!dl_.update()) {
			new_message("Download finished");
			dlwin_.set_changed(true);
			if(check_hash()) request_change_world();
			else new_message("Download invalid");
		}
	return state_;
}

/**
 * Draw the world and the chat.
 */
void GameState::draw() {
	if(worldwin_.changed()) {
		worldwin_.set_changed(false);
		worldwin_.clear();
		world_.draw(worldwin_);
		worldwin_.refresh();
	}
	if(chatwin_.changed()) {
		chatwin_.set_changed(false);
		chatwin_.clear();
		draw_chat();
		chatwin_.refresh();
	}
	if(dlwin_.changed()) {
		dlwin_.set_changed(false);
		dlwin_.clear();
		if(dl_.downloading()) draw_dl();
		dlwin_.refresh();
	}
}

/**
 * Draw the chat.
 */
void GameState::draw_chat() {
	int ct=0;
	box(chatwin_.win(),0,0);
	for(std::list<std::string>::iterator iter=messages_.begin(); iter!=messages_.end(); ++iter) 
		mvwprintw(chatwin_.win(),kMaxMessages-ct++,1,"%s",(*iter).c_str());
	mvwprintw(chatwin_.win(),kMaxMessages+1,1,"%s",chat_.print().c_str());
}

/**
 * Draw the download status.
 */
void GameState::draw_dl() {
	int pos=(dl_.dlcur()/dl_.dlsize())*(double)dlwin_.width();
	for(int i=0;i<pos;i++) mvwprintw(dlwin_.win(),0,i,"=");
	mvwprintw(dlwin_.win(),0,dlwin_.width()/2-7,"%.2fMB/%.2fMB",dl_.dlcur()/1000000.0,dl_.dlsize()/1000000.0);
}
