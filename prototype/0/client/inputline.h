#ifndef CLIENT_INPUTLINE_H_
#define CLIENT_INPUTLINE_H_

#include <string>
#include "input.h"

namespace client {

/*!
 * Store and update a line of text input, and handle special characters (such
 * as backspace and enter) when they are typed.
 *
 * \author Harry Slatyer
 *
 * \copyright see LICENSE
 */
class InputLine {
	private:
		size_t maxlen_, cursorpos_, boxstart_, boxwidth_;
		std::string input_,print_;

		void insert_char(unsigned int);
		void remove_char();
	public:
		InputLine(size_t mlen=16, size_t bwidth=16);
		
		bool new_char(unsigned int);
		void reset(size_t mlen=0, size_t bwidth=0);
		void set_input(const std::string&);
		size_t cursorpos();
		std::string& input();
		std::string& print();
};

}

#endif
