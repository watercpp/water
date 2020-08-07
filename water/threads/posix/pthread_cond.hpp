// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_POSIX_PTHREAD_COND_HPP
#define WATER_THREADS_POSIX_PTHREAD_COND_HPP
#include <water/threads/posix/pthread_mutex.hpp>
#include <water/threads/posix/deadline_clock.hpp>
namespace water { namespace threads {

inline bool wait(pthread_cond_t& c, pthread_mutex_t& m) noexcept {
    int e = pthread_cond_wait(&c, &m);
    ___water_assert(!e);
    return e == 0;
}

inline bool wait(pthread_cond_t& c, pthread_mutex_t& m, deadline_clock<clockid::realtime> d) noexcept {
    int e = pthread_cond_timedwait(&c, &m, &d.underlying());
    ___water_assert(!e || e == ETIMEDOUT);
    return e == 0;
}

#ifdef WATER_SYSTEM_APPLE

inline bool wait(pthread_cond_t& c, pthread_mutex_t& m, double seconds) noexcept {
    if(seconds < 1e-9)
        return false;
    timespec t {};
    timespec_add(t, seconds);
    int e = pthread_cond_timedwait_relative_np(&c, &m, &t);
    ___water_assert(!e || e == ETIMEDOUT);
    return e == 0;
}

#endif

inline bool wake(pthread_cond_t& c) noexcept {
    int e = pthread_cond_signal(&c);
    ___water_assert(!e);
    return e == 0;
}

inline bool wake_all(pthread_cond_t& c) noexcept {
    int e = pthread_cond_broadcast(&c);
    ___water_assert(!e);
    return e == 0;
}

class pthread_cond
{
public:
    using needs = threads::needs<need_system, need_timeout, need_constexpr_constructor>;
    using mutex = pthread_mutex;

private:
    pthread_cond_t my = PTHREAD_COND_INITIALIZER;

public:
    constexpr pthread_cond() noexcept = default;
    pthread_cond(pthread_cond const&) = delete;
    pthread_cond& operator=(pthread_cond const&) = delete;

    bool wait(pthread_mutex_t &m) noexcept {
        return threads::wait(my, m);
    }

    bool wait(pthread_mutex_t &m, deadline_clock<clockid::realtime> d) noexcept {
        return threads::wait(my, m, d);
    }

    bool wait(pthread_mutex_t &m, double seconds) noexcept {
        return seconds >= 1e-9 && threads::wait(my, m, seconds);
    }

    template<typename mutex_>
    auto wait(mutex_ &m) noexcept -> decltype(threads::wait(my, m.underlying())) {
        return wait(m.underlying());
    }

    template<typename mutex_>
    auto wait(mutex_ &m, deadline_clock<clockid::realtime> d) noexcept -> decltype(threads::wait(my, m.underlying(), d)) {
        return wait(m.underlying(), d);
    }

    template<typename mutex_>
    auto wait(mutex_ &m, double seconds) noexcept -> decltype(threads::wait(my, m.underlying(), seconds)) {
        return wait(m.underlying(), seconds);
    }

    template<typename mutex_, typename deadline_>
    auto wait(mutex_ &m, deadline_ d) noexcept -> decltype(threads::wait(my, m.underlying(), d.left())) {
        // this should make apple deadline use relative timeout
        return wait(m.underlying(), d.left());
    }

    bool wake() noexcept {
        return threads::wake(my);
    }

    bool wake_all() noexcept {
        return threads::wake_all(my);
    }

    pthread_cond_t& underlying() noexcept {
        return my;
    }
};

}}
#endif
