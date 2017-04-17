// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_LINUX_MUTEX_HPP
#define WATER_THREADS_LINUX_MUTEX_HPP
#include <water/threads/linux/mutex_futex.hpp>
#include <water/threads/posix/pthread_mutex.hpp>
#include <water/threads/posix/mutex_sem.hpp>
#include <water/threads/spin_mutex.hpp>
#include <water/threads/recursive.hpp>
namespace water { namespace threads {

using mutex_list = list<
	pthread_mutex,
	pthread_mutex_recursive,
	mutex_futex<>,
	recursive<mutex_futex<>>,
	mutex_sem<>,
	spin_mutex<>
	>;

}}
#endif
