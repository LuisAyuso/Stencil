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

