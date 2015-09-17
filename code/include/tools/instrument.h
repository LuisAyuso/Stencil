#pragma once
#include "stopwatch.h"
#include "hyperspace.h"

#include <sstream>

#ifdef INSTRUMENT


namespace instrument{

	template <typename T >
	uibk::StopWatch::swTicket instrument_base_case(const T &z){

		std::stringstream ss;
		for (int i=0; i < T::dimensions; ++i){
			if (z.da(i) >= z.db(i)) ss << "A";
			else ss << "B";
		}
		ss << "\t" << z.getStep();
		return uibk::StopWatch::start(ss.str()); 
	}
	
	template <typename T >
	uibk::StopWatch::swTicket instrument_split(const T &z){

		std::stringstream ss;
		ss << "split";
		ss << "\t" << z.getStep();
		return uibk::StopWatch::start(ss.str()); 
	}

	uibk::StopWatch::swTicket instrument_loop(int x, int t){

		std::stringstream ss;
		ss << "Chunk"; // << x;
		ss << "\t" << t;
		return uibk::StopWatch::start(ss.str()); 
	}
	
 	void instrument_end(uibk::StopWatch::swTicket& t){
		return uibk::StopWatch::stop(t); 
	}

}
	#define BC_INSTRUMENT(Z) \
			auto swt = instrument::instrument_base_case(Z);

	#define SPLIT_INSTRUMENT(Z) \
			auto swt = instrument::instrument_split(Z);

	#define LOOP_INSTRUMENT(X,T) \
			auto swt = instrument::instrument_loop(X,T);

	#define END_INSTUMENT \
			instrument::instrument_end(swt);

#else
	#define BC_INSTRUMENT(Z) \
		;

	#define SPLIT_INSTRUMENT(K) \
		;

	#define LOOP_INSTRUMENT(X,T) \
		;

	#define END_INSTUMENT \
		;

#endif
