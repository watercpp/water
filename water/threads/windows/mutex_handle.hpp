// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_WINDOWS_MUTEX_HANDLE_HPP
#define WATER_THREADS_WINDOWS_MUTEX_HANDLE_HPP
#include <water/threads/windows/deadline.hpp>
namespace water { namespace threads {

class mutex_handle
{
public:
    using needs = threads::needs<need_system, need_timeout, need_recursive>;

private:
    void *my;
    ___water_threads_statistics(threads::statistics::reference mystatistics;)

public:
    mutex_handle() :
        my(CreateMutexW(0, 0, 0))
    {
        ___water_assert(my);
        panic_if_not(my);
    }

    ~mutex_handle() noexcept {
        if(my) CloseHandle(my);
    }

    mutex_handle(mutex_handle const&) = delete;
    mutex_handle& operator=(mutex_handle const&) = delete;

    void lock() {
        ___water_threads_statistics(if(try_lock()) return;)
        panic_if(handle_wait(my));
    }

    bool lock(double seconds) noexcept {
        ___water_threads_statistics(if(try_lock()) return true;)
        return my && handle_wait(my, milli_from_seconds(seconds)) == 0;
    }

    bool lock(deadline d) noexcept {
        ___water_threads_statistics(if(try_lock()) return true;)
        return my && handle_wait(my, d.milli_left()) == 0;
    }

    bool try_lock() noexcept {
        bool r = handle_wait(my, 0) == 0;
        ___water_threads_statistics(threads::statistics::add(mystatistics, this, "mutex_handle").wait(r));
        return r;
    }

    void unlock() {
        ___water_assert(my);
        panic_if_not(ReleaseMutex(my));
    }

    void* underlying() noexcept {
        return my;
    }

    ___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "mutex_handle"); })
};

}}
#endif
