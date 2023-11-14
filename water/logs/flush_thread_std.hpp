// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_LOGS_FLUSH_THREAD_STD_HPP
#define WATER_LOGS_FLUSH_THREAD_STD_HPP
#include <water/logs/bits.hpp>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <water/later.hpp>
namespace water { namespace logs {

/*

see flush_thread.hpp

*/

class flush_thread_std
{
    static double constexpr default_frequency_seconds = 0.1;
    
    using unique_lock = std::unique_lock<std::mutex>;

private:

    std::mutex mymutex;
    std::condition_variable mycondition;
    std::thread my;
    void *mybuffer = 0;
    double myseconds = 0;
    
public:

    flush_thread_std() = default;
    
    flush_thread_std(flush_thread_std const&) = delete;
    flush_thread_std& operator=(flush_thread_std const&) = delete;
    
    template<typename buffer_>
    explicit flush_thread_std(buffer_& buffer, double frequency_seconds = 0.) {
        start(buffer, frequency_seconds);
    }
    
    ~flush_thread_std() {
        stop();
    }
    
    template<typename buffer_>
    bool start(buffer_& buffer, double frequency_seconds = 0.) {
        unique_lock lock{mymutex};
        ___water_assert(!mybuffer && "already started");
        if(mybuffer)
            return false;
        mybuffer = &buffer;
        if(frequency_seconds > 0.)
            myseconds = frequency_seconds;
        std::thread run(
            [this]{
                this->template thread<buffer_>();
            }
        );
        swap(my, run);
        return true;
    }
    
    void stop() {
        std::thread x;
        {
            unique_lock lock{mymutex};
            if(!mybuffer)
                return;
            mybuffer = 0;
            mycondition.notify_one();
            swap(x, my);
        };
        x.join();
    }
    
private:

    template<typename buffer_>
    void thread() {
        buffer_ *buffer = 0;
        {
            // if someone was very quick to call stop and then concurrently start, mybuffer could point to something else now
            unique_lock lock{mymutex};
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
            {
                unique_lock lock{mymutex};
                if(mybuffer != buffer)
                    return;
                auto seconds = myseconds > 0. ? myseconds : default_frequency_seconds;
                mycondition.wait_for(lock, std::chrono::duration<double>{seconds});
            }
            buffer->flush();
        }
    }
};

}}
#endif
