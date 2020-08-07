// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_APPLE_READ_WRITE_COUNT_HPP
#define WATER_THREADS_APPLE_READ_WRITE_COUNT_HPP
#include <water/threads/apple/mach.hpp>
#include <water/threads/apple/deadline.hpp>
#include <water/threads/pause.hpp>
#include <water/threads/algorithms/read_write_count.hpp>
namespace water { namespace threads {

class read_write_count
{
    using algorithm = algorithms::read_write_count<atomic_uint>;

public:
    using needs = threads::needs<need_water, need_timeout, need_constexpr_constructor>;

private:
    atomic_uint my{0};
    mach_atomic mylock{0};
    mach_t mywait = 0;
    mach_t mywrite_thread = 0; // mach_thread() of waiting writer
    ___water_threads_statistics(threads::statistics::reference mystatistics;)
    ___water_threads_statistics(using add_ = threads::statistics::add;)

public:
    constexpr read_write_count() noexcept = default;
    read_write_count(read_write_count const&) = delete;
    read_write_count& operator=(read_write_count const&) = delete;

    ~read_write_count() noexcept {
        if(auto l = mylock.load()) {
            semaphore_destroy(l);
            semaphore_destroy(mywait);
        }
    }

    void lock() noexcept {
        ___water_threads_statistics(add_ add(mystatistics, this, "read_write_count"); add.wait(true);)
        algorithm a(my);
        if(a.write_lock(true))
            return;
        ___water_threads_statistics(add.wait(false));
        pause p = pause_wait();
        mach_t l = semaphore_atomic_get(mylock);
        mach_t thread = mach_thread();  // can fail??
        bool locked = false;
        while(true) {
            if(!l) {
                l = semaphore_atomic_create(mylock, 1, mywait, 0);
                ___water_threads_statistics(add.wait_object_created(l));
            }
            if(l) {
                if(locked) {
                    if(a.write_lock(false)) {
                        if(mywrite_thread == thread)
                            mywrite_thread = 0;
                        break;
                    }
                    mywrite_thread = thread;
                    locked = false;
                    semaphore_wait_signal(mywait, l);
                }
                else if((locked = semaphore_lock(l)) == false)
                    p();
            }
            else
                p();
        }
        if(locked)
            semaphore_signal(l);
    }

    bool lock(deadline d) noexcept {
        ___water_threads_statistics(add_ add(mystatistics, this, "read_write_count"); add.wait(true).timeout(true);)
        algorithm a(my);
        if(a.write_lock(true))
            return true;
        ___water_threads_statistics(add.wait(false).timeout(false));
        bool r = false;
        if(!d.passed()) {
            mach_t l = semaphore_atomic_get(mylock);
            if(!l) {
                l = semaphore_atomic_create(mylock, 1, mywait, 0);
                ___water_threads_statistics(add.wait_object_created(l));
            }
            mach_t thread = mach_thread();
            if(l) {
                int e = 0;
                bool locked = false;
                while((r = a.write_lock(false)) == false && (!e || e == mach_aborted)) {
                    if(!locked) {
                        e = semaphore_wait(l, d.left());
                        locked = e == 0;
                    }
                    else {
                        mywrite_thread = thread;
                        locked = false;
                        e = semaphore_wait_signal(mywait, l, d.left());
                    }
                }
                if(locked) {
                    if(mywrite_thread == thread)
                        mywrite_thread = 0;
                    semaphore_signal(l);
                }
            }
        }
        if(!r && a.write_timeout(r) && !r) {
            mach_t l = semaphore_atomic_get(mylock);
            if(l && semaphore_lock(l)) {
                semaphore_wake_all(mywait);
                semaphore_signal(l);
            }
        }
        ___water_threads_statistics(add.timeout(r));
        return r;
    }

    bool try_lock() noexcept {
        bool r = algorithm(my).write_try_lock();
        ___water_threads_statistics(add_(mystatistics, this, "read_write_count").wait(r));
        return r;
    }

    void unlock() noexcept {
        ___water_threads_statistics(add_ add(mystatistics, this, "read_write_count"); add.wake(true));
        if(algorithm(my).write_unlock())
            if(mach_t l = semaphore_atomic_get(mylock))
                if(semaphore_lock(l)) {
                    semaphore_wake_all(mywait);
                    semaphore_signal(l);
                    ___water_threads_statistics(add.wake(false));
                }
    }

    void read_lock() noexcept {
        ___water_threads_statistics(add_ add(mystatistics, this, "read_write_count"); add.wait(true);)
        algorithm a(my);
        if(a.read_lock(true))
            return;
        ___water_threads_statistics(add.wait(false));
        pause p = pause_wait();
        mach_t l = semaphore_atomic_get(mylock);
        bool locked = false;
        while(true) {
            if(!l) {
                l = semaphore_atomic_create(mylock, 1, mywait, 0);
                ___water_threads_statistics(add.wait_object_created(l));
            }
            if(l) {
                if(locked) {
                    if(a.read_lock(false))
                        break;
                    locked = false;
                    semaphore_wait_signal(mywait, l);
                }
                else if((locked = semaphore_lock(l)) == false)
                    p();
            }
            else
                p();
        }
        if(locked)
            semaphore_signal(l);
    }

    bool read_lock(deadline d) noexcept {
        ___water_threads_statistics(add_ add(mystatistics, this, "read_write_count"); add.wait(true).timeout(true));
        algorithm a(my);
        if(a.read_lock(true))
            return true;
        ___water_threads_statistics(add.wait(false).timeout(false));
        bool r = false;
        if(!d.passed()) {
            mach_t l = semaphore_atomic_get(mylock);
            if(!l) {
                l = semaphore_atomic_create(mylock, 1, mywait, 0);
                ___water_threads_statistics(add.wait_object_created(l));
            }
            if(l) {
                int e = 0;
                bool locked = false;
                while((r = a.read_lock(false)) == false && (!e || e == mach_aborted)) {
                    if(!locked) {
                        e = semaphore_wait(l, d.left());
                        locked = e == 0;
                    }
                    else {
                        locked = false;
                        e = semaphore_wait_signal(mywait, l, d.left());
                    }
                }
                if(locked)
                    semaphore_signal(l);
            }
        }
        if(!r)
            a.read_timeout();
        ___water_threads_statistics(add.timeout(r));
        return r;
    }

    bool read_try_lock() noexcept {
        bool r = algorithm(my).read_try_lock();
        ___water_threads_statistics(add_(mystatistics, this, "read_write_count").wait(r));
        return r;
    }

    void read_unlock() noexcept {
        ___water_threads_statistics(add_ add(mystatistics, this, "read_write_count"); add.wake(true));
        if(algorithm(my).read_unlock())
            if(mach_t l = semaphore_atomic_get(mylock))
                if(semaphore_lock(l)) {
                    if(!mywrite_thread || !semaphore_wake(mywait, mywrite_thread))
                        semaphore_wake_all(mywait);
                    mywrite_thread = 0;
                    semaphore_signal(l);
                    ___water_threads_statistics(add.wake(false));
                }
    }

    ___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "read_write_count"); })
};

}}
#endif
