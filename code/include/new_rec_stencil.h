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
	template <typename DataStorage, typename KernelType, unsigned Dim, bool WithBounds=true> \
			inline typename std::enable_if< is_eq<Dim, N>::value, void>::type

		FOR_DIMENSION(1) base_case (DataStorage& data, const Hyperspace<DataStorage::dimensions>& z, int t0, int t1){

			BC_INSTRUMENT(z)

			int ia = z.a(0);
			int ib = z.b(0);

			for (int t = t0; t < t1; ++t){

				for (int i = ia; i < ib; ++i){
					solve<WithBounds, KernelType, DataStorage> (data, i, t);
				}
				ia += z.da(0);
				ib += z.db(0);
			}

			END_INSTUMENT;
		}

		FOR_DIMENSION(2) base_case (DataStorage& data, const Hyperspace<DataStorage::dimensions>& z, int t0, int t1){

			BC_INSTRUMENT(z)

			int ia = z.a(0);
			int ib = z.b(0);
			int ja = z.a(1);
			int jb = z.b(1);

			for (int t = t0; t < t1; ++t){

				for (int j = ja; j < jb; ++j){
					for (int i = ia; i < ib; ++i){
						solve<WithBounds, KernelType, DataStorage> (data, i, j, t);
					}
				}
				ia += z.da(0);
				ib += z.db(0);
				ja += z.da(1);
				jb += z.db(1);
			}
			END_INSTUMENT;
		}

		FOR_DIMENSION(3) base_case (DataStorage& data, const Hyperspace<DataStorage::dimensions>& z, int t0, int t1){

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
							solve<WithBounds, KernelType, DataStorage> (data, i, j, k, t);
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

		FOR_DIMENSION(4) base_case (DataStorage& data, const Hyperspace<DataStorage::dimensions>& z, int t0, int t1){

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
								solve<WithBounds, KernelType, DataStorage> (data, i, j, k, w, t);
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

#define REMOVE_BOUND(x) x&(~(1<<Dim))
	typedef unsigned char Bound_flags;


	template <typename DataStorage, typename Kernel, int Dim>
	inline void recursive_stencil_A(DataStorage& data, const Hyperspace<DataStorage::dimensions>& z, int t0, int t1, Bound_flags leftB, Bound_flags rightB);

	template <typename DataStorage, typename Kernel, int Dim>
	inline void recursive_stencil_B(DataStorage& data, const Hyperspace<DataStorage::dimensions>& z, int t0, int t1, Bound_flags leftB, Bound_flags rightB);

	// When changing dimension, it is important to see what geometry the hyperspace has, to use the appropiate recursive call
	template <typename DataStorage, typename Kernel, int Dim>
	inline void recursive_stencil_dispatch(DataStorage& data, const Hyperspace<DataStorage::dimensions>& z, int t0, int t1, Bound_flags leftB, Bound_flags rightB){
		const auto da = z.da(Dim);
		const auto db = z.db(Dim);

		if (da <= db) {
			recursive_stencil_B<DataStorage, Kernel, Dim> (data, z, t0, t1, leftB, rightB);
		}
		else{
			recursive_stencil_A<DataStorage, Kernel, Dim> (data, z, t0, t1, leftB, rightB);
		}
	}

	// This function handles hyperspaces with flat bonduaries, is the entry point.
	template <typename DataStorage, typename Kernel, int Dim>
	inline void recursive_stencil_Z(DataStorage& data, const Hyperspace<DataStorage::dimensions>& z, int t0, int t1, Bound_flags leftB, Bound_flags rightB){

		typedef Hyperspace<DataStorage::dimensions> Target_Hyperspace;
		constexpr auto NextDim = next_dim<Dim, Target_Hyperspace::dimensions>::value;

		//std::cout << "Z-" << z << " t(" << t0 << "," << t1 << ") d" << Dim << " lB" << (int)leftB << " rB" << (int)rightB << std::endl;

		const auto deltaT = t1-t0;
		const auto a  = z.a(Dim);
		const auto b  = z.b(Dim);
		const auto da = z.da(Dim);
		const auto db = z.db(Dim);
		const auto deltaBase = b - a;
		const auto neighbours = Kernel::neighbours;
		assert(da == db);

		// spatial cut (this case cuts in M)
		if (deltaBase >= 2*neighbours*deltaT){

			const auto cut = (deltaBase /2);
			//std::cout << " cut in M @" << cut << std::endl;
			const auto& subSpaces  = Target_Hyperspace::template split_M2<Dim, neighbours> (a+cut, z);

			//std::cout << "   			- " << subSpaces[0] << std::endl;
			//std::cout << "   			- " << subSpaces[1] << std::endl;
			//std::cout << "   			- " << subSpaces[2] << std::endl;

			SPAWN ( left, (recursive_stencil_A<DataStorage, Kernel, Dim>), data, subSpaces[0], t0, t1, leftB, REMOVE_BOUND(leftB));
			recursive_stencil_A<DataStorage, Kernel, Dim>( data, subSpaces[1], t0, t1, REMOVE_BOUND(leftB), rightB);
			SYNC(left);

			recursive_stencil_B<DataStorage, Kernel, Dim>( data, subSpaces[2], t0, t1, leftB, rightB);
		}
		// time cut
		else if (deltaT > TIME_CUTOFF){

			const int halfTime = deltaT/2;
			assert(halfTime >= 1);
			//std::cout << " time cut " << "(" << t0 << "," << halfTime+t0 << "](" << halfTime+t0 << "," << t1 << "]" << std::endl;

			recursive_stencil_Z <DataStorage, Kernel, NextDim>(data, z, t0, t0+halfTime, leftB, rightB);

			// We must update all the dimensions as we move in time.... 
			auto upZoid = z;
			for (auto d = 0; d < z.dimensions; ++d){
				upZoid.a(d) = z.a(d) + z.da(d)*halfTime;
				upZoid.b(d) = z.b(d) + z.db(d)*halfTime;
			}

			recursive_stencil_Z<DataStorage, Kernel, NextDim>(data, upZoid, t0+halfTime, t1, leftB, rightB);
		}
		else{
			//std::cout << "					BASECASE: " << z << " t(" << t0 << "," << t1 << ") lB" << (int)leftB << " rB" << (int)rightB  << std::endl;
			base_case<DataStorage, Kernel, Target_Hyperspace::dimensions, true> (data, z, t0, t1);
		}
	}


	// This function handles hyperspaces with wider base
	template <typename DataStorage, typename Kernel, int Dim>
	inline void recursive_stencil_A(DataStorage& data, const Hyperspace<DataStorage::dimensions>& z, int t0, int t1, Bound_flags leftB, Bound_flags rightB){

		typedef Hyperspace<DataStorage::dimensions> Target_Hyperspace;
		constexpr auto NextDim = next_dim<Dim, Target_Hyperspace::dimensions>::value;

		//std::cout << "A-" << z << " t(" << t0 << "," << t1 << ") d" << Dim << " lB" << (int)leftB << " rB" << (int)rightB << std::endl;

		const auto deltaT = t1-t0;
		const auto a  = z.a(Dim);
		const auto b  = z.b(Dim);
		const auto da = z.da(Dim);
		const auto db = z.db(Dim);
		const auto deltaBase = b - a;
		const auto neighbours = Kernel::neighbours;
		assert(da > db);

		// spatial cut (this case cuts in M)
		if (deltaBase >= 2*2*neighbours*deltaT){
			const auto cut = (deltaBase /2);
			//std::cout << " cut in M @" << cut << std::endl;
			const auto& subSpaces  = Target_Hyperspace::template split_M2<Dim, neighbours> (a+cut, z);

			//std::cout << "   			- " << subSpaces[0] << std::endl;
			//std::cout << "   			- " << subSpaces[1] << std::endl;
			//std::cout << "   			- " << subSpaces[2] << std::endl;

			SPAWN ( left, (recursive_stencil_A<DataStorage, Kernel, Dim>), data, subSpaces[0], t0, t1, leftB, db==0? rightB: REMOVE_BOUND(rightB));
			recursive_stencil_A<DataStorage, Kernel, Dim>( data, subSpaces[1], t0, t1, da==0? leftB: REMOVE_BOUND(leftB), rightB);
			SYNC(left);

			recursive_stencil_B<DataStorage, Kernel, Dim>( data, subSpaces[2], t0, t1, da==0? leftB: REMOVE_BOUND(leftB), db==0? rightB: REMOVE_BOUND(rightB));
		}
		else if (Dim != 0){
			recursive_stencil_dispatch<DataStorage, Kernel, NextDim>( data, z, t0, t1, leftB, rightB);
		}
		// time cut
		else if (deltaT > TIME_CUTOFF){

			const int halfTime = deltaT/2;
			assert(halfTime >= 1);
			//std::cout << " time cut " << halfTime << "(" << t0 << "," << halfTime+t0 << "](" << halfTime+t0 << "," << t1 << "]" << std::endl;

			recursive_stencil_dispatch <DataStorage, Kernel, Dim>(data, z, t0, t0+halfTime, leftB, rightB);

			// We must update all the dimensions as we move in time.... 
			auto upZoid = z;
			for (auto d = 0; d < z.dimensions; ++d){
				upZoid.a(d) = z.a(d) + z.da(d)*halfTime;
				upZoid.b(d) = z.b(d) + z.db(d)*halfTime;
			}

			recursive_stencil_dispatch<DataStorage, Kernel, Dim>(data, upZoid, t0+halfTime, t1, da==0? leftB: REMOVE_BOUND(leftB) , db==0? rightB: REMOVE_BOUND(rightB));
		}
		else{
			//std::cout << "					BASECASE: " << z << " t(" << t0 << "," << t1 << ") lB" << (int)leftB << " rB" << (int)rightB  << std::endl;
			if( leftB + rightB == 0) base_case<DataStorage, Kernel, Target_Hyperspace::dimensions, false> (data, z, t0, t1);
			else 					 base_case<DataStorage, Kernel, Target_Hyperspace::dimensions, true> (data, z, t0, t1);
		}
	}

	// This function handles hyperspaces with wider top (inverted pyramid)
	template <typename DataStorage, typename Kernel, int Dim>
	inline void recursive_stencil_B(DataStorage& data, const Hyperspace<DataStorage::dimensions>& z, int t0, int t1, Bound_flags leftB, Bound_flags rightB){

		typedef Hyperspace<DataStorage::dimensions> Target_Hyperspace;
		constexpr auto NextDim = next_dim<Dim, Target_Hyperspace::dimensions>::value;

		//std::cout << "B-" << z << " t(" << t0 << "," << t1 << ") d" << Dim << " lB" << (int)leftB << " rB" << (int)rightB << std::endl;

		const auto deltaT = t1-t0;
		const auto a  = z.a(Dim);
		const auto b  = z.b(Dim);
		const auto da = z.da(Dim);
		const auto db = z.db(Dim);
		const auto deltaTop = (b + db * deltaT) - (a + da * deltaT);
		const auto neighbours = Kernel::neighbours;

		assert(da <= db);
		
		// spatial cut (this case cuts in W)
		if (deltaTop >= 2*2*neighbours*deltaT){

			//std::cout << " cut in W " << std::endl;
			const auto& subSpaces  = Target_Hyperspace::template split_W2<Dim, neighbours> (z);
			assert(subSpaces.size() == 3);

			//std::cout << "   			- " << subSpaces[0] << std::endl;
			//std::cout << "   			- " << subSpaces[1] << std::endl;
			//std::cout << "   			- " << subSpaces[2] << std::endl;

			recursive_stencil_A<DataStorage, Kernel, Dim> (data, subSpaces[0], t0, t1, da==0? leftB: REMOVE_BOUND(leftB) , db==0? rightB: REMOVE_BOUND(rightB));

			SPAWN ( left, (recursive_stencil_B<DataStorage, Kernel, Dim>), data, subSpaces[1], t0, t1, leftB, db==0? rightB: REMOVE_BOUND(rightB));
			recursive_stencil_B<DataStorage, Kernel, Dim>( data, subSpaces[2], t0, t1, da==0? leftB: REMOVE_BOUND(leftB), rightB );
			SYNC(left);

		}
		else if (Dim != 0){
			recursive_stencil_dispatch<DataStorage, Kernel, NextDim>( data, z, t0, t1, leftB, rightB);
		}
		// time cut
		else if (deltaT > TIME_CUTOFF){

			// little optimization, if the base of the hyp in this dimmension is 0, 
			// we can skip it and improve spatial cut chances by shifting the piramid  by one
			const int halfTime = (a!=b) ?  deltaT>>1 : (deltaT>>1)+1;
			assert(halfTime >= 1 && halfTime < deltaT);

			//std::cout << " time cut " << halfTime << "(" << t0 << "," << halfTime+t0 << "](" << halfTime+t0 << "," << t1 << "]" << std::endl;
			recursive_stencil_dispatch<DataStorage, Kernel, NextDim>(data, z, t0, t0+halfTime, da==0? leftB: REMOVE_BOUND(leftB) , db==0? rightB: REMOVE_BOUND(rightB));

			// We must update all the dimensions as we move in time.... 
			auto upZoid = z;
			for (auto d = 0; d < z.dimensions; ++d){
				upZoid.a(d) = z.a(d) + z.da(d)*halfTime;
				upZoid.b(d) = z.b(d) + z.db(d)*halfTime;
			}

			recursive_stencil_dispatch<DataStorage, Kernel, NextDim>(data, upZoid, t0+halfTime, t1, leftB, rightB);
		}
		else {
			//std::cout << "					BASECASE: " << z << " t(" << t0 << "," << t1 << ") lB" << (int)leftB << " rB" << (int)rightB  << std::endl;
			if( leftB + rightB == 0) base_case<DataStorage, Kernel, Target_Hyperspace::dimensions, false> (data, z, t0, t1);
			else 					 base_case<DataStorage, Kernel, Target_Hyperspace::dimensions, true> (data, z, t0, t1);
		}
	}


} // detail

// ~~~~~~~~~~~~~~~~ Recursive stencil entry point  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template <typename DataStorage, typename Kernel>
	void recursive_stencil(DataStorage& data, unsigned t){

		// 7 (111) is all flags saying that touch the border
		unsigned char allDims = 1;
		for (int i =1; i < Kernel::dimensions; i++){
			allDims <<=1;
			allDims += 1;
		}

		PARALLEL_CTX ({

			// notice that the original piramid has perfect vertical sides
			auto z = data.getGlobalHyperspace();


			(detail::recursive_stencil_Z<DataStorage, Kernel, Kernel::dimensions-1>)(data, z, 0, t, allDims, allDims);

		});
	}

} // stencil namespace
