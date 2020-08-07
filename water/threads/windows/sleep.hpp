// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_WINDOWS_SLEEP_HPP
#define WATER_THREADS_WINDOWS_SLEEP_HPP
#include <water/threads/windows/deadline.hpp>
namespace water { namespace threads {

inline bool sleep(double seconds) noexcept {
    Sleep(milli_from_seconds(seconds)); // is void
    return true;
}

inline bool sleep(deadline d) noexcept {
    Sleep(d.milli_left()); // is void
    return true;
}

}}
#endif
