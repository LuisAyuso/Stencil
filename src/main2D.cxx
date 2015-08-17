#include <iostream>

#include <cassert>

#include <CImg.h>

#include "hyperspace.h"
#include "kernel.h"
#include "kernels_2D.h"
#include "bufferSet.h"
#include "rec_stencil.h"

#include "timer.h" 

using namespace stencil;
using namespace cimg_library;

//typedef unsigned char PixelType;
//typedef float PixelType;
typedef double PixelType;
typedef BufferSet<PixelType, 2> ImageSpace;

 // #######################################################################################

bool REC = false, IT = false, INV = false, ALL = false, VALIDATE=true, VISUALIZE=false;

	//const char* input_file = "../eight.png";
	//const char*  input_file = "../sixteen.png";
	//const char*  input_file = "../emo.jpg";
	//const char*  input_file = "../lena.png";
	char* input_file = nullptr;
	//const char* input_file = "../skogafossBW.png";
	int timeSteps = 10;


void help(){
	std::cout << "Stencil ops:" << std::endl;
	std::cout << "Stencil2D [all|it|rec] -i image [-t time steps]" << std::endl;
}

void parse_args(int argc, char *argv[]){

	int i = 1;
	while(i < argc){

		std::string param(argv[i]);
		if(param == "it") {
			IT = true;
		}
		else if(param == "rec") {
			REC = true;
		}
		else if(param == "inv") {
			INV = true;
		}
		else if( param == "-i"){
			i++;
			input_file = argv[i];
		}
		else if (param == "-t"){

			i++;
			timeSteps = std::atoi(argv[i]);
		}
		else if (param == "-h"){

			help();
			exit(0);
		}
		else if (param == "-no-val"){
			VALIDATE = false;
		}
		else if (param == "-v"){
			VISUALIZE = true;
		}
		else {

			help();
			exit(0);
		}

		i++;
	}

	ALL = !(IT || REC || INV);

	VALIDATE =  ALL;

	if (VISUALIZE && !ALL) {
		std::cout << "can only visualize when running ALL" << std::endl;
		exit(-1);
	}
}


//######################## MAIN ###################################################

int main(int argc, char *argv[]) {

	// ~~~~~~~~~~~~~~~ Input problem parameters ~~~~~~~~~~~~~~~~~~~~~
	parse_args(argc, argv);

	if (!input_file) {
		help();
		return -1;
	}

	// ~~~~~~~~~~~~~~~ Load image ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	CImg<PixelType> orgImage(input_file);
	assert(orgImage.size ()  == (unsigned)orgImage.width() *  (unsigned)orgImage.height() && "only Grayscale allowed");

	std::cout <<" execute " << input_file << " with " << timeSteps << " time steps ";
	std::cout << "(" << (sizeof(PixelType) * orgImage.size ()) << "Bytes)" << std::endl;

	// ~~~~~~~~~~~~~~~~~~  create multidimensional buffer for flip-flop ~~~~~~~~~~~~~~~~~~~~~~~~
	ImageSpace recBuffer( {(unsigned)orgImage.width(), (unsigned)orgImage.height() }, orgImage.data());
	ImageSpace iteBuffer ( {(unsigned)orgImage.width(), (unsigned)orgImage.height() }, orgImage.data());
	ImageSpace invBuffer( {(unsigned)orgImage.width(), (unsigned)orgImage.height() }, orgImage.data());
	assert(orgImage.size () == iteBuffer.getSize());
	assert(orgImage.size () == recBuffer.getSize());
	assert(orgImage.size () == invBuffer.getSize());

	// ~~~~~~~~~~~~~~~~~ create kernel ~~~~~~~~~~~~~~~~~~~~~~~
	
	//using KernelType = example_kernels::Color_k<PixelType> kernel(timeSteps);
	//using KernelType = example_kernels::Copy_k<PixelType>;
	//using KernelType = example_kernels::Life_k<PixelType>;
	//using KernelType = example_kernels::Blur3_k<PixelType>;
	//using KernelType = example_kernels::Blur5_k<PixelType>;
	//using KernelType = example_kernels::BlurN_k<PixelType, 7>;
	using KernelType = example_kernels::BlurN_k<PixelType, 9>;

	KernelType kernel;

	// ~~~~~~~~~~~~~~~~ RUN ~~~~~~~~~~~~~~~~~~~~~~~~~~
	if (REC || ALL){
		//TIME_CALL( recursive_stencil( recBuffer, kernel, timeSteps) );
		auto t = time_call(recursive_stencil<ImageSpace, KernelType>, recBuffer, kernel, timeSteps);
		std::cout << "recursive: " << t << "ms" <<std::endl;
	}

	if (IT || ALL){
		auto it = [&] (){
			for (unsigned t = 0; t < timeSteps; ++t){
				P_FOR ( i, 0, getW(iteBuffer), 1, {
		 			for (unsigned j = 0; j < getH(iteBuffer); ++j){
						kernel(iteBuffer, i, j, t);
					}
				});
			}
		};

		auto t = time_call(it);
		std::cout << "iterative: " << t <<"ms" << std::endl;
	}

	if (INV || ALL){
		auto it = [&] (){
			for (unsigned t = 0; t < timeSteps; ++t){
				P_FOR ( j, 0, getH(iteBuffer), 1 , {
					for (unsigned i = 0; i < getW(iteBuffer); ++i){
						kernel(invBuffer, i, j, t);
					}
				});
			}
		};

		auto t = time_call(it);
		std::cout << "inverted: " << t << "ms" <<std::endl;
	}

	if (ALL && VALIDATE){
		if (recBuffer != iteBuffer) std::cout << "VALIDATION FAILED" << std::endl;
		else if (invBuffer != iteBuffer) std::cout << "VALIDATION FAILED" << std::endl;
		else if (invBuffer != recBuffer) std::cout << "VALIDATION FAILED" << std::endl;
		else std::cout << "VALIDATION OK" << std::endl;
	}

	//std::cout << recBuffer << std::endl;
	//std::cout << invBuffer << std::endl;
	//std::cout << iteBuffer << std::endl;

	// ~~~~~~~~~~~~~~~~ Plot and Validate  ~~~~~~~~~~~~~~~~~~~~~~~~~~
	if (VISUALIZE){

		CImg<PixelType> recImage(recBuffer.getPointer(timeSteps%2), getW(recBuffer), getH(recBuffer));
		CImg<PixelType> iteImage(iteBuffer.getPointer(timeSteps%2), getW(iteBuffer), getH(iteBuffer));
		CImg<PixelType> invImage(invBuffer.getPointer(timeSteps%2), getW(invBuffer), getH(invBuffer));

		orgImage = orgImage.get_resize	(800, 800, -100, -100, 1);
		recImage = recImage.get_resize	(800, 800, -100, -100, 1);
		iteImage = iteImage.get_resize	(800, 800, -100, -100, 1);
		invImage = invImage.get_resize	(800, 800, -100, -100, 1);

		CImgDisplay original(orgImage, "original"), rec(recImage, "rec"), par(iteImage, "iter"), inv(invImage, "inverted loop");
	
		while (!original.is_closed()){
			original.wait();
		}
	}	

	return 0;
}

