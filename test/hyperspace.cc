#include <gtest/gtest.h>

#include "hyperspace.h"

using namespace stencil;


TEST(Hyperspace, Constructor){

	{
		Hyperspace<1> h (0,10,1,-1);

		EXPECT_EQ(h.a(0), 0);
		EXPECT_EQ(h.b(0), 10);
		EXPECT_EQ(h.da(0), 1);
		EXPECT_EQ(h.db(0), -1);
	}

	{
		Hyperspace<2> h (1,2,3,4,
						 5,6,7,8 );

		EXPECT_EQ(h.a(0),  1);
		EXPECT_EQ(h.b(0),  2);
		EXPECT_EQ(h.da(0), 3);
		EXPECT_EQ(h.db(0), 4);

		EXPECT_EQ(h.a(1),  5);
		EXPECT_EQ(h.b(1),  6);
		EXPECT_EQ(h.da(1), 7);
		EXPECT_EQ(h.db(1), 8);
	}
}


TEST(Hyperspace, NoSplit){
		Hyperspace<1> h (0,10,1,-1);
		std::cout << "-" << h << std::endl;

		std::cout << "split: 0" << std::endl;
		auto n = h.split(0);

		EXPECT_EQ(n.size(), 1);
}

TEST(Hyperspace, Split1D){

		Hyperspace<1> h (0,10,1,-1);
		std::cout << "-" << h << std::endl;

		std::cout << "split: 5" << std::endl;
		auto n = h.split(5);

		EXPECT_EQ(n.size(), 3);

		for (const auto& e: n)
			std::cout << e << std::endl;
}

TEST(Hyperspace, Split2D1){

		Hyperspace<2> h (0,10, 1,-1,
						 0,10, 1,-1 );

		std::cout << "-" << h << std::endl;
		std::cout << "split: 5" << std::endl;

		auto n = h.split(5);
		EXPECT_EQ(n.size(), 3);

		for (const auto& e: n)
			std::cout << e << std::endl;
}

TEST(Hyperspace, Split2D2){
		Hyperspace<2> h (0,10, 1,-1,
						 0,10, 1,-1 );

		std::cout << "-" << h << std::endl;
		std::cout << "split: 5,5" << std::endl;

		auto n = h.split(5,5);
		EXPECT_EQ(n.size(), 9);
		for (const auto& e: n)
			std::cout << e << std::endl;
}

TEST(Hyperspace, Split2D2_2){
		Hyperspace<2> h ({0,0},{10,10},{1,1},{-1,-1});

		std::cout << "-" << h << std::endl;
		std::cout << "split: 5,5" << std::endl;
		auto n = h.split(5,5);

		EXPECT_EQ(n.size(), 9);
		for (const auto& e: n)
			std::cout << e << std::endl;
}

TEST(Hyperspace, Split_Edge){
		Hyperspace<2> h ({0,0},{5,5},{1,1},{-1,-1});

		std::cout << "-" << h << std::endl;
		std::cout << "split: 5,5" << std::endl;
		auto n = h.split(5,5);

		EXPECT_EQ(n.size(), 1);
		for (const auto& e: n)
			std::cout << e << std::endl;
}

TEST(Hyperspace, Split_2D_problem){
		Hyperspace<2> h ({13,3},{19,7},{-1,1},{1,-1});

		std::cout << "-" << h << std::endl;
		std::cout << "split: 18,3" << std::endl;
		auto n = h.split(16,3);

		EXPECT_EQ(n.size(), 3);
		for (const auto& e: n)
			std::cout << e << std::endl;
}


TEST(Hyperspace, Split3D3){

		Hyperspace<3> h ({0,0,0},{10,10,10},{1,1,1},{-1,-1,-1});

		std::cout << "-" << h << std::endl;
		std::cout << "split: 5,5,5" << std::endl;

		auto n = h.split(5,5,5);
		EXPECT_EQ(n.size(), 27);
		for (const auto& e: n)
			std::cout << e << std::endl;
}

TEST(Hyperspace, Split3D1){
		Hyperspace<3> h ({0,0,0},{10,10, 10},{1,1,1},{-1,-1,-1});

		std::cout << "-" << h << std::endl;
		std::cout << "split: 0,5,0" << std::endl;

		auto n = h.split(0,5,0);
		EXPECT_EQ(n.size(), 3);
		for (const auto& e: n)
			std::cout << e << std::endl;
}

TEST(Hyperspace, Split_1D_Slopes){
		Hyperspace<1> h (0, 10, 0, 0);

		auto n = h.split_slopes(Hyperspace<1>::CutWithSlopes{5,1,-1});
		
		EXPECT_EQ(n.size(), 3);
		for (const auto& e: n)
			std::cout << e << std::endl;

		EXPECT_EQ(n[0], Hyperspace<1>(0,5,0,-1));
		EXPECT_EQ(n[1], Hyperspace<1>(5,10,1,0));
		EXPECT_EQ(n[2], Hyperspace<1>(5,5,-1,1));

}

TEST(Hyperspace, Split_2D_Slopes){
		Hyperspace<2> h (0, 10, 0, 0,
						 0, 10, 0, 0);

		auto n = h.split_slopes(Hyperspace<2>::CutWithSlopes{5,1,-1});
		
		EXPECT_EQ(n.size(), 3);
		for (const auto& e: n)
			std::cout << e << std::endl;

		EXPECT_EQ(n[0], Hyperspace<2>(0,5,0,-1, 0, 10, 0, 0));
		EXPECT_EQ(n[1], Hyperspace<2>(5,10,1,0, 0, 10, 0, 0));
		EXPECT_EQ(n[2], Hyperspace<2>(5,5,-1,1, 0, 10, 0, 0));
}
