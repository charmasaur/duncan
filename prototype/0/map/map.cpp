#include "map.h"

using namespace map;

Map::Map() : width_(0), height_(0), hash_(0) {
}

/**
 * Reset the map.
 */
void Map::reset() {
	width_=height_=0;
	hash_=0;
	map_.clear();
}

/**
 * Load the map from a file.
 * \param str name of the file
 * \throw std::runtime_error if map cannot be loaded
 */
Map::Map(const std::string& str) : width_(0), height_(0) {
	if(!load(str)) throw std::runtime_error("Could not open map "+str);
}

/**
 * Load the map from a file.
 * \param str name of the file
 * \returns whether or not the map loaded successfully
 */
bool Map::load(const std::string& str) {
	std::ifstream input(str.c_str());
	if(input.fail()) return false;
	map_.clear();
	input.setf(std::ios::skipws);
	input >> width_ >> height_;
	char tile;
	for(size_t i=0;i<width_;i++) 
		for(size_t j=0;j<height_;j++) {
			input >> tile;
			map_.push_back(tile);
		}
	input.close();
	hash_=hash(width_,height_,map_);
	return true;
}

/**
 * Get the width of the map.
 * \returns width of map
 */
size_t Map::width() const {
	return width_;
}

/**
 * Get the height of the map.
 * \returns height of map
 */
size_t Map::height() const {
	return height_;
}

/**
 * Get the hash of the a certain map (0 if it doesn't exist).
 * \param str filename of the map
 * \returns hash of the map
 */
net::maphash_t Map::hash(const std::string& str) {
    std::ifstream input(str.c_str());
    if(input.fail()) return 0;
    input.setf(std::ios::skipws);
    size_t w,h;
    if(!(input >> w >> h)) return 0;
	std::vector<char> tiles; 
    char tile;
    for(size_t i=0;i<w;i++)
        for(size_t j=0;j<h;j++) {
            if(!(input >> tile)) return 0;
        	tiles.push_back(tile);
		}
    return hash(w,h,tiles);
}

/**
 * Get the hash of a map.
 * \param w width
 * \param h height
 * \param tiles map data
 * \returns hash of the map
 */
net::maphash_t Map::hash(size_t w, size_t h, const std::vector<char>& tiles) {
	net::maphash_t nhash=1;
	for(size_t i=0;i<w;i++)
		for(size_t j=0;j<h;j++) nhash=(nhash+tiles[j*w+i]*13)%13371337;
	return nhash;
}

/**
 * Get the hash of this map.
 * \returns hash of the map
 */
net::maphash_t Map::hash() {
	return hash_;
}

/**
 * \return true iff the specified tile on the map can be traversed
 * \param r row
 * \param c column
 */
bool Map::is_vacant(size_t r, size_t c) const {
	return tile(r,c)!='#';
}
