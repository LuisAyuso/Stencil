
#pragma once

#include <cmath>

#include "kernel.h"
#include "bufferSet.h"



namespace stencil{
namespace example_kernels{

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template< typename DataStorage> 
	struct Translate_3D_k : public Kernel<DataStorage, 3, Translate_3D_k<DataStorage>>{

		static void withBonduaries (DataStorage& data, int i, int j, int k, int t) {
			if (0> (int)i-1)		getElem(data, i, j, k, t+1) = 0;
			else if (0> (int)j-1)	getElem(data, i, j, k, t+1) = 0;
			else if (0> (int)k-1)	getElem(data, i, j, k, t+1) = 0;
			else 					getElem(data, i, j, k, t+1) = getElem(data, i-1, j-1, k-1, t);
		}
		static void withoutBonduaries (DataStorage& data, int i, int j, int k, int t) {
			getElem(data, i, j, k, t+1) = getElem(data, i-1, j-1, k-1, t);
		}

		static const unsigned int neighbours = 1;
	};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template< typename DataStorage> 
	struct Heat_3D_k : public Kernel<DataStorage, 3, Heat_3D_k<DataStorage>>{

		static void withBonduaries (DataStorage& data, int i, int j, int k, unsigned t) {

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

		static void withoutBonduaries (DataStorage& data, int i, int j, int k, unsigned t) {

			double fac = 2.0;

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
		static const unsigned int neighbours = 1;
	};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template< typename DataStorage> 
	struct Avg_3D_k : public Kernel<DataStorage, 3, Avg_3D_k<DataStorage>>{

		static void withBonduaries (DataStorage& data, int i, int j, int k, unsigned t) {

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

		static void withoutBonduaries (DataStorage& data, int i, int j, int k, unsigned t) {

			double fac = 2.0;

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

		static const unsigned int neighbours = 1;
	};

}// example_kernels
}// stencil
