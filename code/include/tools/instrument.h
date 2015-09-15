
#include "stopwatch.h"
#include "hyperspace.h"

#include <sstream>

namespace instrument{

	template <unsigned Dims>
	uibk::StopWatch::swTicket instrument_base_case(const stencil::Hyperspace<Dims> &z){

		std::stringstream ss;
		ss << "zoid" << z.getStep();
		return uibk::StopWatch::start(ss.str()); 
	}
	
 	void instrument_end(uibk::StopWatch::swTicket& t){
		uibk::StopWatch::stop(t);
	}

}
