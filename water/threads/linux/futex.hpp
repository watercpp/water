// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_LINUX_FUTEX_HPP
#define WATER_THREADS_LINUX_FUTEX_HPP
#include <water/threads/linux/bits.hpp>
namespace water { namespace threads {

/*

http://man7.org/linux/man-pages/man2/futex.2.html

This depends on std::atomic<unsigned> having the same size and representation as unsigned

*/

using futex_atomic = atomic<unsigned>;

#ifdef WATER_THREADS_LINUX_FUTEX

bool constexpr futex_exists = atomic_exists && sizeof(futex_atomic) == sizeof(unsigned);

// FUTEX_PRIVATE_FLAG means process private
int constexpr
    futex_syscall =
        #ifdef __NR_futex
        __NR_futex,
        #else
        SYS_futex,
        #endif
    futex_operation_cmp_requeue = FUTEX_CMP_REQUEUE | FUTEX_PRIVATE_FLAG,
    futex_operation_wake = FUTEX_WAKE | FUTEX_PRIVATE_FLAG,
    futex_operation_wait = FUTEX_WAIT | FUTEX_PRIVATE_FLAG;

// futex syscall returns -1 on error, and sets errno. positive values are number of woken threads

inline int futex(void* futex1, int operation, unsigned value1) noexcept {
    return static_cast<int>(syscall(futex_syscall, futex1, operation, value1, static_cast<void*>(0), static_cast<void*>(0), 0));
}

inline int futex(void* futex1, int operation, unsigned value1, timespec const* timeout) noexcept {
    return static_cast<int>(syscall(futex_syscall, futex1, operation, value1, timeout, static_cast<void*>(0), 0));
}

inline int futex(void* futex1, int operation, unsigned value1, unsigned long value2, void* futex2, unsigned value3) noexcept {
    return static_cast<int>(syscall(futex_syscall, futex1, operation, value1, value2, futex2, value3));
}

#else

bool constexpr futex_exists = false;

#endif

enum futex_return {
    futex_wakeup = 0,
    futex_again = -EAGAIN,
    futex_signal = -EINTR,
    futex_timeout = -ETIMEDOUT
};

unsigned constexpr futex_max = numeric_limits<int>::max();

template<typename futex_, typename result_>
using if_futex = ifel<futex_exists && sizeof(futex_) == sizeof(int), result_>;

template<typename futex_>
bool futex_is_aligned(futex_& futex) noexcept {
    return sizeof(futex) == sizeof(int) && (reinterpret_cast<size_t>(&futex) % sizeof(int)) == 0;
}

template<typename int_>
if_futex<int_, int> futex_wait(int_& futex, unsigned value, timespec const* timeout = 0) noexcept {
    // if futex == value, wait
    // - timeout is relative
    // - if timeout is 0-pointer, wait is forever
    //
    // returns
    // - futex_wakeup (0) if woken
    // - futex_again if futex != value
    // - futex_timeout
    // - futex_signal if interrupted by a signal
    //
    // pre
    // - int_ is integer type with sizeof >= sizeof(int)
    //
    ___water_assert(futex_is_aligned(futex));
    int r = threads::futex(&futex, futex_operation_wait, value, timeout);
    if(r < 0) {
        r = -errno;
        if(EAGAIN != EWOULDBLOCK && r == -EWOULDBLOCK) // EWOULDBLOCK and EAGAIN is probably the same
            r = -EAGAIN;
    }
    ___water_assert(!r || r == futex_again || r == futex_signal || (timeout && r == futex_timeout));
    return r;
}

template<typename int_>
if_futex<int_, int> futex_wait(int_& futex, unsigned value, double seconds) noexcept {
    timespec t{};
    timespec_add(t, seconds);
    return futex_wait(futex, value, &t);
}

template<typename int_>
if_futex<int_, int> futex_wake(int_& futex, unsigned count = 1) noexcept {
    // wake count threads waiting on futex.
    //
    // returns
    // - number of woken threads
    //
    // pre
    // - 1 <= count <= futex_max
    // - int_ is an integer type with sizeof >= sizeof(int)
    //
    ___water_assert(futex_is_aligned(futex));
    ___water_assert(count && count <= futex_max);
    int r = threads::futex(&futex, futex_operation_wake, count);
    ___water_assert(r >= 0);
    return r;
}

template<typename int_>
if_futex<int_, int> futex_wake_all(int_& futex) noexcept {
    return futex_wake(futex, futex_max);
}

template<typename int1_, typename int2_>
if_futex<int1_, int> futex_cmp_requeue(int1_& futex, unsigned value, unsigned count, int2_& requeue_futex, unsigned requeue_count = futex_max) noexcept {
    // if futex == value
    // - wake count threads
    // - if more than count threads are waiting
    //   - requeue at most requeue_count threads to requeue_futex
    //
    // returns
    // - futex_again if futex != value
    // - number of threads woken + requeued
    // - if return > count, return - count was requeued
    //
    // pre
    // - 1 <= count <= futex_max
    // - 1 <= requeue_count <= futex_max
    // - int1_ and int2_ is an integer type with sizeof >= sizeof(int)
    //
    // post
    // - requeued threads do not know that they have been requeued when they return from futex_wait
    //
    ___water_assert(futex_is_aligned(futex) && futex_is_aligned(requeue_futex));
    ___water_assert(count && count <= futex_max && requeue_count && requeue_count <= futex_max);
    int r = threads::futex(&futex, futex_operation_cmp_requeue, count, requeue_count, &requeue_futex, value);
    if(r < 0) {
        r = -errno;
        if(EAGAIN != EWOULDBLOCK && r == -EWOULDBLOCK) // EWOULDBLOCK and EAGAIN is probably the same
            r = -EAGAIN;
    }
    ___water_assert(r == futex_again || r >= 0);
    return r;
}


}}
#endif
