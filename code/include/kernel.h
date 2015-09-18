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
	template <bool WithBonduaries, typename KernelType, typename DataStorage> \
			inline typename std::enable_if< is_eq<KernelType::dimensions, N>::value, void>::type

		FOR_DIMENSION(1)  solve (DataStorage& data, int x, int t){
			static_assert(sizeof(KernelType) == 1, "no fields allowed in kernel type");
			static_assert( std::is_function<decltype(KernelType::withoutBonduaries)>::value, "kernel has no version without bounduaries");
			if (WithBonduaries) KernelType::withBonduaries(data, x, t);
			else				KernelType::withoutBonduaries(data, x, t);
		}

		FOR_DIMENSION(2)  solve (DataStorage& data, int x, int y, int t){
			static_assert(sizeof(KernelType) == 1, "no fields allowed in kernel type");
			static_assert( std::is_function<decltype(KernelType::withoutBonduaries)>::value, "kernel has no version without bounduaries");
			if (WithBonduaries) KernelType::withBonduaries(data, x, y, t);
			else				KernelType::withoutBonduaries(data, x, y ,t);
		}

		FOR_DIMENSION(3)  solve (DataStorage& data, int x, int y, int z, int t){
			static_assert(sizeof(KernelType) == 1, "no fields allowed in kernel type");
			static_assert( std::is_function<decltype(KernelType::withoutBonduaries)>::value, "kernel has no version without bounduaries");
			if (WithBonduaries) KernelType::withBonduaries(data, x, y, z, t);
			else				KernelType::withoutBonduaries(data, x, y, z, t);
		}

		FOR_DIMENSION(4)  solve (DataStorage& data, int x, int y, int z, int w, int t){
			static_assert(sizeof(KernelType) == 1, "no fields allowed in kernel type");
			static_assert( std::is_function<decltype(KernelType::withoutBonduaries)>::value, "kernel has no version without bounduaries");

			if (WithBonduaries) KernelType::withBonduaries(data, x, y, z, w, t);
			else				KernelType::withoutBonduaries(data, x, y, z, w, t);
		}

	#undef FOR_DIMENSION


} // stencil namespace
