// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_STATISTICS_BITS_HPP
#define WATER_THREADS_STATISTICS_BITS_HPP
#include <water/threads/bits.hpp>
namespace water { namespace threads { namespace statistics {

using count = atomic_uint;
using count_t = decltype(count{}.load());

bool constexpr exists =
    #ifdef WATER_THREADS_STATISTICS
    atomic_exists;
    #else
    false;
    #endif

template<bool = atomic_exists>
struct atomic_if {
    static count_t get(count& a) noexcept { return a.load(memory_order_relaxed); }
    static void add(count& a) noexcept { a.fetch_add(1, memory_order_relaxed); }
    static void set(count& a, count_t b) noexcept { a.store(b, memory_order_relaxed); }
};

template<>
struct atomic_if<false> {
    static count_t get(count&) noexcept { return 0; }
    static void add(count&) noexcept {}
    static void set(count&, count_t) noexcept {}
};

}}}
#endif
