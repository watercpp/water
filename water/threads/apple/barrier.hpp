// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_APPLE_BARRIER_HPP
#define WATER_THREADS_APPLE_BARRIER_HPP
#include <water/threads/apple/barrier_semaphore.hpp>
#include <water/threads/posix/pthread_barrier.hpp>
#include <water/threads/spin_barrier.hpp>
namespace water { namespace threads {

using barrier_list = list<
	pthread_barrier,
	barrier_semaphore,
	spin_barrier<>
	>;

}}
#endif
