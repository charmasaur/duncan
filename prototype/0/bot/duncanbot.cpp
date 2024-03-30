/*!
 * Create a bot object, and update it until it asks to quit.
 * Also create a process to read input from the console, and send
 * this to the bot as it is entered.
 * 
 * \author Harry Slatyer
 *
 * \copyright see LICENSE 
 */

#include <csignal>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "bot.h"

int main(int argc, char ** argv) {
    std::string msg;
	if(argc!=3) {
		std::cout << "Usage: duncanbot name ip\n";
		return 0;
	}
	srand(time(NULL));
    bot::Bot bot(argv[1],argv[2]);
    while(bot.update());
	std::cout << "dead\n";
    return 0;
}
