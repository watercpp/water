// Copyright 2022 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_APPLE_THREAD_NAME_HPP
#define WATER_THREADS_APPLE_THREAD_NAME_HPP
#include <water/threads/posix/bits.hpp>
namespace water { namespace threads {

/*

Exists in macOS 10.6 and iOS 3.2 or later.

Unlike the Linux variant, this can only name the current thread. Setting NSThread.name from another thread could work.

*/

inline bool thread_name(char const* name) noexcept {
    if(&pthread_setname_np == NULL)
        return false;
    return pthread_setname_np(name) == 0;
}

}}
#endif
