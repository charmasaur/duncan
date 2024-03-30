#ifndef MAP_MAP_H_
#define MAP_MAP_H_

#include <vector>
#include <fstream>
#include <stdexcept>
#include <string>
#include "../network/network.h"

namespace map {

/*!
 * Store information about a map, and draw it in a specified ncurses window.
 * \author Harry Slatyer
 * \author Malcolm Gill
 *
 * \copyright see LICENSE
 */
class Map {
	protected:
		size_t width_, height_;
		std::vector<char> map_;
		net::maphash_t hash_;

		/**
 	 	 * Get the index in map_ of a particular tile.
 	 	 * \param r row of tile
 	 	 * \param c column of tile
 	 	 * \returns index in map_ of tile
 	 	 */
		size_t encode(size_t r, size_t c) const {
    		return r*width_+c;
		}

		/**
		 * \param r row of tile
		 * \param c column of tile
		 * \returns the value of the tile at the specified coordinate.
		 */
		char tile(size_t r, size_t c) const {
			return map_[encode(r,c)];
		}

		static net::maphash_t hash(size_t,size_t,const std::vector<char>&);
	public:
		Map();
		Map(const std::string&);
		virtual ~Map() {}

		void reset();

		virtual bool load(const std::string&);

		bool is_vacant(size_t,size_t) const;

		size_t width() const;
		size_t height() const;

		net::maphash_t hash();

		static net::maphash_t hash(const std::string&);
};

} // namespace

#endif
