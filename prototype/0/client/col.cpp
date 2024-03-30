#include "col.h"
void client::col::init() {
	init_pair(kWall, COLOR_BLUE, COLOR_BLUE);
	init_pair(kGrass, COLOR_GREEN, COLOR_GREEN);
	init_pair(kCharacter, COLOR_YELLOW, COLOR_MAGENTA);
}
