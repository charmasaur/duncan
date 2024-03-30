#include "pathfinder.h"

using namespace client;

/**
 * Initialise the path finder.
 * \param world reference to world object
 */
PathFinder::PathFinder(World& world) : world_(world) {
	dirs_[0][0]=0; dirs_[0][1]=1;
	dirs_[1][0]=0; dirs_[1][1]=-1;
	dirs_[2][0]=1; dirs_[2][1]=0;
	dirs_[3][0]=-1; dirs_[3][1]=0;
	newmap();
}

/**
 * The world has a new map, so resize map_ accordingly.
 */
void PathFinder::newmap() {
	map_.resize(world_.width()*world_.height());
}

/**
 * Get the index in map_ of a particular tile.
 * \param r row
 * \param c column
 * \returns index in map_ of tile
 */
size_t PathFinder::encode(size_t r, size_t c) {
	return r*world_.width()+c;
}

/** 
 * Get the row of a tile with a particular index.
 * \param i index
 * \returns row of tile i
 */
size_t PathFinder::row(size_t i) {
	return i/world_.width();
}

/**
 * Get the column of a tile with a particular index.
 * \param i index
 * \returns column of tile i
 */
size_t PathFinder::col(size_t i) {
	return i%world_.width();
}

/**
 * After a path has been found, recursively insert it into a given path_t.
 * \param cur index of last tile in path
 * \param p path_t to fill with data
 */
void PathFinder::getpath(size_t cur, path_t& p) {
	p.push_front(std::make_pair(row(cur),col(cur)));
	if(map_[cur].second==map_.size()) return;
	getpath(map_[cur].second,p);
}

/**
 * Remove unnecessary waypoints (those that lie on a line between two other
 * waypoints) from a path
 * \param p path to clean
 */
void PathFinder::cleanpath(path_t& p) {
	path_t::iterator iter=p.begin(), piter;
	if(iter==p.end()) return;
	int dr(0),dc(0);
	size_t pr((*iter).first), pc((*iter).second), cr, cc;
	piter=iter++;
	for(;iter!=p.end();piter=iter++) {
		cr=(*iter).first; cc=(*iter).second;
		if(dr==int(cr)-int(pr) && dc==int(cc)-int(pc)) iter=p.erase(piter);
		dr=int(cr)-int(pr); 
		dc=int(cc)-int(pc);
		pr=cr;
		pc=cc;
	}
}

/**
 * BFS to find a path. 
 * \param r1 initial row
 * \param c1 initial column
 * \param r2 final row
 * \param c2 final column
 * \param ignore index of character to ignore
 * \param p reference to a path to be filled with data
 * \returns whether or not there exists a path from (r1,c1) to (r2,c2)
 */
bool PathFinder::path(size_t r1,size_t c1,size_t r2,size_t c2,net::index_t ignore,path_t& p) {
	size_t r,c,cur,nr,nc,ne;
	if(world_.width()*world_.height()!=map_.size()) newmap();
	p.clear();
	while(!queue_.empty()) queue_.pop();
	for(std::vector<node_t>::iterator iter=map_.begin();iter!=map_.end();++iter) {
		(*iter).first=false;
		(*iter).second=map_.size();
	}
	map_[encode(r1,c1)].first=true;
	queue_.push(encode(r1,c1));
	while(!queue_.empty()) {
		cur=queue_.front();
		queue_.pop();
		if(cur==encode(r2,c2)) {
			getpath(cur,p);
			cleanpath(p);
			return true;
		}
		r=row(cur);
		c=col(cur);
		for(size_t d=0; d<4; d++) {
			nr=r+dirs_[d][0]; nc=c+dirs_[d][1];
			if(nr>=world_.height() || nc>=world_.width()) continue;	
			if(map_[(ne=encode(nr,nc))].first) continue;
			map_[ne].first=true;
			if(!world_.is_vacant(nr,nc,ignore)) continue;
			map_[ne].second=cur;
			queue_.push(ne);
		}
	}
	return false;
}
