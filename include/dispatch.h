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

#if defined(CILK) || defined(_OPENMP) 
# if defined(CXX_ASYNC)
#	error must choose between CXX_ASYNC, CILK or OPENMP
# endif
#endif


#if !defined(_OPENMP) && ! defined(CILK) && !defined(CXX_ASYNC)
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

	#define PARALLEL_CTX(STMT) \
		STMT
		

    #define SPAWN(f, ...) \
        f(__VA_ARGS__)

	#define SYNC \
		{}

	#define P_FOR(it, B, E, S, STMT) \
		for (auto it = B; it < E; it += S) STMT


#endif

// ~~~~~~~~~~~~~~~~~~~~~ OPENMP ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef _OPENMP

	#include <omp.h>
    
	#define PARALLEL_CTX(STMT) \
		_Pragma( STRINGIFY(  omp parallel )) \
		_Pragma( STRINGIFY(  omp single  )) \
		STMT;
	

    #define SPAWN(f, ...) \
        auto MAKE_UNIQUE(wrap) = [&] () { f(__VA_ARGS__); }; \
		_Pragma( STRINGIFY(  omp task untied )) \
        MAKE_UNIQUE(wrap)() 


	#define SYNC \
		_Pragma( STRINGIFY( omp taskwait ) )


	#define P_FOR(it, B, E, S, STMT) \
		_Pragma( STRINGIFY(  omp parallel )) \
		_Pragma( STRINGIFY(  omp for )) \
		for (auto it = B; it < E; it += S)   STMT


#endif

// ~~~~~~~~~~~~~~~~~~~~~ CILK ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef CILK

	#include <cilk/cilk.h>

	#define PARALLEL_CTX(STMT) \
		STMT
	
    #define SPAWN(f, ...) \
        auto MAKE_UNIQUE(wrap) = [&] () { f(__VA_ARGS__); }; \
        cilk_spawn MAKE_UNIQUE(wrap)() 

	#define SYNC \
		cilk_sync;

	#define P_FOR(it, B, E, S, STMT) \
		cilk_for (auto it = B; it < E; it += S)  STMT

#endif

// ~~~~~~~~~~~~~~~~~~~~~ C++ ASYNC PARALLELISM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef CXX_ASYNC

	#include <functional>
	#include <future>
	#include <thread>
	#include <mutex>

	#define THREAD_CUTOFF 2

	namespace {
		static auto max_threads = std::thread::hardware_concurrency();
		std::atomic_long current_threads (0);


		std::future<void> my_async(std::function<void(void)> f){
			if (current_threads < max_threads * THREAD_CUTOFF) {
				current_threads++; 
				auto wrap = [f]() { f(); current_threads--;};
//				std::cout << "Async call " << current_threads << std::endl;
				return std::async(std::launch::async, wrap);
			} 
			else{ 
//				std::cout << "Deferred call " << current_threads << std::endl;
				return std::async(std::launch::deferred, f);
			}
		}
	
		std::future<void> my_async(std::function<void(int)> f, int arg){
			if (current_threads < max_threads * THREAD_CUTOFF) {
				current_threads++; 
				auto wrap = [f] (int arg) { f(arg); current_threads--;};
//				std::cout << "Async call " << current_threads << std::endl;
				return std::async(std::launch::async, wrap, arg);
			} 
			else{ 
//				std::cout << "Deferred call " << current_threads << std::endl;
				return std::async(std::launch::deferred, f, arg);
			}
		}
	}

	#undef THREAD_CUTOFF 


	#define PARALLEL_CTX(STMT) \
		STMT

    #define SPAWN(f, ...) \
        auto wrap = [&] () { f(__VA_ARGS__); }; \
		std::future<void> fut = my_async(wrap);

	#define SYNC \
		fut.wait(); \

	#define P_FOR(it, B, E, S, STMT) \
		{ \
			std::vector<std::future<void>> promises; \
			auto wrap = [&] (int it) { STMT; }; \
			for (auto it = B; it < E; it += S) \
				promises.push_back(my_async(wrap, it)); \
			for (const auto& f : promises) f.wait(); \
		}


#endif

