// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_CONFIGURATION_HPP
#define WATER_THREADS_CONFIGURATION_HPP
#include <water/water.hpp>

// configuration macros
// WATER_THREADS_PREFER_WATER define to prefer water implementations instead of system
// WATER_THREADS_MUTEX_PREFER_WINDOWS_CRITICAL_SECTION select CRITICAL_SECTION over SRWLOCK

#define WATER_THREADS_PREFER_WATER // test!!

#ifdef WATER_THREADS_STATISTICS
    #ifndef WATER_THREADS_PREFER_WATER
        #define WATER_THREADS_PREFER_WATER
        // the water implementation has more statistics
    #endif
    #define ___water_threads_statistics(...) __VA_ARGS__
#else
    #define ___water_threads_statistics(...)
#endif

#if defined(WATER_SYSTEM_APPLE) && !defined(WATER_THREADS_NO_APPLE)
    #define WATER_THREADS_APPLE
    #define WATER_THREADS_INCLUDE(file) <water/threads/apple/file>
#elif defined(WATER_SYSTEM_LINUX) && !defined(WATER_THREADS_NO_LINUX)
    #define WATER_THREADS_LINUX
    #ifdef linux
        #define WATER_THREADS_INCLUDE_2(a) #a
        #define WATER_THREADS_INCLUDE(file) WATER_THREADS_INCLUDE_2(water/threads/linux/file)
    #else
        #define WATER_THREADS_INCLUDE(file) <water/threads/linux/file>
    #endif
#elif defined(WATER_SYSTEM_POSIX)
    #define WATER_THREADS_POSIX
    #define WATER_THREADS_INCLUDE(file) <water/threads/posix/file>
#elif defined(WATER_SYSTEM_WINDOWS)
    #define WATER_THREADS_WINDOWS
    #define WATER_THREADS_INCLUDE(file) <water/threads/windows/file>
#else
    #error what operating system is this?
#endif

#endif
