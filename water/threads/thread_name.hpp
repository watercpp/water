// Copyright 2022 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_THREAD_NAME_HPP
#define WATER_THREADS_THREAD_NAME_HPP
#include <water/threads/configuration.hpp>
#include WATER_THREADS_INCLUDE(thread_name.hpp)
namespace water { namespace threads {

// Useful for debugging and maybe crash reports.
// Keep the name short, Linux/Android seems to have a limit of 16 characters including the 0 at the end 

#if 0

bool thread_name(char const* name) noexcept;

#endif

}}
#endif
