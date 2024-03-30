#ifndef SERVER_PATHFINDER_H_
#define SERVER_PATHFINDER_H_

#include <queue>
#include <vector>
#include <list>
#include <utility>
#include "pathworld.h"

namespace server {

/*!
 * Find a path between two points in a world.
 *
 * \author Harry Slatyer
 *
 * \copyright see LICENSE
 */
class PathFinder {
	public:
		typedef std::list<std::pair<size_t,size_t> > path_t;
	private:
		typedef std::pair<bool,size_t> node_t;
		const PathWorld* world_;
		int dirs_[4][2];
		std::vector<node_t> map_;
		std::queue<size_t> queue_;

		void getpath(size_t,path_t&);
		void cleanpath(path_t&);

		void newmap();
		inline size_t encode(size_t,size_t);
		inline size_t row(size_t);
		inline size_t col(size_t);
	public:
		PathFinder(const PathWorld*);
		
		bool path(size_t, size_t, size_t, size_t, Character*, path_t&);
};

}

#endif
