// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_SEMAPHORE_HPP
#define WATER_THREADS_SEMAPHORE_HPP
#include <water/threads/configuration.hpp>
#include WATER_THREADS_INCLUDE(semaphore.hpp)
namespace water { namespace threads {

template<typename ...needs_> using
 semaphore = typename need_select<needs<needs_...>, semaphore_list>::result;

#if 0

template<typename ...needs_> class
 semaphore {
	public:
		semaphore(unsigned value = 0);
		semaphore(semaphore const&) = delete;
		semaphore& operator=(semaphore const&) = delete;
		explicit operator bool() const; // true if constructed ok
		bool down();
		bool down(double seconds);
		bool down(deadline);
		bool try_down();
		bool up();
	};

#endif

}}
#endif
