// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_APPLE_CONDITION_SEMAPHORE_HPP
#define WATER_THREADS_APPLE_CONDITION_SEMAPHORE_HPP
#include <water/threads/apple/mutex_semaphore.hpp>
namespace water { namespace threads {

class condition_semaphore
{
public:
    using needs = threads::needs<need_water, need_constexpr_constructor, need_timeout>;
    using mutex = mutex_semaphore;

private:
    mach_atomic mylock{0};
    mach_t mywait = 0;

public:
    constexpr condition_semaphore() noexcept = default;
    condition_semaphore(condition_semaphore const&) = delete;
    condition_semaphore& operator=(condition_semaphore const&) = delete;

    ~condition_semaphore() noexcept {
        if(auto l = mylock.load()) {
            semaphore_destroy(l);
            semaphore_destroy(mywait);
        }
    }

    template<typename mutex_>
    bool wait(mutex_& l) {
        auto l2 = mylock.load(memory_order_relaxed);
        if(!l2) {
            if(!create())
                return false;
            l2 = mylock.load(memory_order_relaxed);
        }
        else if(!semaphore_lock(l2))
            return false;
        unlock(l);
        int e = semaphore_wait_signal(mywait, l2);
        lock(l);
        return !e || e == mach_aborted;
    }

    template<typename mutex_>
    bool wait(mutex_& l, deadline d) {
        auto l2 = mylock.load(memory_order_relaxed);
        if(!l2) {
            if(!create())
                return false;
            l2 = mylock.load(memory_order_relaxed);
        }
        else {
            int e;
            while((e = semaphore_wait(l2, d.left())) == mach_aborted);
            if(e) return false;
    }

        if(!d.left()) {
            semaphore_signal(l2);
            return false;
        }
        unlock(l);
        int e = semaphore_wait_signal(mywait, l2, d.left());
        lock(l);
        return e == 0;
    }

    bool wake() noexcept {
        auto l = mylock.load(memory_order_relaxed);
        if(!l) return true;
        if(!semaphore_lock(l)) return false;
        semaphore_wake(mywait);
        semaphore_signal(l);
        return true;
    }

    bool wake_all() noexcept {
        auto l = mylock.load(memory_order_relaxed);
        if(!l) return true;
        if(!semaphore_lock(l)) return false;
        bool r = semaphore_wake_all(mywait);
        semaphore_signal(l);
        return r;
    }

private:
    bool create() noexcept {
        mach_t
            l = semaphore_create(),
            w = l ? semaphore_create() : 0;
        if(!w) {
            if(l) semaphore_destroy(l);
            return false;
        }
        mywait = w;
        mylock.store(l);
        return true;
    }
    
};

}}
#endif
