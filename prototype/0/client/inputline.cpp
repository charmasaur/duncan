#include "inputline.h"

using namespace client;

/**
 * Initialise members.
 * \param mlen maximum length of input string
 * \param bwidth width of box into which text is being entered
 */
InputLine::InputLine(size_t mlen, size_t bwidth) : maxlen_(mlen), cursorpos_(0), boxstart_(0), boxwidth_(bwidth) {
}

/**
 * Insert a character into the string.
 * \param ch character to be inserted
 */
void InputLine::insert_char(unsigned int ch) {
	input_.insert(boxstart_+cursorpos_,(char*)&ch,1);
	if(cursorpos_==boxwidth_-1) boxstart_++;
	else cursorpos_++;
}

/**
 * Remove a character from the string.
 */
void InputLine::remove_char() {
	if(!cursorpos_ && !boxstart_) return;
	input_.erase(boxstart_+cursorpos_-1,1);
	if(!cursorpos_) boxstart_--;
	else {
		if(boxstart_ && boxstart_+boxwidth_-1==input_.length()+1) boxstart_--;
		else cursorpos_--;
	}
}

/**
 * Insert a new 'character' (possibly backspace, enter, left, right, etc..)
 * into the string
 * \param ch character to be inserted
 * \returns whether or not 'enter' was pressed
 */
bool InputLine::new_char(unsigned int ch) {
	if(ch==Input::kBackspace || ch=='\b') remove_char();
	else if(ch==Input::kLeft) {
		if(cursorpos_==0 && boxstart_) boxstart_--;
		else if(cursorpos_) cursorpos_--;
	}
	else if(ch==Input::kRight) {
		if(boxstart_+cursorpos_<input_.length()) {
			if(cursorpos_==boxwidth_-1) boxstart_++;
			else cursorpos_++;
		}
	}
	else if(ch=='\0' || ch=='\n' || ch=='\r' || ch==Input::kEnter) return true;
	else if(input_.length()<maxlen_-1) {
		if(ch==Input::kSpace) ch=' ';
		if(ch>=' ' && ch<='~') insert_char(ch);
	}
	return false;
}

/**
 * Reset the line of input, changing the maximum length
 * and box size if necessary.
 * \param mlen maximum length of input string
 * \param bwidth width of box into which text is entered
 */
void InputLine::reset(size_t mlen, size_t bwidth) {
	if(mlen) maxlen_=mlen;
	if(bwidth) boxwidth_=bwidth;
	cursorpos_=boxstart_=0;
	input_.clear();
}

/**
 * Set the initial string.
 * \param str initial string
 */
void InputLine::set_input(const std::string& str) {
	if(str.length()>maxlen_) return;
	input_=str;
	cursorpos_=std::min(input_.length(),boxwidth_-1);
	boxstart_=input_.length()-boxstart_;
}

/**
 * Get the position of the cursor.
 * \returns cursor position
 */
size_t InputLine::cursorpos() {
	return cursorpos_;
}

/**
 * Get the full input string.
 * \returns input string
 */
std::string& InputLine::input() {
	return input_;
}

/**
 * Get the substring visible in the text box.
 * \returns visible substring
 */
std::string& InputLine::print() {
	return (print_=input_.substr(boxstart_,std::min(boxwidth_,input_.length()-boxstart_)));
}
