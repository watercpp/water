// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_LOGS_FLUSH_THREAD_WATER_HPP
#define WATER_LOGS_FLUSH_THREAD_WATER_HPP
#include <water/logs/bits.hpp>
#include <water/threads/condition.hpp>
#include <water/threads/mutex.hpp>
#include <water/threads/thread.hpp>
#include <water/later.hpp>
namespace water { namespace logs {

/*

see flush_thread.hpp

*/

class flush_thread_water
{
    using condition = threads::condition<threads::need_constexpr_constructor>;
    
    static double constexpr default_frequency_seconds = 0.1;

private:

    condition mycondition;
    threads::mutex_for_condition<condition> mymutex;
    threads::join_t my {};
    void *mybuffer = 0;
    double myseconds = 0;
    
public:

    constexpr flush_thread_water() noexcept = default;
    
    flush_thread_water(flush_thread_water const&) = delete;
    flush_thread_water& operator=(flush_thread_water const&) = delete;
    
    template<typename buffer_>
    explicit flush_thread_water(buffer_& buffer, double frequency_seconds = 0.) noexcept {
        start(buffer, frequency_seconds);
    }
    
    ~flush_thread_water() {
        stop();
    }
    
    template<typename buffer_>
    bool start(buffer_& buffer, double frequency_seconds = 0.) noexcept {
        auto lock = lock_move(mymutex);
        ___water_assert(!mybuffer && "already started");
        if(mybuffer)
            return false;
        mybuffer = &buffer;
        if(frequency_seconds > 0.)
            myseconds = frequency_seconds;
        using function = threads::member_function<flush_thread_water, &flush_thread_water::thread<buffer_>>;
        bool ok =
            threads::run<function>(this, my, threads::priority_lower) ||
            threads::run<function>(this, my);
        if(!ok)
            mybuffer = 0;
        ___water_assert(ok && "thread did not start");
        return ok;
    }
    
    void stop() noexcept {
        auto lock = lock_move(mymutex);
        if(!mybuffer)
            return;
        mybuffer = 0;
        mycondition.wake();
        auto x = my;
        lock = {};
        join(x);
    }
    
private:

    template<typename buffer_>
    void thread() {
        buffer_ *buffer = 0;
        {
            // if someone was very quick to call stop and then concurrently start, mybuffer could point to something else now
            auto lock = lock_move(mymutex);
            buffer = static_cast<buffer_*>(mybuffer);
        }
        if(!buffer)
            return;
        buffer->concurrent(true);
        auto later = water::later([buffer] {
            buffer->concurrent(false);
            buffer->flush(); // whatever was added before concurrent was switched off
        });
        while(true) {
            auto lock = lock_move(mymutex);
            if(mybuffer != buffer)
                return;
            auto seconds = myseconds > 0. ? myseconds : default_frequency_seconds;
            mycondition.wait(mymutex, seconds);
            lock = {};
            buffer->flush();
        }
    }
};

}}
#endif
