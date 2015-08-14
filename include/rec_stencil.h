#pragma once


#include <array>

#include "hyperspace.h"
#include "bufferSet.h"
#include "tools.h"


namespace stencil{

	#define CUT 10

	/**
	 *	 VERSION 2, 
	 *		change the order of splits:
	 *			deltaT == 1 Base case
	 *			other case:
	 *			 if fits: space cut
	 *			 other case : time cut
	*/
				
	template <typename DataStorage, typename Kernel, unsigned Dim>
	void recursive_stencil_aux(DataStorage& data, Kernel k, const Hyperspace<DataStorage::dimensions>& z, int t0, int t1){

		typedef Hyperspace<DataStorage::dimensions> Target_Hyperspace;

		//std::cout << "zoid: " << z <<  " from  " << t0 << " to " << t1 << std::endl;

		auto deltaT = (int)t1-t0;
		assert(t1 >= t0);
		assert(deltaT >= 0);

		// BASE CASE
		if (deltaT <= 3){

			int ia = z.a(0);
			int ib = z.b(0);
			int ja = z.a(1);
			int jb = z.b(1);

			//int t = t0;
			for (int t = t0; t < t1; ++t){

				for (int j = ja; j < jb; ++j){
					for (int i = ia; i < ib; ++i){
						k(data, i, j, t);
					}
				}
				ia += z.da(0);
				ib += z.db(0);
				ja += z.da(1);
				jb += z.db(1);
			}

		}
		
		else{

			auto a  = z.a(Dim);
			auto b  = z.b(Dim);
			auto da = z.da(Dim);
			auto db = z.db(Dim);
			auto deltaBase = b - a;
			auto deltaTop = (b + db * deltaT) - (a + da * deltaT);
			auto slopeDim = k.getSlope(Dim);

			//std::cout << " a:" << a << " b: "<< b << " da:" << da << " db:" << db << std::endl;
			//std::cout << " deltaBase:" << deltaBase << " deltaTop:" << deltaTop <<  " deltaT:" << deltaT<< std::endl;
			//std::cout << " M cut: " << 2*(ABS(slopeDim.first)+ABS(slopeDim.second))*deltaT << std::endl;
			//std::cout << " W cut: " << 2*(ABS(slopeDim.first)+ABS(slopeDim.second))*deltaT << std::endl;

			// Cut in M
			if (deltaBase >= CUT + 2*(ABS(slopeDim.first)+ABS(slopeDim.second))*deltaT){

				auto split = a + deltaBase /2;

				//std::cout << " cut in M " << split << std::endl;
				const auto& subSpaces  = Target_Hyperspace::template split_M<Dim> (split, z, slopeDim.first, slopeDim.second);

				assert(subSpaces.size() == 3);

				recursive_stencil_aux<DataStorage, Kernel, (Dim+1)%Kernel::dimensions> (data, k, subSpaces[0], t0, t1);
				recursive_stencil_aux<DataStorage, Kernel, (Dim+1)%Kernel::dimensions> (data, k, subSpaces[1], t0, t1);
				recursive_stencil_aux<DataStorage, Kernel, (Dim+1)%Kernel::dimensions> (data, k, subSpaces[2], t0, t1);
				
			}
			// Cut in W
			else if (deltaTop >= CUT + 2*(ABS(slopeDim.first)+ABS(slopeDim.second))*deltaT){ 

				//std::cout << " cut in M " << std::endl;
				const auto& subSpaces  = Target_Hyperspace::template split_W<Dim> (z, slopeDim.first, slopeDim.second);
				assert(subSpaces.size() == 3);

				recursive_stencil_aux<DataStorage, Kernel, (Dim+1)%Kernel::dimensions> (data, k, subSpaces[0], t0, t1);
				recursive_stencil_aux<DataStorage, Kernel, (Dim+1)%Kernel::dimensions> (data, k, subSpaces[1], t0, t1);
				recursive_stencil_aux<DataStorage, Kernel, (Dim+1)%Kernel::dimensions> (data, k, subSpaces[2], t0, t1);
			}
			// Time cut
			else { // if (deltaT > 1 && deltaX > 0  && deltaY > 0){
				//std::cout << "time cut: " << z << " from " << t0 << " to " << t1 <<std::endl;

				int halfTime = deltaT/2;
				assert(halfTime >= 1);

				//std::cout << " t1: " << z << " from " << t0 << " to " << t0+halfTime <<std::endl;
				recursive_stencil_aux<DataStorage, Kernel, Dim>(data, k, z, t0, t0+halfTime);

				// We must update all the dimensions as we move in time.... 
				auto upZoid = z;
				for (auto d = 0; d < z.dimensions; ++d){
					upZoid.a(d) = z.a(d) + z.da(d)*halfTime;
					upZoid.b(d) = z.b(d) + z.db(d)*halfTime;
				}

				//std::cout << " t2:" << upZoid << " from " << t0+halfTime << " to " << t1 <<std::endl;
				recursive_stencil_aux<DataStorage, Kernel, Dim>(data, k, upZoid, t0+halfTime , t1);
			}
		}
	}


	template <typename DataStorage, typename Kernel>
	void recursive_stencil_2D(DataStorage& data, Kernel k, unsigned t){

		int w = getW(data), h = getH(data);

		std::array<int, Kernel::dimensions> leftSlopes;
		std::array<int, Kernel::dimensions> rightSlopes;

		// notice that the original piramid has perfect vertical sides
		Hyperspace<2> z ({0,0}, {w,h}, {0,0}, {0,0} );

		auto z2 = data.getGlobalHyperspace();

		std::cout << z << std::endl;
		std::cout << z2 << std::endl;

		abort();

		recursive_stencil_aux<DataStorage, Kernel, 0>(data, k, z, 0, t);
	}

} // stencil namespace
