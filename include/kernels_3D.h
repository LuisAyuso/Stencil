
#pragma once

#include <cmath>

#include "kernel.h"
#include "bufferSet.h"



namespace stencil{
namespace example_kernels{

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template< typename Elem> 
	struct Translate_3D_k : public Kernel<BufferSet<Elem,3>, 3, Translate_3D_k<Elem>>{

		void operator() (BufferSet<Elem,3>& data, unsigned i, unsigned j, unsigned k, unsigned t) const{
			if (0> (int)i-1)		getElem(data, i, j, k, t+1) = 0;
			else if (0> (int)j-1)	getElem(data, i, j, k, t+1) = 0;
			else if (0> (int)k-1)	getElem(data, i, j, k, t+1) = 0;
			else 					getElem(data, i, j, k, t+1) = getElem(data, i-1, j-1, k-1, t);
		}

		std::pair<int,int> getSlope(unsigned dimension) const{
			return {1,-1};
		}
	};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template< typename Elem> 
	struct Heat_3D_k : public Kernel<BufferSet<Elem,3>, 3, Heat_3D_k<Elem>>{

		void operator() (BufferSet<Elem,3>& data, int i, int j, int k, unsigned t) const{

			double fac = 2.0;
	
			if (i == 0 || i == getW(data)-1) { getElem(data, i, j, k, t+1) =  getElem (data, i, j, k, t); return; }
			if (j == 0 || j == getH(data)-1) { getElem(data, i, j, k, t+1) =  getElem (data, i, j, k, t); return; }
			if (k == 0 || k == getD(data)-1) { getElem(data, i, j, k, t+1) =  getElem (data, i, j, k, t); return; }

			getElem(data, i, j, k, t+1) = 
					getElem (data, i, j, k + 1, t) +
					getElem (data, i, j, k - 1, t) +
					getElem (data, i, j + 1, k, t) +
					getElem (data, i, j - 1, k, t) +
					getElem (data, i + 1, j, k, t) +
					getElem (data, i - 1, j, k, t)
					- 6.0 * getElem (data, i, j, k, t) / (fac*fac);

		//	std::cout << getElem(data, i, j, k, t+1)  << ":" << getElem(data, i, j, k, t) <<  "@ (" << i << "," << j << "," << k << ")" << std::endl;
		}

		std::pair<int,int> getSlope(unsigned dimension) const{
			return {1,-1};
		}
	};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template< typename Elem> 
	struct Avg_3D_k : public Kernel<BufferSet<Elem,3>, 3, Avg_3D_k<Elem>>{

		void operator() (BufferSet<Elem,3>& data, int i, int j, int k, unsigned t) const{

			double fac = 2.0;
	
			if (i == 0 || i == getW(data)-1) { getElem(data, i, j, k, t+1) =  getElem (data, i, j, k, t); return; }
			if (j == 0 || j == getH(data)-1) { getElem(data, i, j, k, t+1) =  getElem (data, i, j, k, t); return; }
			if (k == 0 || k == getD(data)-1) { getElem(data, i, j, k, t+1) =  getElem (data, i, j, k, t); return; }

			getElem(data, i, j, k, t+1) = 
					getElem (data, i, j, k + 1, t) +
					getElem (data, i, j, k - 1, t) +
					getElem (data, i, j + 1, k, t) +
					getElem (data, i, j - 1, k, t) +
					getElem (data, i + 1, j, k, t) +
					getElem (data, i - 1, j, k, t) +
					getElem (data, i, j, k + 1, t) +
					getElem (data, i, j, k - 1, t) +
					getElem (data, i, j + 1, k-1, t) +
					getElem (data, i, j - 1, k-1, t) +
					getElem (data, i + 1, j, k+1, t) +
					getElem (data, i - 1, j, k+1, t) / 12.0;

		//	std::cout << getElem(data, i, j, k, t+1)  << ":" << getElem(data, i, j, k, t) <<  "@ (" << i << "," << j << "," << k << ")" << std::endl;
		}

		std::pair<int,int> getSlope(unsigned dimension) const{
			return {1,-1};
		}
	};

}// example_kernels
}// stencil
