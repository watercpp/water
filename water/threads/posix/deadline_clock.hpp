// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_POSIX_DEADLINE_CLOCK_HPP
#define WATER_THREADS_POSIX_DEADLINE_CLOCK_HPP
#include <water/threads/posix/bits.hpp>
namespace water { namespace threads {

/*
struct timespec {
    time_t tv_sec; //seconds, posix says time_t is integer type
    long tv_nsec; //nanos
    ...
};
*/

enum class clockid {
    realtime, // wall clock, date, can be adjusted
    monotonic_maybe // will be realtime if there is no monotonic clock
};

template<clockid>
inline timespec clock() noexcept {
    timespec r{};
    #ifdef WATER_POSIX_CLOCK_REALTIME
    if(clock_gettime(CLOCK_REALTIME, &r))
        r = {};
    #else
    timeval t{};
    gettimeofday(&t, 0);
    r.tv_sec = t.tv_sec;
    r.tv_nsec = static_cast<decltype(r.tv_nsec)>(t.tv_usec) * 1000;
    #endif
    return r;
}

template<clockid>
inline bool is_monotonic() noexcept {
    return false;
}

#if defined(WATER_POSIX_CLOCK_MONOTONIC) && (!defined(_POSIX_MONOTONIC_CLOCK) || _POSIX_MONOTONIC_CLOCK > 0)

template<>
inline bool is_monotonic<clockid::monotonic_maybe>() noexcept {
    return true;
}

template<>
inline timespec clock<clockid::monotonic_maybe>() noexcept {
    timespec r{};
    if(clock_gettime(CLOCK_MONOTONIC, &r))
        r = {};
    return r;
}

#elif defined(WATER_POSIX_CLOCK_MONOTONIC)

// must check if it exists
namespace _ {
    template<typename = void>
    struct clockid_monotonoic {
        static unsigned is;
    };
        template<typename void_> unsigned clockid_monotonoic<void_>::is;
}

template<>
inline bool is_monotonic<clockid::monotonic_maybe>() noexcept {
    if(!_::clockid_monotonoic<>::is) {
        timespec t{};
        if(!clock_gettime(CLOCK_MONOTONIC, &t))
            _::clockid_monotonoic<>::is = 1;
        else
            _::clockid_monotonoic<>::is = 2;
        ___water_debug(trace() << "water::threads CLOCK_MONOTONIC exists=" << (_::clockid_monotonoic<>::is == 1));
    }
    return _::clockid_monotonoic<>::is == 1;
}

template<>
inline timespec clock<clockid::monotonic_maybe>() noexcept {
    timespec r{};
    if(clock_gettime(is_monotonic<clockid::monotonic_maybe>() ? CLOCK_MONOTONIC : CLOCK_REALTIME, &r))
        r = {};
    return r;
}

#endif

template<clockid clockid_>
class deadline_clock
{
private:
    timespec my;
    bool myleft = true;

public:
    deadline_clock(double seconds) noexcept :
        my(clock<clockid_>())
    {
        if(seconds >= 1e-9) {
            myleft = true;
            timespec_add(my, seconds);
        }
    }

    deadline_clock(timespec absolute_time) noexcept :
        my(absolute_time)
    {}

    deadline_clock(deadline_clock const&) = default;

    template<typename deadline_>
    deadline_clock(deadline_ d, decltype(d.left()) = {}) :
        deadline_clock(d.left())
    {}

    double left() noexcept {
        if(!myleft) return 0;
        auto now = clock<clockid_>();
        double r = 0;
        if(now.tv_sec > my.tv_sec)
            r = now.tv_sec - my.tv_sec;
        r += (now.tv_nsec - my.tv_nsec) / 1e9;
        if(r >= 1e-9)
            return r;
        myleft = false;
        return 0;
    }

    timespec& underlying() noexcept {
        return my;
    }

    timespec const& underlying() const noexcept {
        return my;
    }
};

}}
#endif
