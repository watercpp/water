// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_PAUSE_HPP
#define WATER_THREADS_PAUSE_HPP
#include <water/threads/sleep.hpp>
#include <water/threads/yield.hpp>
namespace water { namespace threads {

class pause
{
    unsigned my = 0;
    unsigned myspin = 0;
    unsigned myyield = 100;
    float mysleep = 0.001f;

public:
    void operator()() noexcept {
        if(my != static_cast<unsigned>(-1))
            ++my;
        if(my > myspin) {
            if(!myyield || my <= myspin + myyield)
                threads::yield();
            else
                threads::sleep(mysleep);
        }
    }

    pause& spin(unsigned times) noexcept {
        // 0 means just yield
        myspin = times;
        return *this;
    }

    pause& yield(unsigned times) noexcept {
        // 0 means yield forever, never sleep
        myyield = times;
        return *this;
    }

    pause& sleep(double seconds) noexcept {
        mysleep = static_cast<float>(seconds);
        return *this;
    }

    unsigned spin() const noexcept {
        // 0 means just yield
        return myspin;
    }

    unsigned yield() const noexcept {
        // 0 means yield forever, never sleep
        return myyield;
    }

    double sleep() const noexcept {
        return mysleep;
    }

    bool once() const noexcept {
        return my != 0;
    }

    bool spinning() const noexcept {
        // return true if this was spinning the last time
        return my < myspin;
    }
};

inline pause pause_wait() noexcept {
    // used as a backup when waiting for system wait objects fail
    #ifdef WATER_THREADS_PAUSE_WAIT
    return WATER_THREADS_PAUSE_WAIT;
    #else
    return pause().spin(0).yield(100).sleep(0.001);
    #endif
}

}}
#endif
