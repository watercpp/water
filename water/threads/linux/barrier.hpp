// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_LINUX_BARRIER_HPP
#define WATER_THREADS_LINUX_BARRIER_HPP
#include <water/threads/linux/barrier_futex.hpp>
#include <water/threads/posix/pthread_barrier.hpp>
#include <water/threads/spin_barrier.hpp>
namespace water { namespace threads {

using barrier_list = list<
	pthread_barrier,
	barrier_futex<>,
	spin_barrier<>
	>;

}}
#endif
