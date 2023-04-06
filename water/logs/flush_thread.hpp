// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_LOGS_FLUSH_THREAD_HPP
#define WATER_LOGS_FLUSH_THREAD_HPP
#include <water/logs/bits.hpp>
#include <water/threads/condition.hpp>
#include <water/threads/mutex.hpp>
#include <water/later.hpp>
namespace water { namespace logs {

/*

Use like this:

    water::logs::buffer<...> log;

    void main() {
        water::logs::flush_thread flush{log};
        log("hello world");
    }

Or call start and stop manually:

    water::logs::buffer<...> log;
    
    void main() {
        water::logs::flush_thread flush;
        flush.start(log);
        log("hello world");
        flush.stop();
        flush.start(log);
        log("hello world");
        flush.stop();
    }

Never call start() while another thread is calling stop().

*/

class flush_thread
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

    constexpr flush_thread() noexcept = default;
    
    flush_thread(flush_thread const&) = delete;
    flush_thread& operator=(flush_thread const&) = delete;
    
    template<typename buffer_>
    explicit flush_thread(buffer_& buffer, double frequency_seconds = 0.) noexcept {
        start(buffer, frequency_seconds);
    }
    
    ~flush_thread() {
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
        threads::run_options options;
        if(threads::qos_exists)
            options.qos(threads::qos_utility);
        else if(threads::priority_exists) {
            auto priority = threads::priority_default();
            if(priority > 1)
                options.priority(--priority);
        }
        using function = threads::member_function<flush_thread, &flush_thread::thread<buffer_>>;
        bool ok =
            threads::run<function>(this, my, options) ||
            threads::run<function>(this, my);
        if(!ok)
            mybuffer = 0;
        ___water_assert(ok || "thread did not start");
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
