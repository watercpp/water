// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_WINDOWS_YIELD_HPP
#define WATER_THREADS_WINDOWS_YIELD_HPP
#include <water/threads/windows/bits.hpp>
namespace water { namespace threads {

inline void yield() noexcept {
	SwitchToThread(); //returns true if it did switch to another
	//could use Sleep(0)
	}

}}
#endif
