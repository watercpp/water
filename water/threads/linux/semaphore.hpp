// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_LINUX_SEMAPHORE_HPP
#define WATER_THREADS_LINUX_SEMAPHORE_HPP
#include <water/threads/linux/semaphore_futex.hpp>
#include <water/threads/posix/sem.hpp>
#include <water/threads/spin_semaphore.hpp>
namespace water { namespace threads {

using semaphore_list = list<
    semaphore_futex<>,
    sem,
    spin_semaphore<>
>;

}}
#endif
