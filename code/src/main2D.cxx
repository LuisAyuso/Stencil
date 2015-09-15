#include <iostream>

#include <cassert>

#include "hyperspace.h"
#include "kernel.h"
#include "kernels_2D.h"
#include "bufferSet.h"
//#include "rec_stencil_inverted_dims.h"
#include "rec_stencil_multiple_splits.h"

#include "timer.h" 

using namespace stencil;

//typedef unsigned char PixelType;
//typedef float PixelType;
typedef bool PixelType;
//typedef double PixelType;
typedef BufferSet<PixelType, 2> ImageSpace;

 // #######################################################################################

bool REC = false, IT = false, INV = false, ALL = false, VALIDATE=true, VISUALIZE=false;

	int timeSteps = 10;
	size_t size = 10;


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
		else if (param == "-s"){
			i++;
			size = std::atoi(argv[i]);
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
		else {

			help();
			exit(0);
		}

		i++;
	}

	ALL = !(IT || REC || INV);

	VALIDATE =  ALL;
}


//######################## MAIN ###################################################

int main(int argc, char *argv[]) {

	// ~~~~~~~~~~~~~~~ Input problem parameters ~~~~~~~~~~~~~~~~~~~~~
	parse_args(argc, argv);

	// ~~~~~~~~~~~~~~~ Generate Input ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	std::cout <<" execute " << size << "^2 with " << timeSteps << " time steps ";
	std::cout << "(" << utils::getSizeHuman(sizeof(PixelType) * size*size*size) << ")" << std::endl;

	PixelType data[size*size];
	//for (auto& e : data) { e = (float)rand()/RAND_MAX; }
	for (auto& e : data) { e = rand() & 0x1; }

	// ~~~~~~~~~~~~~~~~~~  create multidimensional buffer for flip-flop ~~~~~~~~~~~~~~~~~~~~~~~~
	ImageSpace recBuffer( { size, size }, data);
	ImageSpace iteBuffer( { size, size }, data);
	ImageSpace invBuffer( { size, size }, data);

	std::cout << " ~~~~~~~~~~~~~ GO ~~~~~~~~~~~~~~~~~~~" <<std::endl;

	// ~~~~~~~~~~~~~~~~~ create kernel ~~~~~~~~~~~~~~~~~~~~~~~
	
	//using KernelType = example_kernels::Color_k<ImageSpace> kernel(timeSteps);
	//using KernelType = example_kernels::Copy_k<ImageSpace>;
	using KernelType = example_kernels::Life_k<ImageSpace>;
	//using KernelType = example_kernels::Blur3_k<ImageSpace>;
	//using KernelType = example_kernels::Blur5_k<ImageSpace>;
	//using KernelType = example_kernels::BlurN_k<ImageSpace, 7>;
	//using KernelType = example_kernels::BlurN_k<ImageSpace, 9>;

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

//	std::cout << recBuffer << std::endl;
//	std::cout << invBuffer << std::endl;
//	std::cout << iteBuffer << std::endl;

	return 0;
}

