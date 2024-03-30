#ifndef SERVER_MAP_H_
#define SERVER_MAP_H_

#include <vector>
#include <queue>
#include <utility>
#include "../map/map.h"

namespace server {

/*!
 * Store information about a map, including a list of spawn points.
 * \author Harry Slatyer
 *
 * \copyright see LICENSE
 */
class ServerMap : public map::Map {
	private:
		std::vector<std::pair<size_t,size_t> > spawn_;
	public:
		ServerMap(const std::string&);
		bool load(const std::string&);

		const std::vector<std::pair<size_t,size_t> >& spawn();
};

} // namespace

#endif
