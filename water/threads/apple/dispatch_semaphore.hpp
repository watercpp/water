// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_APPLE_DISPATCH_SEMAPHORE_HPP
#define WATER_THREADS_APPLE_DISPATCH_SEMAPHORE_HPP
#include <water/threads/apple/deadline.hpp>
#include <dispatch/dispatch.h>
namespace water { namespace threads {

class dispatch_semaphore
{
public:
    using needs = threads::needs<need_system, need_timeout>;

private:
    dispatch_semaphore_t my;
    ___water_threads_statistics(threads::statistics::reference mystatistics;)

public:
    dispatch_semaphore(unsigned value = 0) noexcept :
        my(dispatch_semaphore_create(static_cast<intptr_t>(value)))
    {
        ___water_assert(my);
    }

    dispatch_semaphore(dispatch_semaphore const&) = delete;
    dispatch_semaphore& operator=(dispatch_semaphore const&) = delete;

    #ifndef __OBJC__
    
    ~dispatch_semaphore() noexcept {
        if(my)
            dispatch_release(my);
    }
    
    #endif

    explicit operator bool() const noexcept {
        return my != 0;
    }

    bool up() noexcept {
        return dispatch_semaphore_signal(my) >= 0; // returns 1 if a thread is woken otherwise 0
    }

    bool down() noexcept {
        ___water_threads_statistics(if(try_down()) return true;)
        return dispatch_semaphore_wait(my, DISPATCH_TIME_FOREVER) == 0;
    }

    bool down(deadline d) noexcept {
        ___water_threads_statistics(if(try_down()) return true;)
        return dispatch_semaphore_wait(my, dispatch_time(d.time(), 0)) == 0;
    }

    bool try_down() noexcept {
        bool r = dispatch_semaphore_wait(my, DISPATCH_TIME_NOW) == 0;
        ___water_threads_statistics(threads::statistics::add(mystatistics, this, "dispatch_semaphore").wait(r));
        return r;
    }

    dispatch_semaphore_t underlying() noexcept {
        return my;
    }

    ___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "dispatch_semaphore"); })
};

}}
#endif
