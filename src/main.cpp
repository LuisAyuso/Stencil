
#include <iostream>
#include "hyperspace.h"
#include "kernel.h"


using namespace stencil;

int main(){


	std::cout << "hello world" << std::endl;	


	{
		Hyperspace<1> h (0,10,1,-1);
		std::cout << "-" << h << std::endl;

		std::cout << "split: 5" << std::endl;
		auto n = h.split(5);

		for (const auto& e: n)
			std::cout << e << std::endl;
	}

	{
		Hyperspace<2> h (0,10,1,-1,
						 0,10, 1, -1 );

		std::cout << "-" << h << std::endl;
		std::cout << "split: 5" << std::endl;
		auto n = h.split(5);
		for (const auto& e: n)
			std::cout << e << std::endl;
	}

	{
		Hyperspace<2> h (0,10,1,-1,
						 0,10, 1, -1 );

		std::cout << "-" << h << std::endl;
		std::cout << "split: 5,5" << std::endl;
		auto n = h.split(5,5);
		for (const auto& e: n)
			std::cout << e << std::endl;
	}

	{
		Hyperspace<2> h ({0,10},{0,10},{1,1},{-1,-1});

		std::cout << "-" << h << std::endl;
		std::cout << "split: 5,5" << std::endl;
		auto n = h.split(5,5);
		for (const auto& e: n)
			std::cout << e << std::endl;
	}

	{
		Hyperspace<3> h ({0,0,0},{10,10, 10},{1,1,1},{-1,-1,-1});

		std::cout << "-" << h << std::endl;
		std::cout << "split: 5,5,5" << std::endl;
		auto n = h.split(5,5,5);
		for (const auto& e: n)
			std::cout << e << std::endl;
	}

	return 0;
}
