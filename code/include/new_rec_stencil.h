#pragma once


#include <array>

#include "hyperspace.h"
#include "bufferSet.h"
#include "tools.h"

#include "dispatch.h"
#include "tools/instrument.h"

#include <thread>
#include <sstream>


#ifndef TIME_CUTOFF 
#  define TIME_CUTOFF 10
#endif 



namespace stencil{


namespace detail {

	#define FOR_DIMENSION(N) \
	template <typename DataStorage, typename Kernel> \
			inline typename std::enable_if< is_eq<Kernel::dimensions, N>::value, void>::type

		FOR_DIMENSION(1) base_case (DataStorage& data, const Kernel& kernel, const Hyperspace<DataStorage::dimensions>& z, int t0, int t1){

			BC_INSTRUMENT(z)

			int ia = z.a(0);
			int ib = z.b(0);

			for (int t = t0; t < t1; ++t){

				for (int i = ia; i < ib; ++i){
						kernel(data, i, t);
				}
				ia += z.da(0);
				ib += z.db(0);
			}

			END_INSTUMENT;
		}

		FOR_DIMENSION(2) base_case (DataStorage& data, const Kernel& kernel, const Hyperspace<DataStorage::dimensions>& z, int t0, int t1){

			BC_INSTRUMENT(z)

			int ia = z.a(0);
			int ib = z.b(0);
			int ja = z.a(1);
			int jb = z.b(1);

			for (int t = t0; t < t1; ++t){

				for (int j = ja; j < jb; ++j){
					for (int i = ia; i < ib; ++i){
						kernel(data, i, j, t);
					}
				}
				ia += z.da(0);
				ib += z.db(0);
				ja += z.da(1);
				jb += z.db(1);
			}
			END_INSTUMENT;
		}

		FOR_DIMENSION(3) base_case (DataStorage& data, const Kernel& kernel, const Hyperspace<DataStorage::dimensions>& z, int t0, int t1){

			BC_INSTRUMENT(z)

			int ia = z.a(0);
			int ib = z.b(0);
			int ja = z.a(1);
			int jb = z.b(1);
			int ka = z.a(2);
			int kb = z.b(2);

			for (int t = t0; t < t1; ++t){

				for (int k = ka; k < kb; ++k){
					for (int j = ja; j < jb; ++j){
						for (int i = ia; i < ib; ++i){
							kernel(data, i, j, k, t);
						}
					}
				}
				ia += z.da(0);
				ib += z.db(0);
				ja += z.da(1);
				jb += z.db(1);
				ka += z.da(2);
				kb += z.db(2);
			}

			END_INSTUMENT;
		}

		FOR_DIMENSION(4) base_case (DataStorage& data, const Kernel& kernel, const Hyperspace<DataStorage::dimensions>& z, int t0, int t1){

			BC_INSTRUMENT(z)

			int ia = z.a(0);
			int ib = z.b(0);
			int ja = z.a(1);
			int jb = z.b(1);
			int ka = z.a(2);
			int kb = z.b(2);
			int wa = z.a(3);
			int wb = z.b(3);

			//int t = t0;
			for (int t = t0; t < t1; ++t){

				for (int w = wa; w < wb; ++w){
					for (int k = ka; k < kb; ++k){
						for (int j = ja; j < jb; ++j){
							for (int i = ia; i < ib; ++i){
								kernel(data, i, j, k, w, t);
							}
						}
					}
				}
				ia += z.da(0);
				ib += z.db(0);
				ja += z.da(1);
				jb += z.db(1);
				ka += z.da(2);
				kb += z.db(2);
				wa += z.da(3);
				wb += z.db(3);
			}
			END_INSTUMENT;
		}


	#undef FOR_DIMENSION


// ~~~~~~~~~~~~~~~~ Reverse Dimension order, split first righmost dimmensions (sparse in memory) ~~~~~~~~~~~~~~~~~~~~~~~~

	template<unsigned N, unsigned Dimensions>
	struct next_dim{
			static const int value = N-1;
	};
	template<unsigned Dimensions>
	struct next_dim<0, Dimensions>{
			static const int value = Dimensions-1;
	};

	template<>
	struct next_dim<0, 1>{
			static const unsigned value = 0;
	};


// ~~~~~~~~~~~~~~~~ multiple splits routine ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template <typename DataStorage, typename Kernel, int Dim>
	inline void recursive_stencil_A(DataStorage& data, const Kernel& k, const Hyperspace<DataStorage::dimensions>& z, int t0, int t1);

	template <typename DataStorage, typename Kernel, int Dim>
	inline void recursive_stencil_B(DataStorage& data, const Kernel& k, const Hyperspace<DataStorage::dimensions>& z, int t0, int t1);

	// When changing dimension, it is important to see what geometry the hyperspace has, to use the appropiate recursive call
	template <typename DataStorage, typename Kernel, int Dim>
	inline void recursive_stencil_dispatch(DataStorage& data, const Kernel& k, const Hyperspace<DataStorage::dimensions>& z, int t0, int t1){
		const auto da = z.da(Dim);
		const auto db = z.db(Dim);
		if (da < db) {
			recursive_stencil_B<DataStorage, Kernel, Dim> (data, k, z, t0, t1);
		}
		else{
			recursive_stencil_A<DataStorage, Kernel, Dim> (data, k, z, t0, t1);
		}
	}

	// This function handles hyperspaces with wider base
	template <typename DataStorage, typename Kernel, int Dim>
	inline void recursive_stencil_A(DataStorage& data, const Kernel& k, const Hyperspace<DataStorage::dimensions>& z, int t0, int t1){

		typedef Hyperspace<DataStorage::dimensions> Target_Hyperspace;
		constexpr auto NextDim = next_dim<Dim, Target_Hyperspace::dimensions>::value;

		//std::cout << "A-" << z << " t(" << t0 << "," << t1 << ") DIM:" << Dim << std::endl;

		const auto deltaT = t1-t0;
		const auto a  = z.a(Dim);
		const auto b  = z.b(Dim);
		const auto da = z.da(Dim);
		const auto db = z.db(Dim);
		const auto deltaBase = b - a;
		const auto slopeDim = k.getSlope(Dim);

		assert(da >= db);

	 //std::cout << "         deltaT: "   <<  deltaT  << std::endl;
     //std::cout << "         a: "        <<  a   << std::endl;
     //std::cout << "         b: "        <<  b   << std::endl;
     //std::cout << "         da: "       <<  da  << std::endl;
     //std::cout << "         db: "       <<  db  << std::endl;
     //std::cout << "         deltaBase: "<<  deltaBase << std::endl;
     //std::cout << "         slopeLeft: "<<  slopeDim.first << std::endl;
     //std::cout << "         slopeRight: "<<  slopeDim.second << std::endl;


		// spatial cut (this case cuts in M)
		if (deltaBase >= 2*(ABS(slopeDim.first)+ABS(slopeDim.second))*deltaT){
			const auto cut = (deltaBase /2);
			const auto& subSpaces  = Target_Hyperspace::template split_M<Dim> (a+cut, z, slopeDim.first, slopeDim.second);

			SPAWN ( left, (recursive_stencil_A<DataStorage, Kernel, Dim>), data, k, subSpaces[0], t0, t1);
			recursive_stencil_A<DataStorage, Kernel, Dim>( data, k, subSpaces[1], t0, t1);
			SYNC(left);

			recursive_stencil_B<DataStorage, Kernel, Dim>( data, k, subSpaces[2], t0, t1);
		}
		else if (Dim != 0){
			recursive_stencil_dispatch<DataStorage, Kernel, NextDim>( data, k, z, t0, t1);
		}
		// time cut
		else if (deltaT > TIME_CUTOFF){

			const int halfTime = deltaT/2;
			assert(halfTime >= 1);
			//std::cout << " time cut " << "(" << t0 << "," << halfTime+t0 << "](" << halfTime+t0 << "," << t1 << "]" << std::endl;

			recursive_stencil_A <DataStorage, Kernel, Dim>(data, k, z, t0, t0+halfTime);

			// We must update all the dimensions as we move in time.... 
			auto upZoid = z;
			for (auto d = 0; d < z.dimensions; ++d){
				upZoid.a(d) = z.a(d) + z.da(d)*halfTime;
				upZoid.b(d) = z.b(d) + z.db(d)*halfTime;
			}
			upZoid.increaseStep();

			recursive_stencil_A<DataStorage, Kernel, Dim>(data, k, upZoid, t0+halfTime, t1);
		}
		else{
			//std::cout << " Base Case: " << z << std::endl;
			base_case (data, k, z, t0, t1);
		}
	}


	// This function handles hyperspaces with wider top (inverted pyramid)
	template <typename DataStorage, typename Kernel, int Dim>
	inline void recursive_stencil_B(DataStorage& data, const Kernel& k, const Hyperspace<DataStorage::dimensions>& z, int t0, int t1){

		typedef Hyperspace<DataStorage::dimensions> Target_Hyperspace;
		constexpr auto NextDim = next_dim<Dim, Target_Hyperspace::dimensions>::value;

		//std::cout << "B-" << z << " t(" << t0 << "," << t1 << ") " << Dim << std::endl;

		const auto deltaT = t1-t0;
		const auto a  = z.a(Dim);
		const auto b  = z.b(Dim);
		const auto da = z.da(Dim);
		const auto db = z.db(Dim);
		const auto deltaBase = b - a;
		const auto deltaTop = (b + db * deltaT) - (a + da * deltaT);
		const auto slopeDim = k.getSlope(Dim);

		assert(da < db);
		
		// spatial cut (this case cuts in W)
		if (deltaTop >= 2*(ABS(slopeDim.first)+ABS(slopeDim.second))*deltaT){

			//std::cout << " cut in W " << std::endl;
			const auto& subSpaces  = Target_Hyperspace::template split_W<Dim> (z, slopeDim.first, slopeDim.second);
			assert(subSpaces.size() == 3);

			recursive_stencil_A<DataStorage, Kernel, Dim> (data, k, subSpaces[0], t0, t1);

			SPAWN ( left, (recursive_stencil_B<DataStorage, Kernel, Dim>), data, k, subSpaces[1], t0, t1);
			recursive_stencil_B<DataStorage, Kernel, Dim>( data, k, subSpaces[2], t0, t1);
			SYNC(left);

		}
		else if (Dim != 0){
			recursive_stencil_dispatch<DataStorage, Kernel, NextDim>( data, k, z, t0, t1);
		}
		// time cut
		else if (deltaT > TIME_CUTOFF){


			// little optimization, if the base of the hyp in this dimmension is 0, we can skip it and improve spatial cut chances by shifting the piramid
			// by one
			const int halfTime = (a!=b) ? deltaT>>2 : (deltaT>>2)+1;
			assert(halfTime >= 1 && halfTime < deltaT);

			//std::cout << " time cut " << "(" << t0 << "," << halfTime+t0 << "](" << halfTime+t0 << "," << t1 << "]" << std::endl;
			recursive_stencil_dispatch<DataStorage, Kernel, NextDim>(data, k, z, t0, t0+halfTime);

			// We must update all the dimensions as we move in time.... 
			auto upZoid = z;
			for (auto d = 0; d < z.dimensions; ++d){
				upZoid.a(d) = z.a(d) + z.da(d)*halfTime;
				upZoid.b(d) = z.b(d) + z.db(d)*halfTime;
			}
			upZoid.increaseStep();

			recursive_stencil_dispatch<DataStorage, Kernel, NextDim>(data, k, upZoid, t0+halfTime, t1);
		}
		else {

			//std::cout << "BASECASE: " << z << " t(" << t0 << "," << t1 << ") " << std::endl;
			base_case (data, k, z, t0, t1);
		}
	}


} // detail

// ~~~~~~~~~~~~~~~~ Recursive stencil entry point  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template <typename DataStorage, typename Kernel>
	void recursive_stencil(DataStorage& data, const Kernel& k, unsigned t){

		PARALLEL_CTX ({

			// notice that the original piramid has perfect vertical sides
			auto z = data.getGlobalHyperspace();

			(detail::recursive_stencil_A<DataStorage, Kernel, Kernel::dimensions-1>)(data, k, z, 0, t);

		});
	}

} // stencil namespace
