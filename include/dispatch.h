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
# if defined(CUSTOM)
#	error must choose between CUSTOM, CILK or OPENMP
# endif
#endif


#if !defined(_OPENMP) && ! defined(CILK) && !defined(CUSTOM)
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

	#define P_FOR(it, B, E, S) \
		for (auto it = B; it < E; it += S)


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


	#define P_FOR(it, B, E, S) \
		_Pragma( STRINGIFY(  omp parallel )) \
		_Pragma( STRINGIFY(  omp for )) \
		for (auto it = B; it < E; it += S)  


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

	#define P_FOR(it, B, E, S)\
		cilk_for (auto it = B; it < E; it += S)  

#endif

// ~~~~~~~~~~~~~~~~~~~~~ CUSTOM PARALLELISM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef CUSTOM

	#include <functional>
	#include <thread>
	#include <mutex>

	class Thread_Pool {

		struct Thread_queue{
			bool go;
			std::mutex queue_lock;
			std::vector<std::function<void(void)>> tasks;
		};

		static Thread_Pool instance;
		std::vector<Thread_queue> threads;

		std::mutex next_lock;
		unsigned next;

		static void run_thread (Thread_queue& queue){

			do{
			// wait for work
			
				auto checkQueue = [&](){ 
					queue.queue_lock.lock();
					auto empty = queue.tasks.empty();
					queue.queue_lock.unlock();
					return empty;
				};
				auto getTask = [&]() { 
					auto t = queue.tasks.front();

					queue.queue_lock.lock();
					queue.tasks.erase(queue.tasks.begin());
					queue.queue_lock.unlock();
					return t;
				};

				while (checkQueue()){
					const auto& task = getTask();
					task();
				}

				// give the chance to someone else to work
				std::this_thread::yield();

			// do work
			}while (queue.go);
		}

	public: 

		Thread_Pool ()
		:threads(std::thread::hardware_concurrency()-1), next(0)
		{
			for (auto& t : threads){
				std::thread(run_thread, std::ref(t));
			}
		}

		void add_task(const std::function<void(void)>& task){

			// if more than 4 tasks queued turn sequential CUTOFF
			if (threads[next].tasks.size() > 4){
				task();
				return;
			}
			else{
				threads[next].queue_lock.lock();
				threads[next].tasks.push_back(task);
				threads[next].queue_lock.unlock();
			}

			// distribute tasks round robin
			next_lock.lock();
			next++;
			next_lock.unlock();
		}
	};


	#define PARALLEL_CTX(STMT) \
		STMT
	
    #define SPAWN(f, ...) \
		f(__VA_ARGS__)

	#define SYNC 

	#define P_FOR(it, B, E, S)\
		for (auto it = B; it < E; it += S)  

#endif

