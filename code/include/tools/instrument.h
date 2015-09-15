
#include "stopwatch.h"
#include "hyperspace.h"

#include <sstream>

#ifdef INSTRUMENT

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
	#define BEGIN_INSTRUMENT(Z) \
			auto swt = instrument::instrument_base_case(Z);

	#define END_INSTUMENT \
			instrument::instrument_end(swt);

#else

	#define BEGIN_INSTRUMENT(Z) \
		;

	#define END_INSTUMENT \
		;

#endif
