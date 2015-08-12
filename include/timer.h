#pragma once

#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <functional>
 

#define TIME_CALL(FC) \
	{ \
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();\
	FC;\
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();\
    std::cout << " - "\
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / (1000.0)\
              << "ms\n";\
	}


//
//double time_call (std::function<void(int)> f, int a){
//    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
//	
//	f(a);
//
//    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / (1000.0);
//}

//template<typename T>
//double time_call (std::function<void(T)> f, T a){
//    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
//	
//	f(a);
//
//    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / (1000.0);
//}
//
template<typename F, typename... ARGS>
double time_call (F f, ARGS& ... args){
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	
	f(args...);

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / (1000.0);
}
