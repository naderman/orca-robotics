#include "../arcnetio.h"
#include <iostream>

using namespace mhipa10arc;
using namespace std;

int main (void)
{
	// Specify arcnet device
	ArcnetIO arcio1("arc0");

	try {

		arcio1.enable();

	} catch (const std::exception& e) {
		std::cout << "Test 1 failed: " << e.what() << std::endl;
		arcio1.disable();
		return -1;
	}
	
	arcio1.disable();
	std::cout << "Test 1 passed." << std::endl;
	
	// See if pcap can find right arcnet device
	ArcnetIO arcio2;
	try {

		arcio2.enable();

	} catch (const std::exception& e) {
		std::cout << "Test 2 failed: " << e.what() << std::endl;
		arcio2.disable();
		return -1;
	}
	
	arcio2.disable();
	std::cout << "Test 2 passed." << std::endl;	
	return 0;
}

