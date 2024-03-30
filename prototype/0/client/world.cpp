#include "world.h"

using namespace client;

World::World() : follow_(net::kMaxCharacters) {
}

/** 
 * Load the map.
 * \param str filename of map
 */
bool World::load(const std::string& str) {
	reset();
	return map_.load(str);
}

/**
 * Reset the world.
 */
void World::reset() {
	map_.reset();
	follow_=net::kMaxCharacters;
	chars_.clear();
	visible_.clear();
}

/**
 * Add a new character.
 * \param ch index of the character
 * \param name name of the character
 */
void World::add_char(net::index_t ch, const std::string& name) {
	if(ch>=net::kMaxCharacters) return;
	charlist_.at(ch).set_name(name);
	chars_.insert(ch);
}

/**
 * Remove a character.
 * \param ch index of character
 */
void World::remove_char(net::index_t ch) {
	chars_.erase(ch);
}

/**
 * Reset the list of visible characters.
 */
void World::reset_visible() {
	visible_.clear();
}

/**
 * Add a character to the list of visible characters.
 * \param ch index of character
 */
void World::add_visible(net::index_t ch) {
	if(ch>=net::kMaxCharacters) return;
	visible_.push_back(ch);
}

/**
 * Set the position of a character.
 * \param ch index of character
 * \param r row
 * \param c column
 */
void World::set_char_pos(net::index_t ch, size_t r, size_t c) {
	if(ch>=net::kMaxCharacters) return;
	charlist_.at(ch).set_pos(r,c);
}

/**
 * Set the world view to center on a particular character.
 * \param ch the character on whom to center the view
 */
void World::set_follow(net::index_t ch) {
	if(chars_.count(ch)>0) follow_=ch;
	else follow_=net::kMaxCharacters;
}

/** 
 * Get the name of a character.
 * \param ch index of character
 * \returns name of character
 */
const std::string& World::char_name(net::index_t ch) {
	if(ch>=net::kMaxCharacters) return charlist_.at(0).name();
	return charlist_.at(ch).name();
}

/** 
 * Draw the world. 
 * \param wnd window in which to draw the world
 */
void World::draw(NCWindow& wnd) {
	int firstr, firstc;
	if(follow_==net::kMaxCharacters) firstr=firstc=0;
	else {
		firstr=((int)charlist_.at(follow_).row())-(int)wnd.height()/2;
		firstc=((int)charlist_.at(follow_).col())-(int)wnd.width()/2;
	}
	map_.draw(wnd,firstr,firstc);
	wattron(wnd.win(),COLOR_PAIR(col::kCharacter));
	for(std::list<net::index_t>::iterator iter=visible_.begin(); iter!=visible_.end(); ++iter) 
		mvwaddch(wnd.win(),charlist_.at(*iter).row()-firstr,charlist_.at(*iter).col()-firstc,charlist_.at(*iter).name().at(0));
	wattroff(wnd.win(),COLOR_PAIR(1));
}

/**
 * Get the width of the map.
 * \returns width of map
 */
size_t World::width() {
    return map_.width();
}

/**
 * Get the height of the map.
 * \returns height of map
 */
size_t World::height() {
    return map_.height();
}

/**
 * \return true iff the tile at the specified ccordinate is not occupied
 * by anything that would prevent a player from moving there (namely a solid
 * map tile or a visible character).
 * \param r row of tile
 * \param c column of tile
 * \param igind index of character to ignore
 */
bool World::is_vacant(size_t r, size_t c, net::index_t igind) {
	if(r>=height() || c>=width()) return false;
	if(!map_.is_vacant(r,c)) return false;
	for(std::list<net::index_t>::iterator iter=visible_.begin();iter!=visible_.end();++iter) 
		if(*iter!=igind && std::abs(int(charlist_.at(*iter).row())-int(r))<=1 && std::abs(int(charlist_.at(*iter).col())-int(c))<=1) return false;
	return true;
}
