#include <gtest/gtest.h>

#include "kernel.h"

using namespace stencil;

TEST(Kernel, Operator){

	{
		typedef std::array<int, 10> Data;

		struct TestKernel : public Kernel<Data, 1, TestKernel>{

			void operator() (Data& data, unsigned i, unsigned t){
				data[i] = 15;
			}
			std::pair<int,int> getSlope(unsigned dimension){
				return {0,0};
			}
		};

		TestKernel k;
		Data       buff;

		for (int i = 0; i < 10; ++i)
			k(buff, i, 0);

		for (int i = 0; i < 10; ++i)
			EXPECT_EQ(buff[i], 15);
	}

	{
		typedef std::array<int, 10*10> Data;

		struct TestKernel : public Kernel<Data, 2, TestKernel>{

			void operator() (Data& data, unsigned i,unsigned j, unsigned t){
				data[i+j*10] = 15;
			}
			std::pair<int,int> getSlope(unsigned dimension){
				return {0,0};
			}
		};

		TestKernel k;
		Data       buff;

		for (int i = 0; i < 10; ++i)
			for (int j = 0; j < 10; ++j)
				k(buff, i, j, 0);

		for (int i = 0; i < 100; ++i)
			EXPECT_EQ(buff[i], 15);
	}

	{
		typedef std::array<int, 10*10*10> Data;

		struct TestKernel : public Kernel<Data, 3, TestKernel>{

			void operator() (Data& data, unsigned i,unsigned j, unsigned k, unsigned t){
				data[i+j*10+k*10*10] = 15;
			}
			std::pair<int,int> getSlope(unsigned dimension){
				return {0,0};
			}
		};

		TestKernel ker;
		Data       buff;

		for (int i = 0; i < 10; ++i)
			for (int j = 0; j < 10; ++j)
				for (int k = 0; k < 10; ++k)
					ker(buff, i, j, k, 0);

		for (int i = 0; i < 1000; ++i)
			EXPECT_EQ(buff[i], 15);
	}
}
