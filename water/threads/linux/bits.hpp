// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_LINUX_BITS_HPP
#define WATER_THREADS_LINUX_BITS_HPP
#include <water/threads/posix/bits.hpp>
#ifndef WATER_THREADS_LINUX_NO_FUTEX
    #include <sys/syscall.h>
    #if defined(WATER_THREADS_LINUX_FUTEX) || defined(__NR_futex) || defined(SYS_futex) // sys/syscall.h -> asm/unistd.h
        #include <linux/futex.h>
        #ifndef WATER_THREADS_LINUX_FUTEX
            #define WATER_THREADS_LINUX_FUTEX
        #endif
    #endif
#endif
#endif
