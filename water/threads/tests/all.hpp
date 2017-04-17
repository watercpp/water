// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_ALL_HPP
#define WATER_THREADS_TESTS_ALL_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/tests/barrier_all.hpp>
#include <water/threads/tests/condition_all.hpp>
#include <water/threads/tests/mutex_all.hpp>
#include <water/threads/tests/need_all.hpp>
#include <water/threads/tests/once_all.hpp>
#include <water/threads/tests/read_write_all.hpp>
#include <water/threads/tests/semaphore_all.hpp>
#include <water/threads/tests/thread_all.hpp>
#include <water/threads/tests/trivial.hpp>
namespace water { namespace threads { namespace tests {

inline void all() {
	#ifdef WATER_SYSTEM_WINDOWS
	timeBeginPeriod(1); // cant take sleep(0.001) becomes 16ms
	#pragma comment(lib, "winmm.lib")
	#endif
	barrier_all();
	condition_all();
	mutex_all();
	need_all();
	once_all();
	read_write_all();
	semaphore_all();
	thread_all();
	trivial();
	#ifdef WATER_SYSTEM_WINDOWS
	timeEndPeriod(1);
	#endif
	}

}}}
#endif
