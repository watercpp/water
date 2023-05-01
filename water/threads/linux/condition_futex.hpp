// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_LINUX_CONDITION_FUTEX_HPP
#define WATER_THREADS_LINUX_CONDITION_FUTEX_HPP
#include <water/threads/linux/mutex_futex.hpp>
namespace water { namespace threads {
    
template<bool exists_ = futex_exists>
class condition_futex
{
public:
    using needs = threads::needs<need_water, need_constexpr_constructor, need_trivial_destructor, need_timeout>;
    using mutex = mutex_futex<>;

private:
    futex_atomic my{0};
    futex_atomic* mymutex = 0; // inited locked in wait

public:
    constexpr condition_futex() noexcept = default;
    condition_futex(condition_futex const&) = delete;
    condition_futex& operator=(condition_futex const&) = delete;

    bool wait(mutex& m) noexcept {
        mymutex = &m.underlying();
        auto value = my.load();
        m.unlock();
        int e = futex_wait(my, value);
        m.lock_after_condition_wait();
        return e == 0;
    }

    bool wait(mutex& m, double seconds) noexcept {
        mymutex = &m.underlying();
        auto value = my.load();
        m.unlock();
        int e = futex_wait(my, value, seconds);
        m.lock_after_condition_wait();
        return e == 0;
    }

    bool wait(mutex& m, deadline_clock<clockid::realtime> d) noexcept {
        return wait(m, d.left());
    }

    bool wait(mutex& m, deadline_clock<clockid::monotonic_maybe> d) noexcept {
        return wait(m, d.left());
    }

    bool wake(unsigned n = 1) noexcept {
        if(!n || n > futex_max)
            n = futex_max;
        decltype(my.load()) a = my.load(memory_order_relaxed), a1;
        while(!my.compare_exchange_weak(a, a1 = (a + 1) & futex_max));
        // unless mutex is locked by this thread, other threads can read + start wait now
        if(!mymutex || n == 1)
            return futex_wake(my, n) >= 0;
        int e = futex_cmp_requeue(my, a1, 1, *mymutex, n - 1);
        if(e >= 0)
            return true;
        if(e != futex_again)
            return false;
        return futex_wake(my, n) >= 0;
    }

    bool wake_all() noexcept {
        return wake(0);
    }
};

template<>
class condition_futex<false>
{
public:
    using needs = threads::needs<>;
};

}}
#endif
