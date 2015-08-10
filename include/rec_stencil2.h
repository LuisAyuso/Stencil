#pragma once


#include <array>

#include "hyperspace.h"
#include "bufferSet.h"
#include "tools.h"


namespace stencil{

	typedef Hyperspace<2> Zoid;


	/**
	 *	 VERSION 2, 
	 *		change the order of splits:
	 *			deltaT == 1 Base case
	 *			other case:
	 *			 if fits: space cut
	 *			 other case : time cut
	*/
				
	template <typename DataStorage, typename Kernel>
	void recursive_stencil_2D_aux(DataStorage& data, Kernel k, const Zoid& z, int t0, int t1){

		static_assert( Kernel::dimensions == 2, "not implemented for anything else yet");

		//std::cout << "zoid: " << z <<  " from  " << t0 << " to " << t1 << std::endl;

		auto deltaT = (int)t1-t0;
		assert(t1 >= t0);
		assert(deltaT >= 0);

		// BASE CASE
		if (deltaT == 1){

			int ia = z.a(0);
			int ib = z.b(0);
			int ja = z.a(1);
			int jb = z.b(1);

			for (int i = ia; i < ib; ++i){
				for (int j = ja; j < jb; ++j){
					k(data, i, j, t0);
				}
			}
		}
		
		else{
	
			auto xa  = z.a(0);
			auto xb  = z.b(0);
			auto dxa = z.da(0);
			auto dxb = z.db(0);
			auto deltaX = MAX(xb - xa, (xb + dxb * deltaT) - (xa + dxa * deltaT));
			auto slopeX = k.getSlope(0);

			auto splitX = xa;
			if (deltaX >= 2*(ABS(slopeX.first)+ABS(slopeX.second))*deltaT){
				splitX = xa + (xb - xa)/2;
			}
		
			auto ya  = z.a(1);
			auto yb  = z.b(1);
			auto dya = z.da(1);
			auto dyb = z.db(1);
			auto deltaY = MAX(yb - ya, (yb + dyb * deltaT) - (ya + dya * deltaT));
			auto slopeY = k.getSlope(1);

			auto splitY = ya;
			if (deltaY >= 2*(ABS(slopeY.first)+ABS(slopeY.second))*deltaT){
				splitY = ya + (yb - ya)/2;
			}

			//std::cout << " x " << xa << " : " << xb << std::endl;
			//std::cout << " slopeX " << slopeX.first << " : " << slopeX.second << std::endl;
			//std::cout << " y " << ya << " : " << yb << std::endl;
			//std::cout << " slopeY " << slopeY.first << " : " << slopeY.second << std::endl;
			//std::cout << " delta " << deltaX << " : " << deltaY << std::endl;
			//std::cout << " split " << splitX <<  " : " << splitY << std::endl;

			auto zoids = z.split_slopes(CutWithSlopes{splitX, slopeX.first, slopeX.second}, CutWithSlopes{splitY, slopeY.first, slopeY.second});

			//if (zoids.size() > 1){
			//	std::cout << "spatial cut: " << z << std::endl;
			//	for (auto subZoid : zoids) std::cout << " - " << subZoid << std::endl;
			//}

			// spatial cut worked, recurse
			if (zoids.size() > 1){
				for (auto subZoid : zoids){
					recursive_stencil_2D_aux(data, k, subZoid, t0, t1);
				}
			}
			// Time cut
			else { // if (deltaT > 1 && deltaX > 0  && deltaY > 0){
				//std::cout << "time cut: " << z << " from " << t0 << " to " << t1 <<std::endl;

				int halfTime = deltaT/2;
				assert(halfTime >= 1);

				//std::cout << " " << z << " from " << t0 << " to " << t0+halfTime <<std::endl;
				recursive_stencil_2D_aux(data, k, z, t0, t0+halfTime);
				Hyperspace<2> upZoid ({xa+dxa*halfTime, ya+dya*halfTime}, 
									  {xb+dxb*halfTime, yb+dyb*halfTime}, 
									  {dxa, dya}, 
									  {dxb, dyb});


				//std::cout << " " << upZoid << " from " << t0+halfTime << " to " << t1 <<std::endl;
				recursive_stencil_2D_aux(data, k, upZoid, t0+halfTime , t1);

			}
		}
	}


	template <typename DataStorage, typename Kernel>
	void recursive_stencil_2D(DataStorage& data, Kernel k, unsigned t){

		std::array<int, Kernel::dimensions> leftSlopes;
		std::array<int, Kernel::dimensions> rightSlopes;

		for (int d = 0; d < Kernel::dimensions; ++d){
			const auto& x = k.getSlope(d);
			leftSlopes[d] = x.first;
			rightSlopes[d] = x.second;
		}

		// well, if the time steps are larger that one full piramid, them we 
		// better cut slices of full computation

		int w = getW(data), h = getH(data);

		Hyperspace<2> z ({0,0}, {w,h}, {0,0}, {0,0} );

		// central piramid!
		recursive_stencil_2D_aux(data, k, z, 0, t);
	}

} // stencil namespace
