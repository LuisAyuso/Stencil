#include <gtest/gtest.h>

#define CXX_ASYNC
#include "dispatch.h"

#include <chrono>

void task(int& i){
	i+1;
}


TEST(ThreadPool, Concepts){

	std::vector<int> data(10);

	for (auto & elem : data){
		std::thread th (task, std::ref(elem));
		std::thread caca (std::move(th));
		caca.join();
	}
	
}


TEST(ThreadPool, Initalization){

	auto& tp = Thread_Pool::get_instance();
	std::cout << "running" << std::endl;
	 std::chrono::seconds sleep_t { 1 };
 	std::this_thread::sleep_for(sleep_t);
	Thread_Pool::shutdown();
}

TEST(ThreadPool, Re_Init){

	auto& tp = Thread_Pool::get_instance();
	std::cout << "running" << std::endl;
	 std::chrono::seconds sleep_t { 1 };
 	std::this_thread::sleep_for(sleep_t);
	Thread_Pool::shutdown();
}

TEST(ThreadPool, add_task){

	auto& tp = Thread_Pool::get_instance();
	std::cout << "running" << std::endl;


	std::vector<std::future<void>> futures;

	std::atomic_long count;
	
	for (int i = 0; i < 100; i++){

		auto task = [=, &count] () { 
			std::cout << "exect " << i << " task in " << std::this_thread::get_id() << std::endl;
			count ++;
		};

		futures.push_back(Thread_Pool::add_task(task));
	}

	for (auto& f : futures){
		f.wait();
	}

	EXPECT_EQ(100, count);
	std::cout << "all done " << std::endl;;

	Thread_Pool::shutdown();
}

