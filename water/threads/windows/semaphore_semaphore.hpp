// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_WINDOWS_SEMAPHORE_SEMAPHORE_HPP
#define WATER_THREADS_WINDOWS_SEMAPHORE_SEMAPHORE_HPP
#include <water/threads/windows/deadline.hpp>
#include <water/threads/windows/hold_semaphore.hpp>
#include <water/threads/algorithms/semaphore.hpp>
#include <water/threads/pause.hpp>
namespace water { namespace threads {

template<typename hold_ = hold_semaphore_atomic>
class semaphore_semaphore :
    private hold_
{
    using unsigned_ = decltype(atomic_uint{}.load());
    using algoritm = algorithms::semaphore<atomic_uint>;

public:
    using needs = typename types::ifel_type<
        types::equal<hold_, hold_semaphore_atomic>,
        threads::needs<need_water, need_constexpr_constructor, need_timeout>,
        threads::needs<need_water, need_constexpr_constructor, need_trivial_destructor, need_timeout>
    >::result;

private:
    atomic_uint my;
    ___water_threads_statistics(threads::statistics::reference mystatistics;)
    ___water_threads_statistics(using add_ = threads::statistics::add;)

public:
    constexpr semaphore_semaphore(unsigned value = 0) noexcept :
        my{value}
    {}

    semaphore_semaphore(semaphore_semaphore const&) = delete;
    semaphore_semaphore& operator=(semaphore_semaphore const&) = delete;

    explicit operator bool() const noexcept {
        return true;
    }

    bool down() noexcept {
        ___water_threads_statistics(add_ add(mystatistics, this, "semaphore_semaphore"); add.wait(true));
        pause p;
        algoritm a(my);
        typename hold_::hold_type s;
        while(!a.down()) {
            ___water_threads_statistics(add.wait(false));
            if(!p.once())
                p = pause_wait();
            if(!s.get()) {
                this->get_or_create(s);
                ___water_threads_statistics(add.wait_object_created(s.get()));
                if(!s.get())
                    p();
            }
            else if(handle_wait(s.get()))
                p();
        }
        return true;
    }

    bool down(deadline d) noexcept {
        ___water_threads_statistics(add_ add(mystatistics, this, "semaphore_semaphore"); add.wait(true).timeout(true));
        algoritm a(my);
        if(a.try_down())
            return true;
        ___water_threads_statistics(add.wait(false).timeout(false));
        if(d.passed())
            return false;
        typename hold_::hold_type s;
        this->get_or_create(s);
        if(!s.get())
            return false;
        ___water_threads_statistics(add.wait_object_created());
        bool r = true;
        while(!a.down())
            if(handle_wait(s.get())) {
                r = a.down_timeout();
                break;
            }
        ___water_threads_statistics(add.timeout(r));
        return r;
    }

    bool try_down() noexcept {
        bool r = algoritm(my).try_down();
        ___water_threads_statistics(add_(mystatistics, this, "semaphore_semaphore").wait(r));
        return r;
    }

    bool up(unsigned n = 1) noexcept {
        ___water_threads_statistics(add_ add(mystatistics, this, "semaphore_semaphore"); add.wake(true));
        if(unsigned wake = algoritm(my).up(n)) {
            typename hold_::hold_type s;
            this->get(s);
            if(s.get()) {
                ___water_threads_statistics(add.wake(false));
                return ReleaseSemaphore(s.get(), static_cast<long_t>(wake), 0) != 0;
            }
        }
        return true;
    }

    ___water_threads_statistics(threads::statistics::data* statistics() noexcept { return threads::statistics::get(mystatistics, this, "semaphore_semaphore"); })
};

}}
#endif
