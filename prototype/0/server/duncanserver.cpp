/**
 * Create a server object, and update it until it asks to quit.
 * Also create a process to read input from the console, and send
 * this to the server as it is entered.
 *
 * \author Harry Slatyer
 * \author Malcolm Gill
 *
 * \copyright see LICENSE
 */

bool g_serverpath;

#include <csignal>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <iostream>
#include "server.h"

namespace server_input {
	int pipefd[2];

	/** 
	 * Close the child's end of the pipe and kill the process
	 * if the parent is quitting.
	 * \param sig the signal
	 */
	void killchild(int s) {
		if(s==SIGTERM) {
			close(pipefd[1]);
			exit(0);
		}
	}
}

using namespace server_input;

int main(int argc, char ** argv) {
	signal(SIGTERM,killchild);
	std::string msg;
	int id;
	if(pipe(pipefd)!=0) return 0;
	if((id=fork())==0) {
		// the child process
		close(pipefd[0]);
		while(1) {
			getline(std::cin,msg);
			write(pipefd[1],msg.c_str(),msg.length()+1);
		}
		close(pipefd[1]);
		return 0;
	}
	else {
		// the parent process
		if(argc<2) { 
			std::cout << "Usage: " << argv[0] << " serverpath" << std::endl; 
			close(pipefd[1]);
			close(pipefd[0]);
			kill(id,SIGTERM);
			return 1;
		}
		g_serverpath=atoi(argv[1]); 
		close(pipefd[1]);
		int flags=fcntl(pipefd[0],F_GETFL,0);
		fcntl(pipefd[0],F_SETFL,flags|O_NONBLOCK);
		srand(time(0));
		server::Server server;
		char ch;
		while(server.update()) {
			while(read(pipefd[0],&ch,1)>0) {
				if(ch) msg.push_back(ch);
				else if(msg.length()) {
					server.message(msg);
					msg.clear();
				}
			}
		}
		close(pipefd[0]);
		kill(id,SIGTERM);
	}
	return 0;
}
