#include <gtest/gtest.h>

#include "kernel.h"
#include "rec_stencil.h"
#include "kernels_2D.h"

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


TEST(Stencil2D, Copy){

	typedef int Type;

	auto data  = initData<Type> (10*10);

	BufferSet<Type, 2> buff1 ({10, 10}, data);

	Copy_k<Type> kernel;

	recursive_stencil<BufferSet<Type, 2>, Copy_k<Type>>( buff1, kernel, 1);

	for (auto i = 0; i < 10; i ++)
	for (auto j = 0; j < 10; j ++)
		EXPECT_EQ( getElem(buff1, i, j, 0), getElem(buff1, i, j, 1));


	recursive_stencil<BufferSet<Type, 2>, Copy_k<Type>>( buff1, kernel, 10);

	for (auto i = 0; i < 10; i ++)
	for (auto j = 0; j < 10; j ++)
		EXPECT_EQ( getElem(buff1, i, j, 0), getElem(buff1, i, j, 1));
}


namespace {

	template< typename Elem> 
	struct Translate_k : public Kernel<BufferSet<Elem,2>, 2, Translate_k<Elem>>{

		void operator() (BufferSet<Elem,2>& data, unsigned i, unsigned j, unsigned t){
			if (0> (int)i-1)		getElem(data, i, j, t+1) = 0;
			else if (0> (int)j-1)	getElem(data, i, j, t+1) = 0;
			else 					getElem(data, i, j, t+1) = getElem(data, i-1, j-1, t);
		}

		std::pair<int,int> getSlope(unsigned dimension){
			return {1,-1};
		}
	};

}


TEST(Stencil2D, Translate){

	typedef int Type;
	const int SIZE = 10;

	auto data  = initData<Type> (SIZE*SIZE);

	BufferSet<Type, 2> buff1 ({SIZE, SIZE}, data);
	BufferSet<Type, 2> buff2 ({SIZE, SIZE}, data);


	Translate_k<Type> kernel;

	recursive_stencil<BufferSet<Type, 2>, Translate_k<Type>>( buff1, kernel, 1);

	for (auto i = 1; i < SIZE; i ++)
	for (auto j = 1; j < SIZE; j ++)
		EXPECT_EQ( getElem(buff1, i-1, j-1, 0), getElem(buff1, i, j, 1));


	recursive_stencil<BufferSet<Type, 2>, Translate_k<Type>>( buff2, kernel, 2);

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


	Blur3_k<Type> kernel;

	recursive_stencil<BufferSet<Type, 2>, Blur3_k<Type>>( buff1, kernel, 1);


	for (int i = 0; i < SIZE; ++i)
	for (int j = 0; j < SIZE; ++j)
		kernel(buff2, i, j, 0);


	for (auto i = 1; i < SIZE; i ++)
	for (auto j = 1; j < SIZE; j ++)
		EXPECT_EQ( getElem(buff1, i, j, 1), getElem(buff2, i, j, 1));
}

TEST(Stencil2D, Blur10){

	typedef double Type;
	const int SIZE = 100;

	auto data  = initData<Type> (SIZE*SIZE);

	BufferSet<Type, 2> buff1 ({SIZE, SIZE}, data);
	BufferSet<Type, 2> buff2 ({SIZE, SIZE}, data);


	Blur3_k<Type> kernel;

	recursive_stencil<BufferSet<Type, 2>, Blur3_k<Type>>( buff1, kernel, 10);


	for (int t = 0; t < 10; ++t)
	for (int i = 0; i < SIZE; ++i)
	for (int j = 0; j < SIZE; ++j)
		kernel(buff2, i, j, t);


	for (auto i = 1; i < SIZE; i ++)
	for (auto j = 1; j < SIZE; j ++)
		EXPECT_EQ( getElem(buff1, i, j, 1), getElem(buff2, i, j, 1));
}



namespace {

	template< typename Elem> 
	struct Translate_3d_k : public Kernel<BufferSet<Elem,3>, 3, Translate_3d_k<Elem>>{

		void operator() (BufferSet<Elem,3>& data, unsigned i, unsigned j, unsigned k, unsigned t){
			if (0> (int)i-1)		getElem(data, i, j, k, t+1) = 0;
			else if (0> (int)j-1)	getElem(data, i, j, k, t+1) = 0;
			else if (0> (int)k-1)	getElem(data, i, j, k, t+1) = 0;
			else 					getElem(data, i, j, k, t+1) = getElem(data, i-1, j-1, k-1, t);
		}

		std::pair<int,int> getSlope(unsigned dimension){
			return {1,-1};
		}
	};

}

TEST(Stencil3D, Translate){

	typedef double Type;
	const int SIZE = 10;

	auto data  = initData<Type> (SIZE*SIZE*SIZE);

	BufferSet<Type, 3> buff1 ({SIZE, SIZE, SIZE}, data);
	BufferSet<Type, 3> buff2 ({SIZE, SIZE, SIZE}, data);


	Translate_3d_k<Type> kernel;

	recursive_stencil( buff1, kernel, 1);

	for (auto i = 1; i < SIZE; i ++)
	for (auto j = 1; j < SIZE; j ++)
	for (auto k = 1; k < SIZE; k ++)
		EXPECT_EQ( getElem(buff1, i-1, j-1, k-1, 0), getElem(buff1, i, j, k, 1));


	recursive_stencil( buff2, kernel, 2);

	for (auto i = 2; i < SIZE; i ++)
	for (auto j = 2; j < SIZE; j ++)
	for (auto k = 2; k < SIZE; k ++)
		EXPECT_EQ( getElem(buff2, i, j, k, 0), getElem(buff2, i-1, j-1, k-1, 1));

	for (auto i = 2; i < SIZE; i ++)
	for (auto j = 2; j < SIZE; j ++)
	for (auto k = 2; k < SIZE; k ++)
		EXPECT_EQ( getElem(buff1, i-2, j-2, k-2, 0), getElem(buff2, i, j, k, 0));

}



namespace {

	template< typename Elem> 
	struct Heat_3d_k : public Kernel<BufferSet<Elem,3>, 3, Heat_3d_k<Elem>>{

		void operator() (BufferSet<Elem,3>& data, int i, int j, int k, unsigned t){

			double fac = 1.0;
	
			if (i == 0 || i == getW(data)-1) { getElem(data, i, j, k, t+1) =  getElem (data, i, j, k); return; }
			if (j == 0 || j == getH(data)-1) { getElem(data, i, j, k, t+1) =  getElem (data, i, j, k); return; }
			if (k == 0 || k == getD(data)-1) { getElem(data, i, j, k, t+1) =  getElem (data, i, j, k); return; }

			getElem(data, i, j, k, t+1) = 
					getElem (data, i, j, k + 1, t) +
					getElem (data, i, j, k - 1, t) +
					getElem (data, i, j + 1, k, t) +
					getElem (data, i, j - 1, k, t) +
					getElem (data, i + 1, j, k, t) +
					getElem (data, i - 1, j, k, t)
					- 6.0 * getElem (data, i, j, k, t) / (fac*fac);

			std::cout << getElem(data, i, j, k, t+1) <<  "@ (" << i << "," << j << "," << k << ")" << std::endl;
		}

		std::pair<int,int> getSlope(unsigned dimension){
			return {1,-1};
		}
	};

}

TEST(Stencil3D, Heat_3D){

	typedef double Type;
	const int SIZE = 4;
	const int TIMESTEPS = 1;

	auto data  = initData<Type> (SIZE*SIZE*SIZE);

	BufferSet<Type, 3> buff1 ({SIZE, SIZE, SIZE}, data);
	BufferSet<Type, 3> buff2 ({SIZE, SIZE, SIZE}, data);

	Heat_3d_k<Type> kernel;


	std::cout << buff1  << std::endl;

	// Recursive
	recursive_stencil( buff1, kernel, TIMESTEPS);

	// iterative
	for (int t = 0; t < TIMESTEPS; ++t)
	for (int i = 0; i < SIZE; ++i)
	for (int j = 0; j < SIZE; ++j)
	for (int k = 0; k < SIZE; ++k)
		kernel(buff2, i, j, k, t);

	

	for (auto i = 1; i < SIZE-1; i ++)
	for (auto j = 1; j < SIZE-1; j ++)
	for (int k = 1; k < SIZE-1; ++k){
//		EXPECT_EQ( getElem(buff1, i, j, k, 0), getElem(buff2, i, j, k, 0)) << "@ (" << i << "," << j << "," << k << ")";
//		EXPECT_EQ( getElem(buff1, i, j, k, 1), getElem(buff2, i, j, k, 1)) << "@ (" << i << "," << j << "," << k << ")";
		ASSERT_NEAR (getElem(buff1, i, j, k, 0), getElem(buff2, i, j, k, 0), 0.0001) << "@ (" << i << "," << j << "," << k << ")";
		ASSERT_NEAR (getElem(buff1, i, j, k, 1), getElem(buff2, i, j, k, 1), 0.0001) << "@ (" << i << "," << j << "," << k << ")";
	}

}


