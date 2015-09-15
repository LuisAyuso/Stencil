#pragma once


#include <array>

#include "hyperspace.h"
#include "bufferSet.h"
#include "tools.h"

#include "dispatch.h"


#ifndef CUT 
#  define CUT 10
#endif 


namespace stencil{


namespace detail {

	#define FOR_DIMENSION(N) \
	template <typename DataStorage, typename Kernel> \
			inline typename std::enable_if< is_eq<Kernel::dimensions, N>::value, void>::type

		FOR_DIMENSION(1) base_case (DataStorage& data, const Kernel& kernel, const Hyperspace<DataStorage::dimensions>& z, int t0, int t1){

			int ia = z.a(0);
			int ib = z.b(0);

			for (int t = t0; t < t1; ++t){

				for (int i = ia; i < ib; ++i){
						kernel(data, i, t);
				}
				ia += z.da(0);
				ib += z.db(0);
			}
		}

		FOR_DIMENSION(2) base_case (DataStorage& data, const Kernel& kernel, const Hyperspace<DataStorage::dimensions>& z, int t0, int t1){

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
		}

		FOR_DIMENSION(3) base_case (DataStorage& data, const Kernel& kernel, const Hyperspace<DataStorage::dimensions>& z, int t0, int t1){

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
		}

		FOR_DIMENSION(4) base_case (DataStorage& data, const Kernel& kernel, const Hyperspace<DataStorage::dimensions>& z, int t0, int t1){

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

// ~~~~~~~~~~~~~~~~ base case routine, if dim == 0 compute, if not, get to next dimension ~~~~~~~~~~~~~~~~~~~~~~~~

	template <typename DataStorage, typename Kernel, int Dim>
	inline void recursive_stencil_inverted(DataStorage& data, const Kernel& k, const Hyperspace<DataStorage::dimensions>& z, const int t0, const int t1);

	template <typename DataStorage, typename Kernel, int Dim>
	inline void next_dimension_or_base_case (DataStorage& data, const Kernel& k, const Hyperspace<DataStorage::dimensions>& z, const int t0, const int t1){
		if (Dim == 0) base_case <DataStorage, Kernel>  (data, k, z, t0, t1);
		else recursive_stencil_inverted<DataStorage, Kernel, next_dim<Dim,Kernel::dimensions>::value>(data, k, z, t0, t1);
	}


// ~~~~~~~~~~~~~~~~ recursive  routine ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	template <typename DataStorage, typename Kernel, int Dim>
	inline void recursive_stencil_inverted(DataStorage& data, const Kernel& k, const Hyperspace<DataStorage::dimensions>& z, const int t0, const int t1){

		typedef Hyperspace<DataStorage::dimensions> Target_Hyperspace;
		static_assert(CUT >= 3, "cut off must be greater than 3");

		//std::cout << "zoid: " << z <<  " from  " << t0 << " to " << t1 << std::endl;

		const auto deltaT = (int)t1-t0;
		assert(t1 >= t0);
		assert(deltaT >= 0);

		// BASE CASE
		if (deltaT <= CUT){
	
			next_dimension_or_base_case <DataStorage, Kernel, Dim>  (data, k, z, t0, t1);
		}
		
		else{

			const auto a  = z.a(Dim);
			const auto b  = z.b(Dim);
			const auto da = z.da(Dim);
			const auto db = z.db(Dim);
			const auto deltaBase = b - a;
			const auto deltaTop = (b + db * deltaT) - (a + da * deltaT);
			const auto slopeDim = k.getSlope(Dim);

			//std::cout << " a:" << a << " b: "<< b << " da:" << da << " db:" << db << std::endl;
			//std::cout << " deltaBase:" << deltaBase << " deltaTop:" << deltaTop <<  " deltaT:" << deltaT<< std::endl;
			//std::cout << " M cut: " << 2*(ABS(slopeDim.first)+ABS(slopeDim.second))*deltaT << std::endl;
			//std::cout << " W cut: " << 2*(ABS(slopeDim.first)+ABS(slopeDim.second))*deltaT << std::endl;

			// Cut in M
			if (deltaBase >= 2*(ABS(slopeDim.first)+ABS(slopeDim.second))*deltaT){

				const auto split = a + deltaBase /2;

				//std::cout << " cut in M " << split << std::endl;
				const auto& subSpaces  = Target_Hyperspace::template split_M<Dim> (split, z, slopeDim.first, slopeDim.second);
				assert(subSpaces.size() == 3);

				SPAWN ( left, (recursive_stencil_inverted<DataStorage, Kernel, Dim>), data, k, subSpaces[0], t0, t1);
				recursive_stencil_inverted<DataStorage, Kernel, Dim>( data, k, subSpaces[1], t0, t1);
				SYNC(left);
				recursive_stencil_inverted<DataStorage, Kernel, Dim> (data, k, subSpaces[2], t0, t1);
			}
			// Cut in W
			else if (deltaTop >= 2*(ABS(slopeDim.first)+ABS(slopeDim.second))*deltaT){ 

				//std::cout << " cut in M " << std::endl;
				const auto& subSpaces  = Target_Hyperspace::template split_W<Dim> (z, slopeDim.first, slopeDim.second);
				assert(subSpaces.size() == 3);

				recursive_stencil_inverted<DataStorage, Kernel, Dim> (data, k, subSpaces[0], t0, t1);

				SPAWN ( left, (recursive_stencil_inverted<DataStorage, Kernel, Dim>), data, k, subSpaces[1], t0, t1);
				recursive_stencil_inverted<DataStorage, Kernel, Dim>( data, k, subSpaces[2], t0, t1);
				SYNC(left);
			}
			// Time cut
			else { // if (deltaT > 1 && deltaX > 0  && deltaY > 0){
				//std::cout << "time cut: " << z << " from " << t0 << " to " << t1 <<std::endl;

				const int halfTime = deltaT/2;
				assert(halfTime >= 1);

				//std::cout << " t1: " << z << " from " << t0 << " to " << t0+halfTime <<std::endl;
				recursive_stencil_inverted<DataStorage, Kernel, Dim>(data, k, z, t0, t0+halfTime);

				// We must update all the dimensions as we move in time.... 
				auto upZoid = z;
				for (auto d = 0; d < z.dimensions; ++d){
					upZoid.a(d) = z.a(d) + z.da(d)*halfTime;
					upZoid.b(d) = z.b(d) + z.db(d)*halfTime;
				}

				//std::cout << " t2:" << upZoid << " from " << t0+halfTime << " to " << t1 <<std::endl;
				recursive_stencil_inverted<DataStorage, Kernel, Dim>(data, k, upZoid, t0+halfTime , t1);
			}
		}
	}

} // detail


	template <typename DataStorage, typename Kernel>
	void recursive_stencil(DataStorage& data, const Kernel& k, unsigned t){

		PARALLEL_CTX ({

			// notice that the original piramid has perfect vertical sides
			auto z = data.getGlobalHyperspace();

			(detail::recursive_stencil_inverted<DataStorage, Kernel, Kernel::dimensions-1>)(data, k, z, 0, t);

		});
	}

} // stencil namespace
