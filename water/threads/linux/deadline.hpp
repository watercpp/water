// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_LINUX_DEADLINE_HPP
#define WATER_THREADS_LINUX_DEADLINE_HPP
#include <water/threads/posix/deadline_clock.hpp>
namespace water { namespace threads {

#ifdef WATER_THREADS_PREFER_WATER

using deadline = deadline_clock<clockid::monotonic_maybe>;

#else

using deadline = deadline_clock<clockid::realtime>;

#endif

}}
#endif
