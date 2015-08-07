#pragma once


#include <array>

#include "hyperspace.h"
#include "bufferSet.h"
#include "tools.h"


namespace stencil{

	typedef Hyperspace<2> Zoid;

	template <typename DataStorage, typename Kernel>
	void recursive_stencil_2D_aux(DataStorage& data, Kernel k, const Zoid& z, unsigned t0, unsigned t1){

		static_assert( Kernel::dimensions == 2, "not implemented for anything else yet");

//		std::cout << "compute: " << z << std::endl;

		auto deltaT = (int)t1-t0;

		// Spatial cut
	
		auto xa  = z.a(0);
		auto xb  = z.b(0);
		auto dxa = z.da(0);
		auto dxb = z.db(0);
        auto deltaX = MAX(xb - xa, (xb + dxb * deltaT) - (xa + dxa * deltaT));
		auto slopeX = k.getSlope(0);

			assert(xa <= 512);
			assert(xb <= 512);

		auto splitX = xa;
		if (deltaX >= 2*(ABS(slopeX.first)+ABS(slopeX.second))*deltaT){
			splitX = xa + deltaX/2;
		}
	

		auto ya  = z.a(1);
		auto yb  = z.b(1);
		auto dya = z.da(1);
		auto dyb = z.db(1);
        auto deltaY = MAX(yb - ya, (yb + dyb * deltaT) - (ya + dya * deltaT));
		auto slopeY = k.getSlope(1);

			assert(xa <= 512);
			assert(xb <= 512);

		auto splitY = ya;
		if (deltaY >= 2*(ABS(slopeY.first)+ABS(slopeY.second))*deltaT){
			splitY = ya + deltaY/2;
		}


	//	std::cout << deltaX << " : " << deltaY << std::endl;
	//	std::cout << splitX <<  " : " << splitY << std::endl;

		auto zoids = z.split(splitX, splitY);

	//	for (auto subZoid : zoids){
	//		std::cout << " - " << subZoid << std::endl;
	//	}

		// spatial cut worked, recurse
		if (zoids.size() > 1){
			for (auto subZoid : zoids){
				recursive_stencil_2D_aux(data, k, subZoid, t0, t1);
			}
		}
		// Time cut
		else if (deltaT > 40){

			recursive_stencil_2D_aux(data, k, z, t0, deltaT/2);
			Hyperspace<2> upZoid ({xa+dxa*deltaT, ya+dya*deltaT}, {xb, yb}, {dxa, dya}, {dxb, dyb});
			recursive_stencil_2D_aux(data, k, upZoid, deltaT/2 , t1);

		}
		// Base case
		else {

	//		std::cout << "compute zoid: " << z << " from " << t0 << " to " << t1 << std::endl;

			int ia = z.a(0);
			int ib = z.b(0);
			int ja = z.a(1);
			int jb = z.b(1);

			for (unsigned t = t0; t < t1; ++t){
				for (unsigned i = ia; i < ib; ++i){
					for (unsigned j = ja; j < jb; ++j){
						k(data, i, j, t);
					}
				}
				ia += z.da(0);
				ib += z.db(0);
				ja += z.da(1);
				jb += z.db(1);
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
			rightSlopes[d] = -x.second;
		}

		Hyperspace<2> z ({0,0}, {getW(data),getH(data)}, leftSlopes, rightSlopes );

		// Hit it!
		recursive_stencil_2D_aux(data, k, z, 0, t);
	}



} // stencil namespace
