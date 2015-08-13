
#pragma once

#include <cmath>

#include "kernel.h"
#include "bufferSet.h"



namespace stencil{
namespace example_kernels{


		template< typename Elem> 
		struct Copy_k : public Kernel<BufferSet<Elem,2>, 2, Copy_k<Elem>>{

			void operator() (BufferSet<Elem,2>& data, unsigned i, unsigned j, unsigned t){

				auto pix = getElem(data, i, j, 0);
				getElem(data, i, j, 1) = pix;
			}

			std::pair<int,int> getSlope(unsigned dimension){
				return {0,0};
			}
		};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		template< typename Elem> 
		struct Life_k : public Kernel<BufferSet<Elem,2>, 2, Life_k<Elem>>{

			void operator() (BufferSet<Elem,2>& data, unsigned i, unsigned j, unsigned t){

				//std::cout << "(" << getW(data) << "," << getH(data) << ")" << std::endl;
				unsigned sum = 0;

				
				for (unsigned x = MAX(0, ((int)i)-1); x < MIN(getW(data), i+1); ++x){
					for (unsigned y = MAX(0, ((int)j)-1); y < MIN(j+1, getH(data)); ++y){	

				//		std::cout << "(" << x << "," << y << ")" << "(" << i << "," << j << ")" << std::endl;
						sum += (getElem(data, x, y, t) > 125)? 1 : 0;
					}
				}
				
				if (getElem(data, i, j, t) < 128) {
					getElem(data, i, j, t+1) = sum == 3? 255: 0;
				}
				else{
					getElem(data, i, j, t+1) = sum > 3? 0: (sum<2)? 0: 255;
				}
			}

			std::pair<int,int> getSlope(unsigned dimension){
				return {1,-1};
			}
		};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		template< typename Elem> 
		struct Color_k : public Kernel<BufferSet<Elem,2>, 2, Color_k<Elem>>{

			Elem maxValue;

			Color_k ( Elem maxValue) : maxValue(maxValue) {}

			void operator() (BufferSet<Elem,2>& data, unsigned i, unsigned j, unsigned t){
				getElem(data, i, j, t+1) = t%maxValue;
			}

			std::pair<int,int> getSlope(unsigned dimension){
				return {1,-1};
			}
		};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		template< typename Elem> 
		struct Blur3_k : public Kernel<BufferSet<Elem,2>, 2, Blur3_k<Elem>>{

			static const float Kcoeff[3][3];

			void operator() (BufferSet<Elem,2>& data, unsigned i, unsigned j, unsigned t){

				//std::cout << "(" << getW(data) << "," << getH(data) << ")" << std::endl;
				double sum = 0.0;

				for (unsigned x = MAX(0, ((int)i)-1); x <= MIN(getW(data)-1, i+1); ++x){
					for (unsigned y = MAX(0, ((int)j)-1); y <= MIN(j+1, getH(data)-1); ++y){	
						
						int ki =  x-i+1;
						int kj =  y-j+1;

						auto e = getElem(data, x, y, t);
						sum += e * Kcoeff[ki][kj];
					}
				}
				
				getElem(data, i, j, t+1) = sum;
			}

			std::pair<int,int> getSlope(unsigned dimension){
				return {1,-1};
			}
		};

		template< typename Elem> 
		const float Blur3_k<Elem>::Kcoeff[3][3] = {{0.01, 0.08, 0.01}, {0.08, 0.64, 0.08}, {0.01, 0.08, 0.01}};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		template< typename Elem> 
		struct Blur5_k : public Kernel<BufferSet<Elem,2>, 2, Blur5_k<Elem>>{

			static const float Kcoeff[5][5];

			void operator() (BufferSet<Elem,2>& data, unsigned i, unsigned j, unsigned t){

				//std::cout << "(" << getW(data) << "," << getH(data) << ")" << std::endl;
				double sum = 0.0;

				for (unsigned x = MAX(0, ((int)i)-1, ((int)i)-2); x <= MIN(getW(data)-1, i+1, i+2); ++x){
					for (unsigned y = MAX(0, ((int)j)-1, ((int)j)-2); y <= MIN(j+2, j+1, getH(data)-1); ++y){	
						
						int ki =  x-i+1;
						int kj =  y-j+1;

						auto e = getElem(data, x, y, t);
						sum += e * Kcoeff[ki][kj];
					}
				}
				
				getElem(data, i, j, t+1) = sum;
			}

			std::pair<int,int> getSlope(unsigned dimension){
				return {2,-2};
			}
		};

		template< typename Elem> 
		const float Blur5_k<Elem>::Kcoeff[5][5] =
									{{0.01, 0.02, 0.04, 0.02, 0.01},
									 {0.02, 0.04, 0.08, 0.04, 0.02},
									 {0.04, 0.08, 0.16, 0.08, 0.04},
									 {0.02, 0.04, 0.08, 0.04, 0.02},
									 {0.01, 0.02, 0.04, 0.02, 0.01}};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~




		/**
		 * a generic  gaussian blur kernel:
		 * http://stackoverflow.com/questions/8204645/implementing-gaussian-blur-how-to-calculate-convolution-matrix-kernel
		 */
		template< typename Elem, unsigned Size> 
		struct BlurN_k : public Kernel<BufferSet<Elem,2>, 2, BlurN_k<Elem, Size>>{

			const unsigned slope;

			float Kcoeff[Size][Size];

			double gaussian (double x, double mu, double sigma) const {
			  return exp( -(((x-mu)/(sigma))*((x-mu)/(sigma)))/2.0 );
			}

			BlurN_k () 
			:slope((Size-1)/2){
				assert(Size%2 == 1);


				int halfSize = Size/2;
			
				// determine sigma
				double sigma = Size/2.0;

				// fill values
				double sum = 0;
				for (int row = 0; row < Size; row++){
					for (int col = 0; col < Size; col++) {

						int i = row<=halfSize? row: Size-(row+1);
						int j = col<=halfSize? col: Size-(col+1);

						Kcoeff[row][col] = gaussian(i, Size, sigma) * gaussian(j, Size, sigma);
						sum += Kcoeff[row][col];
					}
				}

				//		  // normalize
				for (int row = 0; row < Size; row++){
					for (int col = 0; col < Size; col++){
						Kcoeff[row][col] /= sum;
					}
				}

			}




			void operator() (BufferSet<Elem,2>& data, unsigned i, unsigned j, unsigned t){

				//std::cout << "(" << getW(data) << "," << getH(data) << ")" << std::endl;
				double sum = 0.0;

				assert(i < getW(data));
				assert(j < getH(data));

				auto xMin = i;
				for (int v = 1; v <= slope; ++v) xMin = MAX(0, ((int)i)-v);
				auto xMax = i;
				for (int v = 1; v <= slope; ++v) xMax = MIN(getW(data)-1, i+v);
		
				auto yMin = j;
				for (int v = 1; v <= slope; ++v) yMin = MAX(0, ((int)j)-v);
				auto yMax = j;
				for (int v = 1; v <= slope; ++v) yMax = MIN(getH(data)-1, j+v);

				for (unsigned x = xMin; x <= xMax; ++x){
					for (unsigned y = yMin; y <= yMax; ++y){	
						
							// this is quite not right, this offset might produce weird values 
							// on bonduaries, but results are uniform
						int ki =  x-i+slope;
						int kj =  y-j+slope;

						auto e = getElem(data, x, y, t);
						sum += e * Kcoeff[ki][kj];
					}
				}
				
				getElem(data, i, j, t+1) = sum;
			}

			std::pair<int,int> getSlope(unsigned dimension){
				return {slope,-slope};
			}
		};

}// example_kernels
}// stencil
