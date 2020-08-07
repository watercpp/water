// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_LINUX_MUTEX_FUTEX_HPP
#define WATER_THREADS_LINUX_MUTEX_FUTEX_HPP
#include <water/threads/linux/futex.hpp>
#include <water/threads/functions.hpp>
#include <water/threads/pause.hpp>
#include <water/threads/deadline.hpp>
namespace water { namespace threads {
    
template<bool exists_ = futex_exists>
class mutex_futex
{
public:
    using needs = threads::needs<need_water, need_constexpr_constructor, need_trivial_destructor, need_timeout>;

private:
    futex_atomic my{0};
    ___water_threads_statistics(threads::statistics::reference mystatistics;)
    ___water_threads_statistics(using add_ = threads::statistics::add;)

public:
    constexpr mutex_futex() noexcept = default;
    mutex_futex(mutex_futex const&) = delete;
    mutex_futex& operator=(mutex_futex const&) = delete;

    void lock() noexcept {
        ___water_threads_statistics(add_ add(mystatistics, this, "mutex_futex"); add.wait(true);)
        decltype(my.load()) x = 0;
        if(my.compare_exchange_strong(x, 1, memory_order_acquire))
            return;
        ___water_threads_statistics(add.wait(false));
        pause p = pause_wait();
        while(my.exchange(2))
            if(int e = futex_wait(my, 2))
                if(e != futex_again && e != futex_signal)
                    p();
    }

    bool lock(deadline_clock<clockid::monotonic_maybe> d) noexcept {
        ___water_threads_statistics(add_ add(mystatistics, this, "mutex_futex"); add.wait(true).timeout(true);)
        decltype(my.load()) x = 0;
        if(my.compare_exchange_strong(x, 1, memory_order_acquire))
            return true;
        ___water_threads_statistics(add.wait(false));
        double left;
        while((left = d.left()) >= 1e-9) {
            if(!my.exchange(2))
                return true;
            int e = futex_wait(my, 2, left);
            if(e && e != futex_again && e != futex_signal)
                break;
        }
        ___water_threads_statistics(add.timeout(false));
        return false;
    }

    bool try_lock() noexcept {
        decltype(my.load()) x;
        bool r = my.compare_exchange_strong(x = 0, 1, memory_order_acquire);
        ___water_threads_statistics(add_(mystatistics, this, "mutex_futex").wait(r));
        return r;
    }

    void unlock() noexcept {
        ___water_threads_statistics(add_ add(mystatistics, this, "mutex_futex"); add.wake(true));
        if(my.exchange(0, memory_order_release) == 2) {
            futex_wake(my);
            ___water_threads_statistics(add.wake(false));
        }
    }

    futex_atomic& underlying() noexcept {
        return my;
    }

    ___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "mutex_futex"); })
};

template<>
class mutex_futex<false>
{
public:
    using needs = threads::needs<>;
};

}}
#endif
