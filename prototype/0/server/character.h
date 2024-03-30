#ifndef SERVER_CHARACTER_H_
#define SERVER_CHARACTER_H_

#include <string>
#include "movable.h"

namespace server {

/*!
 * Store information about a character.
 *
 * \author Harry Slatyer
 * \author Malcolm Gill
 *
 * \copyright see LICENSE
 */
class Character : public Movable {
	protected:
		std::string name_;
		double row_,col_;
	public:
		Character();
		virtual ~Character();

		void set_pos(double,double);
		void set_name(const std::string&);
		double row();
		double col();
		const std::string& name();
		virtual bool can_find_path();

		/**
		 * Uniquely identifies this Character within a World;
		 * the World takes all responsibility for setting/using this value.
		 */
		net::index_t index;
};

}

#endif
