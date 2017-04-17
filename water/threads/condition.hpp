// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_CONDITION_HPP
#define WATER_THREADS_CONDITION_HPP
#include <water/threads/bits.hpp>
#include <water/threads/deadline.hpp>
#include WATER_THREADS_INCLUDE(condition.hpp)
namespace water { namespace threads {

template<typename ...needs_> using
 condition = typename need_select<needs<needs_...>, condition_list>::result;
 
template<typename condition_> using
 mutex_for_condition = typename condition_::mutex;

#if 0

template<typename ...needs_> class
 condition {
	public:
		condition() = default;
		condition(condition const&) = delete;
		condition& operator=(condition const&) = delete;
		template<typename mutex_> bool wait(mutex_& mutex);
		template<typename mutex_> bool wait(mutex_& mutex, double seconds);
		template<typename mutex_> bool wait(mutex_& mutex, deadline);
		bool wake();
		bool wake_all();
	};

#endif

}}
#endif
