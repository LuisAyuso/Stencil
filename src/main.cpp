#include <iostream>

#include <cassert>

#include "hyperspace.h"
#include "kernel.h"
#include "bufferSet.h"
#include "rec_stencil.h"

#include "timer.h" 

using namespace stencil;

//typedef unsigned char PixelType;
typedef double PixelType;
typedef BufferSet<PixelType, 2> ImageSpace;

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


		struct Blur3_k : public Kernel<ImageSpace, 2, Blur3_k>{

			static const float Kcoeff[3][3];

			void operator() (ImageSpace& data, unsigned i, unsigned j, unsigned t){

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
				
				getElem(data, i, j, t+1) = (unsigned char)sum;
			}

			std::pair<int,int> getSlope(unsigned dimension){
				return {1,-1};
			}
		};

		const float Blur3_k::Kcoeff[3][3] = {{0.01, 0.08, 0.01}, {0.08, 0.64, 0.08}, {0.01, 0.08, 0.01}};


		struct Blur5_k : public Kernel<ImageSpace, 2, Blur5_k>{

			static const float Kcoeff[5][5];

			void operator() (ImageSpace& data, unsigned i, unsigned j, unsigned t){

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
				
				getElem(data, i, j, t+1) = (unsigned char)sum;
			}

			std::pair<int,int> getSlope(unsigned dimension){
				return {2,-2};
			}
		};

		const float Blur5_k::Kcoeff[5][5] =
									{{0.01, 0.02, 0.04, 0.02, 0.01},
									 {0.02, 0.04, 0.08, 0.04, 0.02},
									 {0.04, 0.08, 0.16, 0.08, 0.04},
									 {0.02, 0.04, 0.08, 0.04, 0.02},
									 {0.01, 0.02, 0.04, 0.02, 0.01}};

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
				return {1,-1};
			}
		};

		struct Color_k : public Kernel<ImageSpace, 2, Life_k>{

			unsigned steps;
			Color_k (unsigned steps) : steps(256/steps) {}

			void operator() (ImageSpace& data, unsigned i, unsigned j, unsigned t){
				getElem(data, i, j, t+1) = (t*steps)%256;
			}

			std::pair<int,int> getSlope(unsigned dimension){
				return {1,-1};
			}
		};

} // #######################################################################################

bool REC = false, IT = false, ALL = false;

void parse_args(int argc, char *argv[]){

	if (argc == 1) {
		ALL = true;
		return;
	}
	if (argc == 2){
		ALL = std::string(argv[1]) == "all";
		IT  = std::string(argv[1]) == "it";
		REC = std::string(argv[1]) == "rec";
	}
}




#include "CImg.h"
using namespace cimg_library;
int main(int argc, char *argv[]) {

	// ~~~~~~~~~~~~~~~ Input problem parameters ~~~~~~~~~~~~~~~~~~~~~
	//const char* input_file = "../eight.png";
	//const char*  input_file = "../sixteen.png";
	//const char*  input_file = "../emo.jpg";
	//const char*  input_file = "../lena.png";
	const char* input_file = "../yoBW.png";
	//const char* input_file = "../skogafossBW.png";
	const int timeSteps = 10;
	parse_args(argc, argv);

	// ~~~~~~~~~~~~~~~ Load image ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	CImg<PixelType> orgImage(input_file);
	assert(orgImage.size ()  == (unsigned)orgImage.width() *  (unsigned)orgImage.height() && "only Grayscale allowed");

	std::cout <<" execute " << input_file << " with " << timeSteps << " time steps" << std::endl;

	// ~~~~~~~~~~~~~~~~~~  create multidimensional buffer for flip-flop ~~~~~~~~~~~~~~~~~~~~~~~~
	ImageSpace recBuffer(std::vector<PixelType>(orgImage.begin(), orgImage.end()), 
										{(unsigned)orgImage.width(), (unsigned)orgImage.height() } );
	ImageSpace itBuffer(std::vector<PixelType>(orgImage.begin(), orgImage.end()), 
										{(unsigned)orgImage.width(), (unsigned)orgImage.height() } );
	assert(orgImage.size () == itBuffer.getSize());
	assert(orgImage.size () == recBuffer.getSize());


	// ~~~~~~~~~~~~~~~~~ create kernel ~~~~~~~~~~~~~~~~~~~~~~~
	//Blur3_k kernel;
	Blur5_k kernel;
	//Life_k kernel;
	//Color_k kernel(timeSteps);
	//Copy_k kernel;
	
	// ~~~~~~~~~~~~~~~~ RUN ~~~~~~~~~~~~~~~~~~~~~~~~~~
	if (REC || ALL){
		std::cout << " ==== Recursive ==== " << std::endl;
		TIME_CALL(recursive_stencil_2D(recBuffer, kernel, timeSteps));
	}

	if (IT || ALL){
		std::cout << " ==== Iterative ==== " << std::endl;

		auto seq = [&] (){
			for (unsigned t = 0; t < timeSteps; ++t){
				for (unsigned i = 0; i < getW(itBuffer); ++i){
					for (unsigned j = 0; j < getH(itBuffer); ++j){
						kernel(itBuffer, i, j, t);
					}
				}
			}
		};

		TIME_CALL(seq());
	}

	// ~~~~~~~~~~~~~~~~ Plot and Validate  ~~~~~~~~~~~~~~~~~~~~~~~~~~
	if (ALL){


		CImg<PixelType> recImage(recBuffer.getPointer(timeSteps%2), getW(recBuffer), getH(recBuffer));
		CImg<PixelType> itImage(itBuffer.getPointer(timeSteps%2), getW(itBuffer), getH(itBuffer));

		orgImage = orgImage.get_resize(800, 800, -100, -100, 1);
		recImage = recImage.get_resize(800, 800, -100, -100, 1);
		itImage = itImage.get_resize(800, 800, -100, -100, 1);

		CImgDisplay original(orgImage, "original"), rec(recImage, "rec"), par(itImage, "par"); 

		if (recBuffer != itBuffer) std::cout << "VALIDATION FAILED" << std::endl;

		while (!original.is_closed() && !rec.is_closed() && !par.is_closed()){
			original.wait();
		}
		return 0;
	}
}

