#pragma once


#include <array>

#include "tools.h"


namespace stencil{

	/**
	 * A  kernel is a functional class that executes the core of the computation,
	 * It operates on an N dimensional space and requires neigbourhooding data in
	 * the N dimensions. 
	 * Recursive: the number of elements required on each dimension define
	 * the slope of the hyperspaces utiliced to resolve.
	 * Iterative: the number of elements required on each dimension define the halo
	 * or ghost cells to be shared
	 */
	template <typename Data, unsigned Dimensions, typename Parent>
	struct Kernel{

		static const unsigned dimensions = Dimensions;
	};


	#define FOR_DIMENSION(N) \
	template <typename DataStorage, typename Kernel> \
			inline typename std::enable_if< is_eq<Kernel::dimensions, N>::value, void>::type

		FOR_DIMENSION(1)  solve (DataStorage& data, int x, int t){
		}

		FOR_DIMENSION(2)  solve (DataStorage& data, int x, int y, int t){
		}

		FOR_DIMENSION(3)  solve (DataStorage& data, int x, int y, int z, int t){
		}

		FOR_DIMENSION(4)  solve (DataStorage& data, int x, int y, int z, int w, int t){
		}

	#undef FOR_DIMENSION


} // stencil namespace
