// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_LINUX_CONDITION_HPP
#define WATER_THREADS_LINUX_CONDITION_HPP
#include <water/threads/linux/condition_futex.hpp>
#include <water/threads/posix/pthread_cond.hpp>
namespace water { namespace threads {

using condition_list = list<
	pthread_cond,
	condition_futex<>
	>;

}}
#endif
