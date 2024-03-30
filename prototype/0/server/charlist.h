#ifndef SERVER_CHARLIST_H_
#define SERVER_CHARLIST_H_

#include <vector>
#include <functional>
#include <utility>
#include "character.h"
#include "kdtree++/kdtree.hpp"

namespace server {

/*!
 * Maintain a list of characters in such a way that all characters inside
 * a given rectangle can be found quickly.
 *
 * \author Harry Slatyer
 * \author Malcolm Gill
 * 
 * \copyright see LICENSE
 */
template <typename T> class CharList {
	public:
		typedef std::pair<T*,std::pair<double,double> > node_t;
	private:
	//	typedef std::pair<T*,std::pair<double,double> > node_t;
		typedef KDTree::KDTree<2,node_t,std::pointer_to_binary_function<node_t,size_t,double> > tree_t;
		tree_t tree_;
		//mutable T* dummy_;

		inline node_t make_node(T* c) const {
			return std::make_pair(c,std::make_pair(c->row(),c->col()));
		}
		inline node_t make_node(double r, double c) const {
			return std::make_pair((T*)0,std::make_pair(r,c));
		}
	public:
		typedef typename tree_t::const_iterator const_iterator;

		/**
		 * This is a constructor. It constructs.
		 */
		CharList() : tree_(std::ptr_fun(acc)) {
		//	dummy_=new T();
		//	dummy_->set_name("Dummy (tc)");
		}

		/**
		 * Add an element to the tree.
		 * \param c pointer to element
		 */
		void insert(T* c) {tree_.insert(make_node(c));}

		/**
		 * Remove an element from the list.
		 * \param c pointer to element
		 */
		void erase(T* c) {tree_.erase_exact(make_node(c));}

		/**
		 * Rebuilds the tree.
		 */
		void optimise() {tree_.optimise();}

		/**
		 * Get an iterator pointing to the first element of the tree.
		 * \returns iterator pointing to the start
		 */
		const_iterator begin() {return tree_.begin();}

		/**
		 * Get an iterator pointing to the last element of the tree.
		 * \returns iterator pointing to the end
		 */
		const_iterator end() {return tree_.end();}

		/**
		 * Get all characters inside a square.
		 * \param rc row of center
		 * \param cc col of center
		 * \param r `radius' of square
		 * \param v vector to fill with characters
		 */
		void square(double rc,double cc,double r,std::vector<node_t>& v) const {
		//	dummy_->set_pos(rc,cc);
			tree_.find_within_range(make_node(rc,cc),r,std::back_inserter(v));	
		}

		/** 
		 * Get all characters inside a square centered at a certain character.
		 * \param c center character
		 * \param r `radius' of square
		 * \param v vector to fill with characters
		 */
		void square(T* c,double r,std::vector<node_t>& v) const {
			tree_.find_within_range(make_node(c),r,std::back_inserter(v));
		}

		/**
		 * Get the character closest to a certain point.
		 * \param r row of point
		 * \param c column of point
		 * \returns pointer to nearest character
		 */
		T* nearest(double r, double c) {
			//dummy_->set_pos(r,c);
			typename tree_t::iterator iter=tree_.find_nearest(make_node(r,c)).first;
			return (iter==tree_.end()?0:(*iter).first);
		}

		/**
		 * Updates the position of the specified element in the list.
		 * \param e pointer to element
		 * \param r new row
		 * \param c new col
		 */
		void move(T* e, double r, double c) {
			erase(e);
			e->set_pos(r,c);
			insert(e);
		}

		/**
		 * Get a particular coordinate of a character's position.
		 * \parram c pointer to character
		 * \param k coordinate index (0 or 1)
		 * \returns kth coordinate of character c's position
		 */
		static inline double acc(node_t c,size_t k) {return (k?c.second.second:c.second.first);}
};

}

#endif
