// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_APPLE_SEMAPHORE_HPP
#define WATER_THREADS_APPLE_SEMAPHORE_HPP
#include <water/threads/apple/dispatch_semaphore.hpp>
#include <water/threads/apple/semaphore_mach.hpp>
#include <water/threads/apple/semaphore_semaphore.hpp>
#include <water/threads/spin_semaphore.hpp>
namespace water { namespace threads {

using semaphore_list = list<
    dispatch_semaphore,
    semaphore_semaphore,
    semaphore_mach,
    spin_semaphore<>
>;

}}
#endif
