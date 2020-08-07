// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_APPLE_SEMAPHORE_MACH_HPP
#define WATER_THREADS_APPLE_SEMAPHORE_MACH_HPP
#include <water/threads/apple/mach.hpp>
#include <water/threads/apple/deadline.hpp>
namespace water { namespace threads {

class semaphore_mach
{
public:
    using needs = threads::needs<need_system, need_timeout>;

private:
    mach_t my;
    ___water_threads_statistics(threads::statistics::reference mystatistics;)

public:
    semaphore_mach(unsigned value = 0) noexcept :
        my(semaphore_create(value))
    {
        ___water_assert(my);
    }

    semaphore_mach(semaphore_mach const&) = delete;
    semaphore_mach& operator=(semaphore_mach const&) = delete;

    ~semaphore_mach() noexcept {
        if(my)
            semaphore_destroy(my);
    }

    explicit operator bool() const noexcept {
        return my != 0;
    }

    bool up() noexcept {
        return semaphore_signal(my);
    }

    bool down() noexcept {
        ___water_threads_statistics(if(try_down()) return true;)
        int e;
        while((e = semaphore_wait(my)) == mach_aborted);
        return e == 0;
    }

    bool down(deadline d) noexcept {
        ___water_threads_statistics(if(try_down()) return true;)
        int e;
        while((e = semaphore_wait(my, d.left())) == mach_aborted);
        return e == 0;
    }

    bool try_down() noexcept {
        bool r = semaphore_wait(my, 0) == 0;
        ___water_threads_statistics(threads::statistics::add(mystatistics, this, "semaphore_mach").wait(r));
        return r;
    }

    mach_t underlying() noexcept {
        return my;
    }

    ___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "semaphore_mach"); })
};

}}
#endif
