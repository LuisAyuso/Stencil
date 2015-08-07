#include <gtest/gtest.h>

#include "bufferSet.h"

using namespace stencil;

TEST(Buffer, Constructor){

	{
		std::vector<int> v = {0,1,2,3,4};

		BufferSet<int,1> b (v, {5});

		std::cout << b << std::endl;

	// test loaded data
		for (int i=0; i<5; ++i){
			EXPECT_EQ(getElem(b, i), i);
		}

	// test mirror data (unused)
		for (int i=0; i<5; ++i){
			EXPECT_EQ(getElem(b, i, 1), 0);
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

		BufferSet<int,2> b (v, {5,2});

		std::cout << b << std::endl;

		for (int i=0; i<5; ++i)
			for (int j=0; j<2; ++j)
				EXPECT_EQ(getElem(b, i, j), i);

		for (int i=0; i<5; ++i)
			for (int j=0; j<2; ++j)
				EXPECT_EQ(getElem(b, i, j, 1), 0);

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
				EXPECT_EQ(getElem(b, i, j), 0);

		for (int i=0; i<5; ++i)
			for (int j=0; j<2; ++j)
				EXPECT_EQ(getElem(b, i, j, 1), i);
	}

	{
		std::vector<int> v = {0,1,2,3,4, 0,1,2,3,4, 0,1,2,3,4,
		                      0,1,2,3,4, 0,1,2,3,4, 0,1,2,3,4};

		BufferSet<int,3> b (v, {5,3,2});

		std::cout << b << std::endl;

		for (int i=0; i<5; ++i)
			for (int j=0; j<2; ++j)
				for (int k=0; k<2; ++k)
					EXPECT_EQ(getElem(b, i, j, k), i);

		for (int i=0; i<5; ++i)
			for (int j=0; j<2; ++j)
				for (int k=0; k<2; ++k)
					EXPECT_EQ(getElem(b, i, j, k, 1), 0);

		// modify
		for (int i=0; i<5; ++i)
			for (int j=0; j<2; ++j)
				for (int k=0; k<2; ++k)
					getElem(b, i, j, k, 0) = 0;

		for (int i=0; i<5; ++i)
			for (int j=0; j<2; ++j)
				for (int k=0; k<2; ++k)
					getElem(b, i, j, k, 1) = i;

		// check
		for (int i=0; i<5; ++i)
			for (int j=0; j<2; ++j)
				for (int k=0; k<2; ++k)
					EXPECT_EQ(getElem(b, i, j, k, 0), 0);

		for (int i=0; i<5; ++i)
			for (int j=0; j<2; ++j)
				for (int k=0; k<2; ++k)
					EXPECT_EQ(getElem(b, i, j, k, 1), i);
	}
}
