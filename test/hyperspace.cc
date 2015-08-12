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

TEST(Hyperspace, Split_1D_W){
		Hyperspace<1> h (5, 10, -1, 1);

		auto n = h.split(7);
		
		EXPECT_EQ(n.size(), 3);
		for (const auto& e: n)
			std::cout << e << std::endl;

		EXPECT_EQ(Hyperspace<1>(5,10,1,-1) ,n[0]);
		EXPECT_EQ(Hyperspace<1>(5,5,-1,1)  ,n[1]);
		EXPECT_EQ(Hyperspace<1>(10,10,-1,1),n[2]);
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

		auto n = h.split_slopes(CutWithSlopes{5,1,-1});
		
		EXPECT_EQ(n.size(), 3);
		for (const auto& e: n)
			std::cout << e << std::endl;

		EXPECT_EQ(n[0], Hyperspace<1>(0,5,0,-1));
		EXPECT_EQ(n[1], Hyperspace<1>(5,10,1,0));
		EXPECT_EQ(n[2], Hyperspace<1>(5,5,-1,1));

}

TEST(Hyperspace, Split_1D_W_Slopes){
		Hyperspace<1> h (5, 10, -1, 1);

		auto n = h.split_slopes(CutWithSlopes{7,1,-1});
		
		EXPECT_EQ(n.size(), 3);
		for (const auto& e: n)
			std::cout << e << std::endl;

		EXPECT_EQ(Hyperspace<1>(5,10,1,-1) ,n[0]);
		EXPECT_EQ(Hyperspace<1>(5,5,-1,1)  ,n[1]);
		EXPECT_EQ(Hyperspace<1>(10,10,-1,1),n[2]);
}

TEST(Hyperspace, Split_2D_Slopes){
		Hyperspace<2> h (0, 10, 0, 0,
						 0, 10, 0, 0);

		auto n = h.split_slopes(CutWithSlopes{5,1,-1});
		
		EXPECT_EQ(n.size(), 3);
		for (const auto& e: n)
			std::cout << e << std::endl;

		EXPECT_EQ(n[0], Hyperspace<2>(0,5,0,-1, 0, 10, 0, 0));
		EXPECT_EQ(n[1], Hyperspace<2>(5,10,1,0, 0, 10, 0, 0));
		EXPECT_EQ(n[2], Hyperspace<2>(5,5,-1,1, 0, 10, 0, 0));
}


TEST(Hyperspace, Split_2D_M_slopes){
		Hyperspace<2> h (0, 512, 0, 0,
						 0, 512, 0, 0);

		auto n = h.split_slopes(CutWithSlopes{256,1,-1}, CutWithSlopes{256,1,-1});
		
		EXPECT_EQ(n.size(), 9);
		for (const auto& e: n)
			std::cout << e << std::endl;

		EXPECT_EQ(n[0], Hyperspace<2>(0,256,0,-1, 0,256, 0,-1));
		EXPECT_EQ(n[1], Hyperspace<2>(256,512,1,0,0,256,0,-1) );
		EXPECT_EQ(n[2], Hyperspace<2>(256,256,-1,1,0,256,0,-1) );
		EXPECT_EQ(n[3], Hyperspace<2>(0,256,0,-1,256,512,1,0) );
		EXPECT_EQ(n[4], Hyperspace<2>(256,512,1,0,256,512,1,0) );
		EXPECT_EQ(n[5], Hyperspace<2>(256,256,-1,1,256,512,1,0) );
		EXPECT_EQ(n[6], Hyperspace<2>(0,256,0,-1,256,256,-1,1) );
		EXPECT_EQ(n[7], Hyperspace<2>(256,512,1,0,256,256,-1,1) );
		EXPECT_EQ(n[8], Hyperspace<2>(256,256,-1,1,256,256,-1,1));
}

TEST(Hyperspace, Split_2D_M_slopes_2){
		Hyperspace<2> h (0, 512, 1, -1,
						 0, 512, 1, -1);

		auto n = h.split_slopes(CutWithSlopes{256,1,-1}, CutWithSlopes{256,1,-1});
		
		EXPECT_EQ(n.size(), 9);
		for (const auto& e: n)
			std::cout << e << std::endl;

		EXPECT_EQ(n[0], Hyperspace<2>(0  ,256, 1,-1,   0, 256, 1,-1) );
		EXPECT_EQ(n[1], Hyperspace<2>(256,512, 1,-1,   0, 256, 1,-1) );
		EXPECT_EQ(n[2], Hyperspace<2>(256,256,-1, 1,   0, 256, 1,-1) );
		EXPECT_EQ(n[3], Hyperspace<2>(0  ,256, 1,-1, 256, 512, 1,-1) );
		EXPECT_EQ(n[4], Hyperspace<2>(256,512, 1,-1, 256, 512, 1,-1) );
		EXPECT_EQ(n[5], Hyperspace<2>(256,256,-1, 1, 256, 512, 1,-1) );
		EXPECT_EQ(n[6], Hyperspace<2>(0  ,256, 1,-1, 256, 256,-1, 1) );
		EXPECT_EQ(n[7], Hyperspace<2>(256,512, 1,-1, 256, 256,-1, 1) );
		EXPECT_EQ(n[8], Hyperspace<2>(256,256,-1, 1, 256, 256,-1, 1) );
}


template<unsigned Dim>
std::vector<Hyperspace<2>> recCut( const Hyperspace<2>& hyp, bool stop = false, int steps = 0){

//	for (int i=0; i<steps; ++i) std::cout << " ";
//	std::cout << hyp << std::endl;
	
	auto a = hyp.a(Dim);
	auto b = hyp.b(Dim);

	auto delta = b-a;

	//base case
	if( delta == 0 && stop){
		return {hyp};
	}
	else if( delta == 1 && stop){
		return {hyp};
	}

	if (delta <= 1 ){
		stop =true;
	}

	auto x = hyp.split_slopes<Dim>(CutWithSlopes{a+delta/2, 1, -1});

	std::vector<Hyperspace<2>> res;
	for (auto h : x){
		auto tmp = recCut< (Dim+1)%2 > (h, stop, steps+1);
		res.insert(res.end(), tmp.begin(), tmp.end());
	}

	return res;
}

TEST(Hyperspace, Split_2D_2){

	Hyperspace<2> h (0, 2, 0, 0,
					 0, 2, 0, 0);

	auto n = recCut<0>(h);

	for (const auto& e: n)
		std::cout << e << std::endl;


								//     |\        |\  
	EXPECT_EQ( n[0], Hyperspace<2> (0,1, 0,-1,  0,1, 0,-1));
								//     |\        /|  
	EXPECT_EQ( n[1], Hyperspace<2> (0,1, 0,-1,  1,2, 1, 0));
								//     |\        \/   
	EXPECT_EQ( n[2], Hyperspace<2> (0,1, 0,-1,  1,1,-1, 1));

								//     /|        |\    
	EXPECT_EQ( n[3], Hyperspace<2> (1,2, 1, 0,  0,1, 0,-1));
								//     /|        /|    
	EXPECT_EQ( n[4], Hyperspace<2> (1,2, 1, 0,  1,2, 1, 0));
								//     /|        \/    
	EXPECT_EQ( n[5], Hyperspace<2> (1,2, 1, 0,  1,1,-1, 1));

								//     \/	     |\ 
	EXPECT_EQ( n[6], Hyperspace<2> (1,1,-1, 1,  0,1, 0,-1));
								//     \/	     /| 
	EXPECT_EQ( n[7], Hyperspace<2> (1,1,-1, 1,  1,2, 1, 0));
								//     \/	     \/
	EXPECT_EQ( n[8], Hyperspace<2> (1,1,-1, 1,  1,1,-1, 1));


	std::vector<Hyperspace<2>> level0;
	std::vector<Hyperspace<2>> level1;
	std::vector<Hyperspace<2>> level2;

	for (const auto& i: n){
		if (i.getStep() == 0) level0.push_back(i);
		if (i.getStep() == 1) level1.push_back(i);
		if (i.getStep() == 2) level2.push_back(i);
	}

	EXPECT_EQ(level0.size() + level1.size() + level2.size(), n.size());

	EXPECT_EQ(level0.size(), 4);
	EXPECT_EQ(level1.size(), 4);
	EXPECT_EQ(level2.size(), 1);
}

TEST(Hyperspace, Split_2D_3){

	Hyperspace<2> h (0, 3, 0, 0,
					 0, 3, 0, 0);

	auto n = recCut<0>(h);


	for (const auto& i: n){
		int sum = 0;
		for (const auto& j: n){
			if (i==j) sum ++;
		}
		EXPECT_TRUE(sum == 1) << i << "is repeated:";
	}

	std::vector<Hyperspace<2>> level0;
	std::vector<Hyperspace<2>> level1;
	std::vector<Hyperspace<2>> level2;

	for (const auto& i: n){
		if (i.getStep() == 0) level0.push_back(i);
		if (i.getStep() == 1) level1.push_back(i);
		if (i.getStep() == 2) level2.push_back(i);
	}

	EXPECT_EQ(level0.size() + level1.size() + level2.size(), n.size());

	for (const auto& e: level0)
		std::cout << e << std::endl;

	EXPECT_EQ(level0.size(), 9);
	EXPECT_EQ(level1.size(), 12);
	EXPECT_EQ(level2.size(), 4);
}

TEST(Hyperspace, Split_2D_8){

	Hyperspace<2> h (0, 8, 0, 0,
					 0, 8, 0, 0);

	auto n = recCut<0>(h);

	for (const auto& e: n)
		std::cout << e << std::endl;

	for (const auto& i: n){
		int sum = 0;
		for (const auto& j: n){
			if (i==j) sum ++;
		}
		EXPECT_TRUE(sum == 1) << i << "is repeated:";
	}

	std::cout << "========================== " << std::endl;
	std::vector<int> levels(3);

	ASSERT_FALSE(n.empty() );
	for (const auto& i: n){
		levels[i.getStep()] ++;
	}

	EXPECT_EQ(levels[0], 8*8);
	EXPECT_EQ(levels[1], 108);
}

TEST(Hyperspace, Split_2D_16){

	Hyperspace<2> h (0, 16, 0, 0,
					 0, 16, 0, 0);

	auto n = recCut<0>(h);

	for (const auto& e: n)
		std::cout << e << std::endl;

	for (const auto& i: n){
		int sum = 0;
		for (const auto& j: n){
			if (i==j) sum ++;
		}
		EXPECT_TRUE(sum == 1) << i << "is repeated:";
	}

}

