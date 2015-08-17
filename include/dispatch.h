#pragma once

#include <tuple>


#if defined(_OPENMP) && defined(SEQUENTIAL)
# error omp and sequential? 
#endif

#if defined(CILK) && defined(SEQUENTIAL)
# error CILK and sequential? 
#endif

#if defined(CILK) && defined(_OPENMP)
# error CILK and openmp?
#endif

#if !defined(_OPENMP) && ! defined(CILK)
# define SEQUENTIAL 1
#endif



// macro tools, boilerplate
#define STR(x) #x
#define STRINGIFY(x) STR(x) 
#define CONCATENATE_DETAIL(x, y) x##y
#define CONCATENATE(x, y) CONCATENATE_DETAIL(x, y)
#define MAKE_UNIQUE(x) CONCATENATE(x, __LINE__ )



// ~~~~~~~~~~~~~~~~~~~~~ SEQUENTIAL ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef SEQUENTIAL

	#define PARALLEL_CTX 
		

    #define SPAWN(f, ...) \
        f(__VA_ARGS__)

	#define SYNC \
		{}

	#define P_FOR(it, B, E, S) \
		for (auto it = B; it < E; it += S)


#endif

// ~~~~~~~~~~~~~~~~~~~~~ OPENMP ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef _OPENMP


	#include <omp.h>
    
	#define PARALLEL_CTX \
		_Pragma( STRINGIFY(  omp parallel )) \
		_Pragma( STRINGIFY(  omp single  ))
	

    #define SPAWN(f, ...) \
        auto MAKE_UNIQUE(wrap) = [&] () { f(__VA_ARGS__); }; \
		_Pragma( STRINGIFY(  omp task untied )) \
        MAKE_UNIQUE(wrap)() 


	#define SYNC \
		_Pragma( STRINGIFY( omp taskwait ) )


	#define P_FOR(it, B, E, S) \
		_Pragma( STRINGIFY(  omp parallel )) \
		_Pragma( STRINGIFY(  omp for )) \
		for (auto it = B; it < E; it += S)  


#endif

// ~~~~~~~~~~~~~~~~~~~~~ CILK ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef CILK

	#include <cilk/cilk.h>

	#define PARALLEL_CTX 
	
    #define SPAWN(f, ...) \
        auto MAKE_UNIQUE(wrap) = [&] () { f(__VA_ARGS__); }; \
        cilk_spawn MAKE_UNIQUE(wrap)() 

	#define SYNC \
		cilk_sync;

	#define P_FOR(it, B, E, S)\
		cilk_for (auto it = B; it < E; it += S)  

#endif

