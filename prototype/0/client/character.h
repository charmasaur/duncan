#ifndef CLIENT_CHARACTER_H_
#define CLIENT_CHARACTER_H_

#include <string>

namespace client {

/*!
 * Store information about a character in the world.
 *
 * \author Harry Slatyer
 *
 * \copyright see LICENSE
 */
class Character {
	private:
		std::string name_;
		size_t row_,col_;
	public:
		Character();

		void set_pos(size_t,size_t);
		void set_name(const std::string&);
		size_t row();
		size_t col();
		const std::string& name();
};

} // namespace

#endif
