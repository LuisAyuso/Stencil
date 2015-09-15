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


#if !defined(_OPENMP) && ! defined(CILK) && !defined(CXX_ASYNC) && !defined(INSIEME_RT)
# define SEQUENTIAL 1
#else
// common to all parallel versions
# define THREAD_CUTOFF 2
# include <atomic>
#endif


// macro tools, boilerplate
#define CONCATENATE_DETAIL(x, y) x##y
#define CONCATENATE(x, y) CONCATENATE_DETAIL(x, y)
#define MAKE_UNIQUE(x) CONCATENATE(x, __LINE__ )



// ~~~~~~~~~~~~~~~~~~~~~ SEQUENTIAL ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef SEQUENTIAL

	namespace {
		const static auto MAX_THREADS = 1;
	}

	#define PARALLEL_CTX(STMT) \
		STMT

    #define SPAWN(taskName, f, ...) \
        f(__VA_ARGS__);  \
		int taskName;

	#define SYNC(...) \
		{}

	#define P_FOR(it, B, E, S, STMT) \
		for (auto it = B; it < E; it += S) STMT

	// dummy promise for future values
	typedef int PROMISE;

#endif

// ~~~~~~~~~~~~~~~~~~~~~ OPENMP ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef _OPENMP

	#include <omp.h>

	// dummy promise for future values, sync is done based on threadgroup
	typedef int PROMISE;

	namespace {
		const static auto MAX_THREADS = omp_get_thread_limit();
		static auto max_threads = MAX_THREADS * THREAD_CUTOFF;
		std::atomic_long current_threads (0);
	}
    
	#define PARALLEL_CTX(STMT) \
		_Pragma( "omp parallel" ) \
		_Pragma( "omp single"  ) \
		STMT;
	
    #define SPAWN(taskName, f, ...) \
        auto MAKE_UNIQUE(wrap) = [&] () { current_threads++; f(__VA_ARGS__); current_threads--; }; \
		_Pragma( "omp task untied if (current_threads  < max_threads)") \
		MAKE_UNIQUE(wrap)(); \
		PROMISE taskName;

	#define SYNC(...) \
		_Pragma( "omp taskwait ")

	#define P_FOR(it, B, E, S, STMT) \
		_Pragma( " omp parallel ") \
		_Pragma( "  omp for ") \
		for (auto it = B; it < E; it += S)   STMT


#endif

// ~~~~~~~~~~~~~~~~~~~~~ CILK ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef CILK

#ifdef SEQUENTIAL
# error what?
#endif
#ifdef _OPENMP
# error what omp?
#endif

	#include <cilk/cilk.h>
	#include <cilk/cilk_api.h>

	namespace {
		const static auto MAX_THREADS = __cilkrts_get_nworkers();
		static auto max_threads = MAX_THREADS * THREAD_CUTOFF;
		std::atomic_long current_threads (0);
	}

	#define PARALLEL_CTX(STMT) \
		STMT
	
    #define SPAWN(taskName, f, ...) \
        auto MAKE_UNIQUE(wrap) = [&] () { current_threads++; f(__VA_ARGS__); current_threads--; }; \
		if(current_threads < max_threads) cilk_spawn MAKE_UNIQUE(wrap)(); \
		else f(__VA_ARGS__); \
		int taskName;

	#define SYNC(...) \
		cilk_sync;

	#define P_FOR(it, B, E, S, STMT) \
		cilk_for (auto it = B; it < E; it += S)  STMT

	// dummy promise for future values, sync is done based on threadgroup
	typedef int PROMISE;

#endif

// ~~~~~~~~~~~~~~~~~~~~~ C++ ASYNC PARALLELISM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef CXX_ASYNC

	#include <functional>
	#include <future>
	#include <thread>
	#include <mutex>

	#include <thread>

	namespace {

		const static auto MAX_THREADS = std::thread::hardware_concurrency();
		static auto max_threads = MAX_THREADS * THREAD_CUTOFF;
		std::atomic_long current_threads (0);

	//	class Thread_Pool{

	//		typedef std::pair<std::function<void(void)>, std::promise<void>> task_t;

	//		struct Thread_queue{

	//			std::vector<task_t> task_queue;
	//			std::mutex queue_lock;
	//			int next_task;
	//			bool go;

	//			Thread_queue()
	//			: next_task(0), go(true) {}
	//		};

	//		const int num_threads;
	//		std::vector<Thread_queue> thread_queues;
	//		std::atomic_long next_thread;
	//		std::vector<std::thread> thread_pids;
	//		bool running;


	//		static void run (Thread_queue& queue){

	//			auto task_available = [&] () { 
	//				queue.queue_lock.lock();
	//				auto nelem =  queue.task_queue.size();
	//				queue.queue_lock.unlock();
	//				return queue.next_task < nelem;
	//			};

	//			while (queue.go){

	//				while(task_available()){

	//					std::cout << "execute: " << queue.next_task << " of " << queue.task_queue.size() << std::endl;

	//					// execute task
	//					queue.task_queue[queue.next_task].first();
	//					// make future ready
	//					queue.task_queue[queue.next_task].second.set_value();
	//					queue.next_task ++;

	//				}
	//		//		std::cout << "list empty" << std::endl;

	//				std::this_thread::yield();
	//			}
	//			
	//		}


	//		Thread_Pool()
	//		: num_threads(std::thread::hardware_concurrency()), thread_queues(num_threads), next_thread(0), thread_pids(num_threads), running(false)
	//		{ 
	//		}

	//		void _init(){
	//			for (int i =0; i < num_threads; ++i){

	//				thread_queues[i].go = true;
	//				thread_queues[i].next_task = 0;

	//				std::thread th (Thread_Pool::run, std::ref(thread_queues[i]));
	//				thread_pids.emplace(thread_pids.begin()+i, std::move(th) );
	//			}
	//			running = true;
	//		}
	//		void _shutdown(){
	//			for (int i =0; i < num_threads; ++i){
	//				thread_queues[i].go = false;
	//			}
	//			for (int i =0; i < num_threads; ++i){
	//				thread_pids[i].join();
	//				thread_queues[i].task_queue.clear();
	//			}

	//			running = false;
	//		}


	//		std::future<void> _add_task(std::function<void(void)> f) {
	//			int current = next_thread.fetch_add(1) % num_threads;
	//			thread_queues[current].queue_lock.lock();
	//			thread_queues[current].task_queue.push_back({f, std::promise<void>()});
	//			auto fut = thread_queues[current].task_queue.back().second.get_future();
	//			thread_queues[current].queue_lock.unlock();
	//			std::cout << "task added to: " << current<<std::endl;
	//			return fut;
	//		}

	//	public:

	//		static Thread_Pool& get_instance(){
	//			static Thread_Pool inst;
	//			if (!inst.running) inst._init();
	//			return inst;
	//		}

	//		static std::future<void> add_task(std::function<void(void)> f) {
	//			return get_instance()._add_task(f);
	//		}

	//		static void shutdown() {
	//			get_instance()._shutdown();
	//		}

	//	};


		std::future<void> my_async(std::function<void(void)> f){
			if (current_threads < max_threads) {
				current_threads++; 
				auto wrap = [f]() { f(); current_threads--;};
//				std::cout << "Async call " << current_threads << std::endl;
				return std::async(std::launch::async, wrap);
//				return Thread_Pool::add_task(wrap);
			} 
			else{ 
//				std::cout << "Deferred call " << current_threads << std::endl;
				return std::async(std::launch::deferred, f);
			}
		}
	
		std::future<void> my_async(std::function<void(int)> f, int arg){
			if (current_threads < max_threads) {
				current_threads++; 
				auto wrap = [=] () { f(arg); current_threads--;};
//				std::cout << "Async call " << current_threads << std::endl;
				return std::async(std::launch::async, wrap);
//				return Thread_Pool::add_task(wrap);
			} 
			else{ 
//				std::cout << "Deferred call " << current_threads << std::endl;
				return std::async(std::launch::deferred, f, arg);
			}
		}
	}


	#define PARALLEL_CTX(STMT) \
		STMT

    #define SPAWN(taskName, f, ...) \
        auto wrap = [&] () { f(__VA_ARGS__); }; \
		std::future<void> taskName = my_async(wrap);

namespace {


	template <typename ...ARGS>
	void waitTasks (ARGS& ... args){
		waitTasks(args...);
	}

	template <typename ...ARGS>
	void waitTasks (std::future<void>& f, ARGS& ... args){

		f.wait();
		waitTasks(args...);
	}

	void waitTasks (std::future<void>& f){
		f.wait();
	}

	void waitTasks (std::vector<std::future<void>>& fs){
		for (auto& f : fs) f.wait();
	}

	template<unsigned N>
	void waitTasks (std::array<std::future<void>, N>& fs){
		for (auto& f : fs) f.wait();
	}
}

	#define SYNC(...) \
		waitTasks(__VA_ARGS__);

	#define P_FOR(it, B, E, S, STMT) \
		{ \
			std::vector<std::future<void>> futures; \
			auto wrap = [&] (int it) { STMT; }; \
			for (auto it = B; it < E; it += S) \
				futures.push_back(my_async(wrap, it)); \
			for (const auto& f : futures) f.wait(); \
		}

	typedef std::future<void> PROMISE;

#endif

// ~~~~~~~~~~~~~~~~~~~~~ INSIEME RUNTIME ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef INSIEME_RT

	#define IRT_LIBRARY_MAIN
	#include"irt_library.hxx"

//	struct RT_INIT{
//		RT_INIT(){
//			std::cout << "Hello RT" << std::endl;
//			irt::init();
//		}
//		~RT_INIT(){
//			std::cout << "Bye RT" << std::endl;
//			irt::shutdown();
//		}
//	};
//	// initialize insieme RT
//	RT_INIT dummy_init;

	#include <thread>
	namespace {
		const static auto MAX_THREADS = std::thread::hardware_concurrency();
		static auto max_threads =  MAX_THREADS * THREAD_CUTOFF;
		std::atomic_long current_threads (0);
	}

	#define PARALLEL_CTX(STMT) \
		STMT
	
    #define SPAWN(taskName, f, ...) \
        auto MAKE_UNIQUE(wrap) = [&] () { current_threads++; f(__VA_ARGS__); current_threads--;}; \
		if(current_threads < max_threads) irt::parallel(1, MAKE_UNIQUE(wrap)); \
        else f(__VA_ARGS__);\
		int taskName;

	#define SYNC(...) \
		irt::merge_all()

	#define P_FOR(it, B, E, S, STMT) \
        auto MAKE_UNIQUE(wrap) = [&] (int it) { STMT; }; \
		irt::pfor(B, E, S, MAKE_UNIQUE(wrap));

	// dummy promise
	typedef int PROMISE;

#endif


