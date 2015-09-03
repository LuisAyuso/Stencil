
#pragma once

#include <cmath>

#include "kernel.h"
#include "bufferSet.h"



namespace stencil{
namespace example_kernels{


		template< typename DataStorage> 
		struct Avg_1D_k : public Kernel<DataStorage, 1, Avg_1D_k<DataStorage>>{

			void operator() (DataStorage& data, int i, int t) const{

				auto left  = i>0? getElem(data, i, t%2): 0;
				auto me    = getElem(data, i, t%2);
				auto right = i<data.getSize()? getElem(data, i, t%2): 0;

				getElem(data, i, (t+1)%2) = (left + right + me) / 3.0;
			}

			inline std::pair<int,int> getSlope(unsigned dimension) const{
				return {1,-1};
			}
		};

}// example_kernels
}// stencil
