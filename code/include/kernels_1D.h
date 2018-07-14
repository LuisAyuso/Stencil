
#pragma once

#include <cmath>

#include "kernel.h"
#include "bufferSet.h"



namespace stencil{
namespace example_kernels{


		template< typename DataStorage> 
		struct Avg_1D_k : public Kernel<DataStorage, 1, Avg_1D_k<DataStorage>>{

			static void withBonduaries (DataStorage& data, int i, int t) {

				auto left  = i>0? getElem(data, i, t%2): 0;
				auto me    = getElem(data, i, t%2);
				auto right = i<data.getSize()? getElem(data, i, t%2): 0;

				getElem(data, i, (t+1)%2) = (left + right + me) / 3.0;
			}

			static void withoutBonduaries (DataStorage& data, int i, int t) {

				auto left  = getElem(data, i, t%2);
				auto me    = getElem(data, i, t%2);
				auto right = getElem(data, i, t%2);

				getElem(data, i, (t+1)%2) = (left + right + me) / 3.0;
			}

			static const unsigned int neighbours = 1;
		};

}// example_kernels
}// stencil
