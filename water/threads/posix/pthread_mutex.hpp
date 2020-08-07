// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_POSIX_PTHREAD_MUTEX_HPP
#define WATER_THREADS_POSIX_PTHREAD_MUTEX_HPP
#include <water/threads/posix/bits.hpp>
#include <water/threads/posix/deadline_clock.hpp>
#include <water/threads/functions.hpp>
namespace water { namespace threads {

inline void lock(pthread_mutex_t& a) {
    int e = pthread_mutex_lock(&a);
    ___water_assert(!e && "pthread_mutex_lock error");
    panic_if(e);
}

inline bool try_lock(pthread_mutex_t& a) noexcept {
    return pthread_mutex_trylock(&a) == 0;
}

inline void unlock(pthread_mutex_t& a) {
    int e = pthread_mutex_unlock(&a);
    ___water_assert(!e && "pthread_mutex_lock error");
    panic_if(e);
}

#ifdef WATER_POSIX_TIMEOUTS

using pthread_mutex_timeout = need_timeout;

inline bool lock(pthread_mutex_t& a, deadline_clock<clockid::realtime> d) noexcept {
    int e = pthread_mutex_timedlock(&a, &d.underlying());
    ___water_assert(!e || e == ETIMEDOUT);
    return e == 0;
}

#else

using pthread_mutex_timeout = need_nothing;

#endif

class pthread_mutex
{
public:
    using needs = threads::needs<need_system, need_constexpr_constructor, pthread_mutex_timeout>;

private:
    pthread_mutex_t my = PTHREAD_MUTEX_INITIALIZER;
    ___water_threads_statistics(threads::statistics::reference mystatistics;)

public:
    constexpr pthread_mutex() noexcept = default;
    pthread_mutex(pthread_mutex const&) = delete;
    pthread_mutex& operator=(pthread_mutex const&) = delete;

    ~pthread_mutex() noexcept {
        pthread_mutex_destroy(&my);
    }

    void lock() {
        ___water_threads_statistics(if(try_lock()) return;)
        threads::lock(my);
    }

    bool try_lock() noexcept {
        bool r = threads::try_lock(my);
        ___water_threads_statistics(threads::statistics::add(mystatistics, this, "pthread_mutex").wait(r));
        return r;
    }

    void unlock() {
        threads::unlock(my);
    }

    #ifdef WATER_POSIX_TIMEOUTS
    bool lock(deadline_clock<clockid::realtime> d) noexcept {
        ___water_threads_statistics(if(try_lock()) return true;)
        return threads::lock(my, d);
    }

    #endif
    pthread_mutex_t& underlying() noexcept {
        return my;
    }

    ___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "pthread_mutex"); })
};

#ifdef WATER_POSIX_MUTEX_RECURSIVE

// non portable
// PTHREAD_RECURSIVE_MUTEX_INITIALIZER (mac, linux)
// PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP (linux only?)

class pthread_mutex_recursive
{

#ifdef PTHREAD_RECURSIVE_MUTEX_INITIALIZER

public:
    using needs = threads::needs<need_system, need_recursive, need_constexpr_constructor, pthread_mutex_timeout>;

private:
    pthread_mutex_t my = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
    ___water_threads_statistics(threads::statistics::reference mystatistics;)

public:
    constexpr pthread_mutex_recursive() noexcept = default;

    ~pthread_mutex_recursive() noexcept {
        pthread_mutex_destroy(&my);
    }

#else

public:
    using needs = threads::needs<need_system, need_recursive, pthread_mutex_timeout>;

private:
    pthread_mutex_t my;
    bool myok;
    ___water_threads_statistics(threads::statistics::reference mystatistics;)

public:
    pthread_mutex_recursive() {
        pthread_mutexattr_t a;
        if(!pthread_mutexattr_init(&a)) {
            if(!pthread_mutexattr_settype(&a, PTHREAD_MUTEX_RECURSIVE))
                myok = pthread_mutex_init(&my, &a) == 0;
            pthread_mutexattr_destroy(&a);
        }
        ___water_assert(myok);
        panic_if_not(myok);
    }

    ~pthread_mutex_recursive() noexcept {
        if(myok) pthread_mutex_destroy(&my);
    }

#endif

    pthread_mutex_recursive(pthread_mutex_recursive const&) = delete;
    pthread_mutex_recursive& operator=(pthread_mutex_recursive const&) = delete;

    void lock() {
        ___water_threads_statistics(if(try_lock()) return;)
        threads::lock(my);
    }

    bool try_lock() noexcept {
        bool r = threads::try_lock(my);
        ___water_threads_statistics(threads::statistics::add(mystatistics, this, "pthread_mutex_recursive").wait(r));
        return r;
    }

    void unlock() {
        threads::unlock(my);
    }

    #ifdef WATER_POSIX_TIMEOUTS
    bool lock(deadline_clock<clockid::realtime> d) noexcept {
        ___water_threads_statistics(if(try_lock()) return true;)
        return threads::lock(my, d);
    }

    #endif
    pthread_mutex_t& underlying() noexcept {
        return my;
    }

    ___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "pthread_mutex_recursive"); })
};

#else

class pthread_mutex_recursive
{
public:
    using needs = threads::needs<>;
};

#endif

}}
#endif
