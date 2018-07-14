#include <gtest/gtest.h>

#include "kernel.h"
#include "kernels_2D.h"

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


/*
TEST(Kernel, Gaussian_Blur){

	using namespace example_kernels;

	{
		BlurN_k<double, 3> kernel;

		std::cout << "size 3:\n ";
		double sum = 0.0;
		for (auto i = 0; i < 3; ++i){
			for (auto j = 0; j < 3; ++j){
				std::cout  << kernel.Kcoeff[i][j] << ",\t";
				sum += kernel.Kcoeff[i][j];
			}
			std::cout << "\n ";
		}
		ASSERT_NEAR(1, sum, 0.00001);

		for (auto i = 0; i < 3; ++i){
			for (auto j = 0; j < 3; ++j){
				EXPECT_EQ( kernel.Kcoeff[i][j], kernel.Kcoeff[j][i]);
			}
		}
		
	}

	{
		BlurN_k<double, 5> kernel;


		std::cout << "size 5:\n ";
		double sum = 0.0;
		for (auto i = 0; i < 5; ++i){
			for (auto j = 0; j < 5; ++j){
				std::cout  << kernel.Kcoeff[i][j] << ",\t";
				sum += kernel.Kcoeff[i][j];
			}
			std::cout << "\n ";
		}
		ASSERT_NEAR(1, sum, 0.00001);
		
		for (auto i = 0; i < 5; ++i){
			for (auto j = 0; j < 5; ++j){
				EXPECT_EQ( kernel.Kcoeff[i][j], kernel.Kcoeff[j][i]);
			}
		}
	}
	
	{
		BlurN_k<double, 7> kernel;


		std::cout << "size 7:\n ";
		double sum = 0.0;
		for (auto i = 0; i < 7; ++i){
			for (auto j = 0; j < 7; ++j){
				std::cout  << kernel.Kcoeff[i][j] << ",\t";
				sum += kernel.Kcoeff[i][j];
			}
			std::cout << "\n ";
		}
		ASSERT_NEAR(1, sum, 0.00001);
		
		for (auto i = 0; i < 7; ++i){
			for (auto j = 0; j < 7; ++j){
				EXPECT_EQ( kernel.Kcoeff[i][j], kernel.Kcoeff[j][i]);
			}
		}
	}
}*/

TEST(Kernel, SolveRoutine){

	{
		typedef std::array<int, 10> Data;

		struct TestKernel : public Kernel<Data, 1, TestKernel>{

			static void withBonduaries (Data& data, unsigned i, unsigned t){
				data[i] = 15;
			}
			static void withoutBonduaries (Data& data, unsigned i, unsigned t){
				data[i] = 25;
			}

		};

		Data data;
		solve<true,TestKernel> (data, 0, 0);

		EXPECT_EQ(data[0], 15);

		solve<false,TestKernel> (data, 1, 0);
		EXPECT_EQ(data[1], 25);
	}
}


