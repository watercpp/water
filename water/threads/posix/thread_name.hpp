// Copyright 2022 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_POSIX_THREAD_NAME_HPP
#define WATER_THREADS_POSIX_THREAD_NAME_HPP
#include <water/threads/posix/bits.hpp>
namespace water { namespace threads {

/*

int pthread_setname_np(pthread_t thread, const char *name)

This function seems to exist on Linux, Android, FreeBSD, OpenBSD, NetBSD and more.
Linux and Android seem to limit the length to 16 chars, and will error ERANGE when it's too long.
It was added in glibc 2.12 on 2010-05-03

*/

#ifdef WATER_THREADS_NO_THREAD_NAME

inline bool thread_name(char const* name) noexcept {
    return false;
}

#else

inline bool thread_name(char const* name) noexcept {
    auto t = pthread_self();
    int e = pthread_setname_np(t, name);
    if(!e)
        return true;
    if(e == ERANGE) {
        // try cutting it
        char n[64] {};
        unsigned length = 0;
        if(name && *name) {
            do n[length++] = *name; while(*++name && length != 64);
            unsigned sizes[] = {64, 32, 16, 8};
            for(auto const s : sizes) {
                if(length >= s) {
                    n[s - 1] = 0;
                    e = pthread_setname_np(t, n);
                    if(e != ERANGE)
                        return e == 0;
                }
            }
        }
    }
    return false;
}

#endif

}}
#endif
