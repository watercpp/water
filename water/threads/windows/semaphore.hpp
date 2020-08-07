// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_WINDOWS_SEMAPHORE_HPP
#define WATER_THREADS_WINDOWS_SEMAPHORE_HPP
#include <water/threads/windows/semaphore_semaphore.hpp>
#include <water/threads/windows/semaphore_handle.hpp>
#include <water/threads/spin_semaphore.hpp>
namespace water { namespace threads {

using semaphore_list = list<
    semaphore_semaphore<>,
    semaphore_semaphore<hold_semaphore_named>,
    semaphore_handle,
    spin_semaphore<>
>;

}}
#endif
