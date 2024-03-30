#include <iostream>
#include <tr1/memory>
#include <cstdlib>
#include <vector>
#include "../timer/timer.h"
#include "../server/charlist.h" // close enough to using KDTree directly i hope
using std::cout;
using std::endl;
using std::tr1::shared_ptr;
using timer::Timer;
using server::CharList;

struct Char {
	double row_,col_;
	double row() { return row_; }
	double col() { return col_; }
	const static double kMaxRow = 200;
	const static double kMaxCol = 200;
	Char() : row_(kMaxRow*rand()/RAND_MAX), col_(kMaxCol*rand()/RAND_MAX) {}
};
typedef CharList<Char> tree_t;
typedef shared_ptr<tree_t> tree_ptr;
typedef std::vector<tree_t::node_t> vec_t;

//! \returns optimised tree with n randomly positioned Chars
tree_ptr make_tree(size_t n) {
	tree_ptr t(new tree_t());
	for(size_t i=0; i<n; i++) {
		t->insert(new Char());
	}
	t->optimise();
	return t;
}

//! deletes pointers held in this tree
void free_tree(tree_ptr t) {
	for(tree_t::const_iterator it=t->begin(); it!=t->end(); ++it) {
		delete it->first;
	}
}

//! range searches in a 20*20 square at a random location in t
void range_search(tree_ptr t) {
	vec_t v;
	t->square(100,100,2,v);
}

int main() {
	srand(time(0));

	// timer with microsecond precision
	Timer timer(1);

	for(size_t n=0; n<100000; n+=1000) {
	//size_t n = 5000000;
		tree_ptr tree(make_tree(n));
		timer.reset();
		for(int i=0; i<20; i++)
			range_search(tree);
		cout << n << ' ' << timer.elapsed()/20. << "\n";
		free_tree(tree);
	}
	return 0;
}
