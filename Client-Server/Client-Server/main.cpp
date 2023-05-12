#include "main.h"



int main()
{
	int t;
	std::cin >> t;
	if (t == 0) {
		Server Sr((char*)"127.168.0.123", 12345, 100);
		Sr.Handler();
	}
	else {
		Client Cl((char*)"127.168.0.123", 12345);
		Cl.Handler();
	}
}