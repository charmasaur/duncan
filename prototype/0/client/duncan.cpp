/*!
 * Create a client object, and update it until it asks to quit.
 * 
 * \author Harry Slatyer
 * \author Malcolm Gill
 *
 * \copyright see LICENSE
 */

#include "client.h"

using namespace client;

int main() {
	Client client;
	while(client.update());
	return 0;
}
