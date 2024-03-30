#ifndef CLIENT_WORLD_H_
#define CLIENT_WORLD_H_

#include <algorithm>
#include <set>
#include <list>
#include <iostream>
#include <tr1/array>
#include "map.h"
#include "ncwindow.h"
#include "character.h"
#include "../network/network.h"
#include "col.h"

namespace client {

/*!
 * Store the client's information about the world.
 * 
 * \author Harry Slatyer
 *
 * \copyright see LICENSE
 */
class World {
	private:
		DrawableMap map_;
		std::tr1::array<Character,net::kMaxCharacters> charlist_;//Character charlist_[net::kMaxCharacters];
		std::set<net::index_t> chars_;
		std::list<net::index_t> visible_;
		net::index_t follow_;
	public:
		World();

		bool load(const std::string&);

		void reset();

		void reset_visible();
		void add_visible(net::index_t);
		void set_char_pos(net::index_t, size_t, size_t);
		void add_char(net::index_t, const std::string&);
		void remove_char(net::index_t);
		void set_char_name(net::index_t, const std::string&);
		void set_follow(net::index_t);
		const std::string& char_name(net::index_t);
		void draw(NCWindow&);

		bool is_vacant(size_t, size_t, net::index_t);
		size_t width();
		size_t height();
};

}

#endif
