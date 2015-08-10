#include <iostream>

#include <cassert>

#include "hyperspace.h"
#include "kernel.h"
#include "bufferSet.h"
#include "rec_stencil2.h"

#include "timer.h" 

using namespace stencil;

typedef BufferSet<unsigned char, 2> ImageSpace;

namespace {

		struct Copy_k : public Kernel<ImageSpace, 2, Copy_k>{

			void operator() (ImageSpace& data, unsigned i, unsigned j, unsigned t){

				auto pix = getElem(data, i, j, 0);
				getElem(data, i, j, 1) = pix;
			}

			std::pair<int,int> getSlope(unsigned dimension){
				return {0,0};
			}
		};


		struct Blur_k : public Kernel<ImageSpace, 2, Blur_k>{

			void operator() (ImageSpace& data, unsigned i, unsigned j, unsigned t){

				//std::cout << "(" << getW(data) << "," << getH(data) << ")" << std::endl;
				unsigned sum = 0;
				
				for (unsigned x = MAX(0, ((int)i)-1); x < MIN(getW(data), i+1); ++x){
					for (unsigned y = MAX(0, ((int)j)-1); y < MIN(j+1, getH(data)); ++y){	

				//		std::cout << "(" << x << "," << y << ")" << "(" << i << "," << j << ")" << std::endl;
						auto e = getElem(data, x, y, t);
						if (x != i || y != j) e = e*0.02;
						if (x == i || y == j) e = e*0.9;
						sum += e;
					}
				}
				
				getElem(data, i, j, t+1) = sum;
			}

			std::pair<int,int> getSlope(unsigned dimension){
				return {1,1};
			}
		};


		struct Life_k : public Kernel<ImageSpace, 2, Life_k>{

			void operator() (ImageSpace& data, unsigned i, unsigned j, unsigned t){

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
				return {1,1};
			}
		};

		struct Color_k : public Kernel<ImageSpace, 2, Life_k>{

			void operator() (ImageSpace& data, unsigned i, unsigned j, unsigned t){
				getElem(data, i, j, t+1) = t;
			}

			std::pair<int,int> getSlope(unsigned dimension){
				return {1,1};
			}
		};

} // #######################################################################################


#include "CImg.h"
using namespace cimg_library;
int main() {

	// Input problem parameters
	CImg<unsigned char> image("../lena.png");
	const int timeSteps = 4;
	
	assert(image.size ()  == (unsigned)image.width() *  (unsigned)image.height());


	// create multidimensional buffer for flip-flop
	ImageSpace recBuffer(std::vector<unsigned char>(image.begin(), image.end()), 
										{(unsigned)image.width(), (unsigned)image.height() } );
	ImageSpace parBuffer(std::vector<unsigned char>(image.begin(), image.end()), 
										{(unsigned)image.width(), (unsigned)image.height() } );
	assert(image.size () == parBuffer.getSize());
	assert(image.size () == recBuffer.getSize());

	// create kernel
	//Blur_k kernel;
	//Life_k kernel;
	Color_k kernel;
	
	std::cout << " ==== Recursive ==== " << std::endl;
	TIME_CALL(recursive_stencil_2D(recBuffer, kernel, timeSteps));

	// run
	std::cout << " ==== Iterative ==== " << std::endl;

	auto seq = [&] (){
		for (unsigned t = 0; t < timeSteps; ++t){
			for (unsigned i = 0; i < getW(parBuffer); ++i){
				for (unsigned j = 0; j < getH(parBuffer); ++j){
					kernel(parBuffer, i, j, t);
				}
			}
		}
	};

	TIME_CALL(seq());

	// Copy back the data and plot
	{
		CImg<unsigned char> recImage(recBuffer.getPointer(timeSteps%2), getW(recBuffer), getH(recBuffer));
		CImg<unsigned char> parImage(parBuffer.getPointer(timeSteps%2), getW(parBuffer), getH(parBuffer));

		CImgDisplay original(image, "original"), rec(recImage, "rec"), par(parImage, "par"); 

		while (!original.is_closed() && !rec.is_closed() && !par.is_closed()){
			original.wait();
		}
		return 0;
	}
}

