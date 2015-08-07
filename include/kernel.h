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


		#define FOR_DIMENSION(N) \
			std::enable_if< is_eq<Dimensions, N>::value, void>

		FOR_DIMENSION(1) operator() (Data& data, unsigned i, unsigned t){
			static_cast<Parent*>(this)->operator() (data, i, t);
		}
		FOR_DIMENSION(2) operator() (Data& data, unsigned i, unsigned j, unsigned t){
			static_cast<Parent*>(this)->operator() (data, i, t);
		}
		FOR_DIMENSION(3) operator() (Data& data, unsigned i, unsigned j, unsigned k, unsigned t){
			static_cast<Parent*>(this)->operator() (data, i, t);
		}

		#undef FOR_DIMENSION
		
		std::pair<int,int> getSlope(unsigned dimension){
			return static_cast<Parent*>(this)->getSlope(dimension);
		}


		static const unsigned dimensions = Dimensions;

	};



} // stencil namespace
