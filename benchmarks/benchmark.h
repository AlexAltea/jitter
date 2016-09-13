/**
 * (c) 2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include <iomanip>
#include <iostream>

// Print
template<typename T> void printElement(T t, const int& width) {
    std::cout << std::left << std::setw(width) << std::setfill(' ') << t;
}

// Timer
#if _WIN32
#include <Windows.h>

#define TIMER_INIT() \
    LARGE_INTEGER frequency; \
    LARGE_INTEGER t1, t2; \
    QueryPerformanceFrequency(&frequency);
#define TIMER_START() \
    QueryPerformanceCounter(&t1);
#define TIMER_STOP() \
    QueryPerformanceCounter(&t2); \
    elapsedTime = (double)(t2.QuadPart - t1.QuadPart) / frequency.QuadPart;

#endif
