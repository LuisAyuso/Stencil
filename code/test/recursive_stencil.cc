#include <gtest/gtest.h>

#include "kernel.h"
//#include "rec_stencil_inverted_dims.h"
//#include "rec_stencil_multiple_splits.h"
#include "new_rec_stencil.h"
#include "kernels_1D.h"
#include "kernels_2D.h"
#include "kernels_3D.h"

#include "timer.h"

using namespace stencil;
using namespace stencil::example_kernels;


template <typename Data>
std::vector<Data> initData(int size){
	std::vector<Data> data (size);
	for (auto i =1; i< size; ++i) 	data[i] = i;
	return data;
}

template <typename Data>
std::vector<Data> zeroData(int size){
	return std::vector<Data> (size);
}

TEST(Stencil1D, AVG){

	typedef int Type;

	const int SIZE = 100;
	using KernelType = Avg_1D_k<BufferSet<Type, 1>>;

	auto data  = initData<Type> (SIZE);
	{

		BufferSet<Type, 1> buff1 ({SIZE}, data);
		BufferSet<Type, 1> buff2 ({SIZE}, data);


		recursive_stencil<BufferSet<Type, 1>, Avg_1D_k<BufferSet<Type, 1>>>( buff1, 1);

		for (int i=0; i < SIZE; ++i)
			KernelType::withBonduaries(buff2, i, 0);

		for (int i=0; i < SIZE; ++i){
			EXPECT_EQ(getElem(buff1, i, 0), getElem(buff1, i, 0));
			EXPECT_EQ(getElem(buff1, i, 1), getElem(buff1, i, 1));
		}
	}

	{

		BufferSet<Type, 1> buff1 ({SIZE}, data);
		BufferSet<Type, 1> buff2 ({SIZE}, data);

		recursive_stencil<BufferSet<Type, 1>, Avg_1D_k<BufferSet<Type, 1>>>( buff1,  40);

		for (int t=0; t < 40; ++t)
		for (int i=0; i < SIZE; ++i)
			KernelType::withBonduaries(buff2, i, t);

		for (int i=0; i < SIZE; ++i){
			EXPECT_EQ(getElem(buff1, i, 0), getElem(buff1, i, 0));
			EXPECT_EQ(getElem(buff1, i, 1), getElem(buff1, i, 1));
		}
	}
}



TEST(Stencil2D, Copy){

	typedef int Type;

	auto data  = initData<Type> (10*10);

	BufferSet<Type, 2> buff1 ({10, 10}, data);
	using KernelType = Copy_k<BufferSet<Type, 2>>;

	recursive_stencil<BufferSet<Type, 2>, Copy_k<BufferSet<Type, 2>>>( buff1, 1);

	for (auto i = 0; i < 10; i ++)
	for (auto j = 0; j < 10; j ++)
		EXPECT_EQ( getElem(buff1, i, j, 0), getElem(buff1, i, j, 1));


	recursive_stencil<BufferSet<Type, 2>, Copy_k<BufferSet<Type, 2>>>( buff1, 10);

	for (auto i = 0; i < 10; i ++)
	for (auto j = 0; j < 10; j ++)
		EXPECT_EQ( getElem(buff1, i, j, 0), getElem(buff1, i, j, 1));


	recursive_stencil<BufferSet<Type, 2>, Copy_k<BufferSet<Type, 2>>>( buff1, 20);

	for (auto i = 0; i < 10; i ++)
	for (auto j = 0; j < 10; j ++)
		EXPECT_EQ( getElem(buff1, i, j, 0), getElem(buff1, i, j, 1));
}


namespace {

	template< typename DataStorage> 
	struct Translate_k : public Kernel<DataStorage, 2, Translate_k<DataStorage>>{

		static void withBonduaries (DataStorage& data, unsigned i, unsigned j, unsigned t){
			if 		(0> (int)i-1)	getElem(data, i, j, t+1) = 0;
			else if (0> (int)j-1)	getElem(data, i, j, t+1) = 0;
			else 					getElem(data, i, j, t+1) = getElem(data, i-1, j-1, t);
		}

		static const unsigned int neighbours = 1;
	};

}


TEST(Stencil2D, Translate){

	typedef int Type;
	const int SIZE = 10;

	auto data  = initData<Type> (SIZE*SIZE);

	BufferSet<Type, 2> buff1 ({SIZE, SIZE}, data);
	BufferSet<Type, 2> buff2 ({SIZE, SIZE}, data);

	using KernelType = Translate_k<BufferSet<Type, 2>>;

	recursive_stencil<BufferSet<Type, 2>, Translate_k<BufferSet<Type, 2>>>( buff1, 1);

	for (auto i = 1; i < SIZE; i ++)
	for (auto j = 1; j < SIZE; j ++)
		EXPECT_EQ( getElem(buff1, i-1, j-1, 0), getElem(buff1, i, j, 1));


	recursive_stencil<BufferSet<Type, 2>, Translate_k<BufferSet<Type, 2>>>( buff2, 2);

	for (auto i = 2; i < SIZE; i ++)
	for (auto j = 2; j < SIZE; j ++)
		EXPECT_EQ( getElem(buff2, i, j, 0), getElem(buff2, i-1, j-1, 1));

	for (auto i = 2; i < SIZE; i ++)
	for (auto j = 2; j < SIZE; j ++)
		EXPECT_EQ( getElem(buff1, i-2, j-2, 0), getElem(buff2, i, j, 0));
}


TEST(Stencil2D, Blur){

	typedef int Type;
	const int SIZE = 100;

	auto data  = initData<Type> (SIZE*SIZE);

	BufferSet<Type, 2> buff1 ({SIZE, SIZE}, data);
	BufferSet<Type, 2> buff2 ({SIZE, SIZE}, data);
	BufferSet<Type, 2> buff3 ({SIZE, SIZE}, data);
	BufferSet<Type, 2> buff4 ({SIZE, SIZE}, data);

	using KernelType = Blur3_k<BufferSet<Type, 2>>;

	recursive_stencil<BufferSet<Type, 2>, Blur3_k<BufferSet<Type, 2>>>( buff1, 1);


	for (int i = 0; i < SIZE; ++i)
	for (int j = 0; j < SIZE; ++j)
		KernelType::withBonduaries(buff2, i, j, 0);


	for (auto i = 1; i < SIZE; i ++)
	for (auto j = 1; j < SIZE; j ++)
		EXPECT_EQ( getElem(buff1, i, j, 1), getElem(buff2, i, j, 1));


	recursive_stencil<BufferSet<Type, 2>, Blur3_k<BufferSet<Type, 2>>>( buff3, 20);
	recursive_stencil<BufferSet<Type, 2>, Blur3_k<BufferSet<Type, 2>>>( buff4, 20);

	for (auto i = 1; i < SIZE; i ++)
	for (auto j = 1; j < SIZE; j ++)
		EXPECT_EQ( getElem(buff3, i, j, 1), getElem(buff4, i, j, 1));

}

TEST(Stencil2D, Blur11){

	typedef double Type;
	const int SIZE = 100;
	const int TIMESTEPS = 20;

	auto data  = initData<Type> (SIZE*SIZE);

	BufferSet<Type, 2> buff1 ({SIZE, SIZE}, data);
	BufferSet<Type, 2> buff2 ({SIZE, SIZE}, data);
	using KernelType = Blur3_k<BufferSet<Type, 2>>;

	recursive_stencil<BufferSet<Type, 2>, Blur3_k<BufferSet<Type, 2>>>( buff1, TIMESTEPS);

	for (int t = 0; t < TIMESTEPS; ++t)
	for (int i = 0; i < SIZE; ++i)
	for (int j = 0; j < SIZE; ++j)
		KernelType::withBonduaries(buff2, i, j, t);


	for (auto i = 1; i < SIZE; i ++)
	for (auto j = 1; j < SIZE; j ++)
		ASSERT_EQ( getElem(buff1, i, j, 1), getElem(buff2, i, j, 1));
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 3D ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TEST(Stencil3D, Translate){

	typedef double Type;
	const int SIZE = 10;

	auto data  = initData<Type> (SIZE*SIZE*SIZE);

	BufferSet<Type, 3> buff1 ({SIZE, SIZE, SIZE}, data);
	BufferSet<Type, 3> buff2 ({SIZE, SIZE, SIZE}, data);

	using KernelType =Translate_3D_k<BufferSet<Type, 3>>;

	recursive_stencil<BufferSet<Type, 3>,KernelType> ( buff1, 1);

	for (auto i = 1; i < SIZE; i ++)
	for (auto j = 1; j < SIZE; j ++)
	for (auto k = 1; k < SIZE; k ++)
		EXPECT_EQ( getElem(buff1, i-1, j-1, k-1, 0), getElem(buff1, i, j, k, 1));

	recursive_stencil<BufferSet<Type, 3>,KernelType> ( buff2, 2);

	for (auto i = 2; i < SIZE; i ++)
	for (auto j = 2; j < SIZE; j ++)
	for (auto k = 2; k < SIZE; k ++)
		ASSERT_EQ( getElem(buff2, i, j, k, 0), getElem(buff2, i-1, j-1, k-1, 1));

	for (auto i = 2; i < SIZE; i ++)
	for (auto j = 2; j < SIZE; j ++)
	for (auto k = 2; k < SIZE; k ++)
		ASSERT_EQ( getElem(buff1, i-2, j-2, k-2, 0), getElem(buff2, i, j, k, 0));

}

TEST(Stencil3D, AVG_3D){

	typedef double Type;
	const int SIZE = 10;
	const int TIMESTEPS = 15;

	auto data  = initData<Type> (SIZE*SIZE*SIZE);

	BufferSet<Type, 3> buff1 ({SIZE, SIZE, SIZE}, data);
	BufferSet<Type, 3> buff2 ({SIZE, SIZE, SIZE}, data);

	using KernelType = Avg_3D_k<BufferSet<Type, 3>>;

	// Recursive
	recursive_stencil<BufferSet<Type, 3>,KernelType>( buff1, TIMESTEPS);

	// iterative
	for (int t = 0; t < TIMESTEPS; ++t)
	for (int i = 0; i < SIZE; ++i)
	for (int j = 0; j < SIZE; ++j)
	for (int k = 0; k < SIZE; ++k)
		KernelType::withBonduaries(buff2, i, j, k, t);

	for (auto i = 1; i < SIZE-1; i ++)
	for (auto j = 1; j < SIZE-1; j ++)
	for (int k = 1; k < SIZE-1; ++k){
//		EXPECT_EQ( getElem(buff1, i, j, k, 0), getElem(buff2, i, j, k, 0)) << "@ (" << i << "," << j << "," << k << ")";
//		EXPECT_EQ( getElem(buff1, i, j, k, 1), getElem(buff2, i, j, k, 1)) << "@ (" << i << "," << j << "," << k << ")";
		ASSERT_NEAR (getElem(buff1, i, j, k, 0), getElem(buff2, i, j, k, 0), 0.0001) << "@ (" << i << "," << j << "," << k << ")";
		ASSERT_NEAR (getElem(buff1, i, j, k, 1), getElem(buff2, i, j, k, 1), 0.0001) << "@ (" << i << "," << j << "," << k << ")";
	}

}


TEST(Stencil3D, Heat_3D){

	typedef double Type;
	const int SIZE = 10;
	const int TIMESTEPS = 10;

	auto data  = initData<Type> (SIZE*SIZE*SIZE);

	BufferSet<Type, 3> recursive ({SIZE, SIZE, SIZE}, data);
	BufferSet<Type, 3> iterative ({SIZE, SIZE, SIZE}, data);

	using KernelType = Heat_3D_k<BufferSet<Type, 3>>;


	// Recursive
	{
		auto t = time_call(recursive_stencil<BufferSet<Type, 3>, Heat_3D_k<BufferSet<Type, 3>>>, recursive, TIMESTEPS);
		std::cout << "recursive: " << t << "ms" <<std::endl;
	}

	// iterative
	{
		auto it = [](BufferSet<Type, 3>& buff, unsigned timeSteps) {
			for (int t = 0; t < timeSteps; ++t)
			for (int i = 0; i < SIZE; ++i)
			for (int j = 0; j < SIZE; ++j)
			for (int k = 0; k < SIZE; ++k)
				KernelType::withBonduaries(buff, i, j, k, t);
		};
		auto t = time_call(it, iterative, TIMESTEPS);
		std::cout << "iterative: " << t << "ms" <<std::endl;
	}

	

	for (auto i = 1; i < SIZE-1; i ++)
	for (auto j = 1; j < SIZE-1; j ++)
	for (int k = 1; k < SIZE-1; ++k){
		EXPECT_NEAR (getElem(recursive, i, j, k, 0), getElem(iterative, i, j, k, 0), 0.0001) << "@ (" << i << "," << j << "," << k << ")";
		EXPECT_NEAR (getElem(recursive, i, j, k, 1), getElem(iterative, i, j, k, 1), 0.0001) << "@ (" << i << "," << j << "," << k << ")";
	}

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 4D ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace {

	template< typename DataStorage> 
	struct Avg_4D_k : public Kernel<DataStorage, 4, Avg_4D_k<DataStorage>>{

		static void withBonduaries (DataStorage& data, int i, int j, int k, int w, unsigned t){

			double fac = 2.0;
	
			if (i == 0 || i == getW(data)-1) { getElem(data, i, j, k, w, t+1) =  getElem (data, i, j, k, w, t); return; }
			if (j == 0 || j == getH(data)-1) { getElem(data, i, j, k, w, t+1) =  getElem (data, i, j, k, w, t); return; }
			if (k == 0 || k == getD(data)-1) { getElem(data, i, j, k, w, t+1) =  getElem (data, i, j, k, w, t); return; }

			getElem(data, i, j, k, w, t+1) = 
					getElem (data, i, j, k + 1, w, t) +
					getElem (data, i, j, k - 1, w, t) +
					getElem (data, i, j + 1, k, w, t) +
					getElem (data, i, j - 1, k, w, t) +
					getElem (data, i + 1, j, k, w, t) +
					getElem (data, i - 1, j, k, w, t)
					- 6.0 * getElem (data, i, j, k, w, t) / (fac*fac);

		//	std::cout << getElem(data, i, j, k, t+1)  << ":" << getElem(data, i, j, k, t) <<  "@ (" << i << "," << j << "," << k << ")" << std::endl;
		}

		//std::pair<int,int> getSlope(unsigned dimension) const{
		//	return {1,-1};
		//}

		static const unsigned int neighbours = 1;
	};

}

TEST(Stencil4D, AVG_4D){

	typedef float Type;
	const int SIZE = 10;
	const int TIMESTEPS = 20;

	auto data  = initData<Type> (SIZE*SIZE*SIZE*SIZE);

	BufferSet<Type, 4> recursive ({SIZE, SIZE, SIZE, SIZE}, data);
	BufferSet<Type, 4> iterative ({SIZE, SIZE, SIZE, SIZE}, data);

	using KernelType = Avg_4D_k<BufferSet<Type, 4>>;

	// Recursive
	{
		auto t = time_call(recursive_stencil<BufferSet<Type, 4>, Avg_4D_k<BufferSet<Type, 4>>>, recursive, TIMESTEPS);
		std::cout << "recursive: " << t << "ms" <<std::endl;
	}

	// iterative
	{
		auto it = [](BufferSet<Type, 4>& buff, unsigned timeSteps) {
			for (int t = 0; t < timeSteps; ++t)
			for (int i = 0; i < SIZE; ++i)
			for (int j = 0; j < SIZE; ++j)
			for (int k = 0; k < SIZE; ++k)
			for (int w = 0; w < SIZE; ++w)
				KernelType::withBonduaries(buff, i, j, k, w, t);
		};
		auto t = time_call(it, iterative, TIMESTEPS);
		std::cout << "iterative: " << t << "ms" <<std::endl;
	}

	for (int i = 1; i < SIZE-1; i ++)
	for (int j = 1; j < SIZE-1; j ++)
	for (int k = 1; k < SIZE-1; ++k)
	for (int w = 1; w < SIZE-1; ++w){
		EXPECT_NEAR (getElem(recursive, i, j, k, w, 0), getElem(iterative, i, j, k, w, 0), 0.0001) << "@ (" << i << "," << j << "," << k << ")";
		EXPECT_NEAR (getElem(recursive, i, j, k, w, 1), getElem(iterative, i, j, k, w, 1), 0.0001) << "@ (" << i << "," << j << "," << k << ")";
	}

}


