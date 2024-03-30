#include <iostream>
#include <fstream>
#include <string>
#include "../network/network.h"


int main(int argc, char ** argv) {
	if(argc<=1) return 0;
	std::ifstream input(argv[1]);
	if(input.fail()) return 0;
	input.setf(std::ios::skipws);
	int width_,height_;
	if(!(input >> width_ >> height_)) return 0;
	net::maphash_t hash=1;
	char tile;
	for(size_t i=0;i<width_;i++) 
		for(size_t j=0;j<height_;j++) {
			if(!(input >> tile)) return 0;
			hash+=tile*13;
			hash%=13371337;
		}
	std::cout << hash << "\n";
	input.close();
}
