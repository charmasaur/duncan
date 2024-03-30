#include "map.h"

using namespace server;

/**
 * Load the map from a file.
 * \param str name of the file
 * \throw std::runtime_error if map cannot be loaded
 */
ServerMap::ServerMap(const std::string& str) {
	if(!load(str)) throw std::runtime_error("Could not open map"+str);
}

/**
 * Load the map from a file. 
 * \param str name of the file
 * \returns whether or not the map loaded successfully
 */
bool ServerMap::load(const std::string& str) {
	if(!Map::load(str)) return false;
	std::priority_queue<std::pair<size_t, std::pair<size_t,size_t> > > sp;
	for(size_t r=0;r<height_;r++) 
		for(size_t c=0;c<width_;c++) 
			if(is_vacant(r,c)) sp.push(std::make_pair(rand()%(height_*width_),std::make_pair(r,c)));
	while(!sp.empty()) {
		spawn_.push_back(sp.top().second);
		sp.pop();
	}
	return true;
}

/**
 * Get a reference to the list of spawn points.
 * \returns reference to the list of spawn points
 */
const std::vector<std::pair<size_t,size_t> >& ServerMap::spawn() {
	return spawn_;
}
