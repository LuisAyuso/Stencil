#include <gtest/gtest.h>

#include "bufferSet.h"

using namespace stencil;

TEST(Buffer, Constructor){

	{
		std::vector<int> v = {0,1,2,3,4};

		BufferSet<int,1> b ({5}, v);

		std::cout << b << std::endl;

	// test loaded data
		for (int i=0; i<5; ++i){
			EXPECT_EQ(i, getElem(b, i, 0));
		}

	// modify data:
		for (int i=0; i<5; ++i){
			getElem(b, i, 0) =  0;
		}
		for (int i=0; i<5; ++i){
			getElem(b, i, 1) =  i;
		}
		for (int i=0; i<5; ++i){
			EXPECT_EQ(getElem(b, i, 1), i);
		}
		for (int i=0; i<5; ++i){
			EXPECT_EQ(getElem(b, i, 0), 0);
		}
	}

	{
		std::vector<int> v = {0,1,2,3,4,
		                      0,1,2,3,4};

		BufferSet<int,2> b ({5,2}, v);

		std::cout << b << std::endl;

		for (int i=0; i<5; ++i)
			for (int j=0; j<2; ++j)
				EXPECT_EQ(getElem(b, i, j, 0), i);


		// modify
		for (int i=0; i<5; ++i)
			for (int j=0; j<2; ++j)
				getElem(b, i, j, 0)= 0;

		for (int i=0; i<5; ++i)
			for (int j=0; j<2; ++j)
				getElem(b, i, j, 1)= i;

		// check
		for (int i=0; i<5; ++i)
			for (int j=0; j<2; ++j)
				EXPECT_EQ(getElem(b, i, j, 0), 0);

		for (int i=0; i<5; ++i)
			for (int j=0; j<2; ++j)
				EXPECT_EQ(getElem(b, i, j, 1), i);
	}

	{
		std::vector<int> v = {0,1,2,3,4, 0,1,2,3,4, 0,1,2,3,4,
		                      0,1,2,3,4, 0,1,2,3,4, 0,1,2,3,4};

		BufferSet<int,3> b ({5,2,3}, v);

		std::cout << b << std::endl;

		for (int i=0; i<5; ++i)
			for (int j=0; j<2; ++j)
				for (int k=0; k<3; ++k)
					EXPECT_EQ(getElem(b, i, j, k, 0), i);

		// modify
		for (int i=0; i<5; ++i)
			for (int j=0; j<2; ++j)
				for (int k=0; k<3; ++k)
					getElem(b, i, j, k, 0) = 0;

		for (int i=0; i<5; ++i)
			for (int j=0; j<2; ++j)
				for (int k=0; k<3; ++k)
					getElem(b, i, j, k, 1) = i;

		// check
		for (int i=0; i<5; ++i)
			for (int j=0; j<2; ++j)
				for (int k=0; k<3; ++k)
					EXPECT_EQ(0, getElem(b, i, j, k, 0));

		for (int i=0; i<5; ++i)
			for (int j=0; j<2; ++j)
				for (int k=0; k<3; ++k)
					EXPECT_EQ(i, getElem(b, i, j, k, 1));

	}
}



TEST(Buffer, Copy){

	std::cout << "Copy is not cool, deactivated"<<std::endl;
//
//	{
//		std::vector<int> v = {0,1,2,3,4};
//
//		BufferSet<int,1> b ({5}, v);
//
//		std::cout << b << std::endl;
//
//	// test loaded data
//		for (int i=0; i<5; ++i){
//			EXPECT_EQ(getElem(b, i), i);
//		}
//
//		auto b2 = b;
//		std::cout << b2 << std::endl;
//
//	// test loaded data
//		for (int i=0; i<5; ++i){
//			EXPECT_EQ(getElem(b2, i), i);
//		}
//
//	}
//
//	{
//		std::vector<int> v = {0,1,2,3,4,
//		                      0,1,2,3,4};
//
//		BufferSet<int,2> b ({5,2}, v);
//
//		std::cout << b << std::endl;
//
//		for (int i=0; i<5; ++i)
//			for (int j=0; j<2; ++j)
//				EXPECT_EQ(getElem(b, i, j), i);
//
//		auto b2 = b;
//
//		for (int i=0; i<5; ++i)
//			for (int j=0; j<2; ++j)
//				EXPECT_EQ(getElem(b2, i, j), i);
//
//	}
//
//	{
//		std::vector<int> v = {0,1,2,3,4, 0,1,2,3,4, 0,1,2,3,4,
//		                      0,1,2,3,4, 0,1,2,3,4, 0,1,2,3,4};
//
//		BufferSet<int,3> b ({5,2,3}, v);
//
//		std::cout << b << std::endl;
//
//		for (int i=0; i<5; ++i)
//			for (int j=0; j<2; ++j)
//				for (int k=0; k<3; ++k)
//					EXPECT_EQ(getElem(b, i, j, k), i);
//
//		auto b2 = b;
//
//		for (int i=0; i<5; ++i)
//			for (int j=0; j<2; ++j)
//				for (int k=0; k<3; ++k)
//					EXPECT_EQ(getElem(b2, i, j, k), i);
//
//	}
}

TEST(Buffer, Move){

	{
		std::vector<int> v = {0,1,2,3,4};

		BufferSet<int,1> b ({5}, v);

		std::cout << b << std::endl;

	// test loaded data
		for (int i=0; i<5; ++i){
			EXPECT_EQ(getElem(b, i, 0), i);
		}

		auto b2 = std::move(b);

	// test loaded data
		for (int i=0; i<5; ++i){
			EXPECT_EQ(getElem(b2, i, 0), i);
		}

	}

	{
		std::vector<int> v = {0,1,2,3,4,
		                      0,1,2,3,4};

		BufferSet<int,2> b ({5,2}, v);

		std::cout << b << std::endl;

		for (int i=0; i<5; ++i)
			for (int j=0; j<2; ++j)
				EXPECT_EQ(getElem(b, i, j, 0), i);


		auto b2 = std::move(b);

		for (int i=0; i<5; ++i)
			for (int j=0; j<2; ++j)
				EXPECT_EQ(getElem(b2, i, j, 0), i);
	}

	{
		std::vector<int> v = {0,1,2,3,4, 0,1,2,3,4, 0,1,2,3,4,
		                      0,1,2,3,4, 0,1,2,3,4, 0,1,2,3,4};

		BufferSet<int,3> b ({5,3,2}, v);

		std::cout << b << std::endl;

		for (int i=0; i<5; ++i)
			for (int j=0; j<2; ++j)
				for (int k=0; k<2; ++k)
					EXPECT_EQ(getElem(b, i, j, k, 0), i);

		auto b2 = std::move(b);

		for (int i=0; i<5; ++i)
			for (int j=0; j<2; ++j)
				for (int k=0; k<2; ++k)
					EXPECT_EQ(getElem(b2, i, j, k, 0), i);
	}
}

TEST(Buffer, Getters){

	{
		std::vector<int> v = {0,1,2,3,4};
		BufferSet<int,3> b ({5,3,2}, v);
		EXPECT_EQ (5, getW(b));
		EXPECT_EQ (3, getH(b));
		EXPECT_EQ (2, getD(b));
	}

	{
		std::vector<int> v = {0,1,2,3,4};
		BufferSet<int,2> b ({3,2}, v);
		EXPECT_EQ (3, getW(b));
		EXPECT_EQ (2, getH(b));
	}

	{
		std::vector<int> v = {0,1,2,3,4};
		BufferSet<int,1> b ({15}, v);
		EXPECT_EQ (15, getW(b));
	}
}


