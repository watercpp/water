// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_LOGS_FLUSH_THREAD_HPP
#define WATER_LOGS_FLUSH_THREAD_HPP
#include <water/logs/bits.hpp>
#if defined(WATER_NO_STD) || defined(WATER_USE_WATER_THREADS)
    #include <water/logs/flush_thread_water.hpp>
#else
    #include <water/logs/flush_thread_std.hpp>
#endif
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

#if defined(WATER_NO_STD) || defined(WATER_USE_WATER_THREADS)
using flush_thread = flush_thread_water;
#else
using flush_thread = flush_thread_std;
#endif

}}
#endif
