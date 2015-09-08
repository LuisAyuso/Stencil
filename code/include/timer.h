#pragma once

//#include <iostream>
//#include <iomanip>
//#include <ctime>
//#include <chrono>

#include <functional>
#include <time.h>
 
template<typename F, typename... ARGS>
double time_call (F f, ARGS& ... args){
    
	//std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	
   auto start = clock();

	f(args...);
   auto end = clock();

   // std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
   // return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / (1000.0);

   return (end - start)/((double)CLOCKS_PER_SEC/1000.0);
}
