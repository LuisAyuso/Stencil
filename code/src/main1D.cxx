#include <iostream>

#include <cassert>


#include "hyperspace.h"
#include "kernel.h"
#include "kernels_1D.h"
#include "bufferSet.h"
//#include "rec_stencil_inverted_dims_by_dim.h"
//#include "rec_stencil_inverted_dims.h"
//#include "rec_stencil_multiple_splits.h"
#include "rec_stencil_multiple_splits_by_dimension.h"

#include "timer.h" 

using namespace stencil;

// #######################################################################################


typedef double ElemType;

typedef BufferSet<ElemType, 1> ImageSpace;


 // #######################################################################################

bool REC = false, IT = false, INV = false, ALL = false, VALIDATE=true;
size_t size = 10;
int timeSteps = 10;


void help(){
	std::cout << "Stencil ops:" << std::endl;
	std::cout << "Stencil [all|it|rec] -s size [-r time steps]" << std::endl;
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
	std::cout <<" execute " << size << " with " << timeSteps << " time steps ";
	std::cout << "(" << utils::getSizeHuman(sizeof(ElemType) * size) << ")" << std::endl;
	
	// ~~~~~~~~~~~~~~~ Load data ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	std::vector<ElemType> data(size);
	for (auto& e : data) e = (float)rand()/RAND_MAX;

	// ~~~~~~~~~~~~~~~~~~  create multidimensional buffer for flip-flop ~~~~~~~~~~~~~~~~~~~~~~~~

	ImageSpace recBuffer( {{size}}, data);
	ImageSpace iteBuffer( {{size}}, data);
	ImageSpace invBuffer( {{size}}, data);

	std::cout << " ~~~~~~~~~~~~~ GO ~~~~~~~~~~~~~~~~~~~" <<std::endl;

	// ~~~~~~~~~~~~~~~~~ create kernel ~~~~~~~~~~~~~~~~~~~~~~~
	
	using KernelType = example_kernels::Avg_1D_k<ImageSpace>;

	KernelType kernel;

	// ~~~~~~~~~~~~~~~~ RUN ~~~~~~~~~~~~~~~~~~~~~~~~~~
	if (REC || ALL){
		auto t = time_call(recursive_stencil<ImageSpace, KernelType>, recBuffer, kernel, timeSteps);
		std::cout << "recursive: " << t << "ms" <<std::endl;
	}

//	if (IT || ALL){
//		auto it = [&] (){
//			for (unsigned t = 0; t < timeSteps; ++t){
//				P_FOR ( i, 0, getW(iteBuffer), 1, {
//		 			for (unsigned j = 0; j < getH(iteBuffer); ++j){
//		 				for (unsigned k = 0; k < getD(iteBuffer); ++k){
//							kernel(iteBuffer, i, j, k, t);
//						}
//					}
//				});
//			}
//		};
//
//		auto t = time_call(it);
//		std::cout << "iterative: " << t <<"ms" << std::endl;
//	}

	if (INV || ALL){
		auto it = [&] (){
			for (unsigned t = 0; t < timeSteps; ++t){
				P_FOR ( i, 0, getW(iteBuffer), 1, {
					kernel(invBuffer, i, t);
				});
			}
		};

		auto t = time_call(it);
		std::cout << "inverted: " << t << "ms" <<std::endl;
	}

	if (ALL && VALIDATE){
		if (recBuffer != invBuffer) std::cout << "VALIDATION FAILED" << std::endl;
	//	else if (invBuffer != iteBuffer) std::cout << "VALIDATION FAILED" << std::endl;
	//	else if (iteBuffer != recBuffer) std::cout << "VALIDATION FAILED" << std::endl;
		else std::cout << "VALIDATION OK" << std::endl;
	}

//	std::cout << recBuffer << std::endl;
//	std::cout << invBuffer << std::endl;
//	std::cout << iteBuffer << std::endl;

	return 0;
}

