// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_POSIX_SLEEP_HPP
#define WATER_THREADS_POSIX_SLEEP_HPP
#include <water/threads/posix/deadline_clock.hpp>
namespace water { namespace threads {

// these will be interrupted by signals.

inline bool sleep(double seconds) noexcept {
    timespec t {};
    timespec_add(t, seconds);
    #ifdef WATER_POSIX_CLOCK_NANOSLEEP
    if(is_monotonic<clockid::monotonic_maybe>())
        return clock_nanosleep(CLOCK_MONOTONIC, 0, &t, 0) == 0;
    #endif
    #ifdef WATER_POSIX_NANOSLEEP
    return nanosleep(&t, 0) == 0;
    #else
    return pselect(0, 0, 0, 0, &t, 0) != -1;
    #endif
}

template<clockid clockid_>
bool sleep(deadline_clock<clockid_> d) noexcept {
    return sleep(d.left());
}

#ifdef WATER_POSIX_CLOCK_NANOSLEEP

inline bool sleep(deadline_clock<clockid::monotonic_maybe> d) noexcept {
    if(is_monotonic<clockid::monotonic_maybe>())
        return clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &d.underlying(), 0) == 0;
    return sleep(d.left());
}

#endif

}}
#endif
