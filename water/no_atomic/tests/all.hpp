// Copyright 2022 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NO_ATOMIC_TESTS_ALL_HPP
#define WATER_NO_ATOMIC_TESTS_ALL_HPP
#include <water/water.hpp>
#include <water/test.hpp>
#include <water/no_atomic/atomic.hpp>
namespace water { namespace no_atomic { namespace tests {

template<typename type_>
constexpr bool is_atomic() {
    #ifdef WATER_COMPILER_MICROSOFT
    return interlocked_is_atomic<type_>();
    #else
    return gcclang_is_atomic<type_>();
    #endif
}

template<typename type_>
void common(type_ const v) {
    // for pointers and integers
    atomic<type_> a1;
    atomic<type_> a2{v};
    bool b = a1.is_lock_free();
    ___water_test(b);
    a1.store(a2.load());
    ___water_test(a1.load() == v && a2.load() == v);
    a2.store(0, memory_order_seq_cst);
    a1.store(a2.load(memory_order_relaxed), memory_order_release);
    ___water_test(a1.load(memory_order_acquire) == 0 && a2.load(memory_order_consume) == 0);
    auto x = a1.exchange(v);
    ___water_test(a1.load() == v && x == 0);
    x = a1.exchange(0, memory_order_acq_rel);
    ___water_test(a1.load() == 0 && x == v);
    
    a1.store(0);
    x = v + 2;
    unsigned n = 0;
    while(!a1.compare_exchange_strong(x, x + 1, memory_order_acq_rel, memory_order_relaxed))
        ++n;
    ___water_test(n > 0 && x == 0 && a1.load() == x + 1);
    
    a1.store(v);
    x = 0;
    n = 0;
    while(!a1.compare_exchange_strong(x, x + 1, memory_order_acq_rel))
        ++n;
    ___water_test(n > 0 && x == v && a1.load() == x + 1);
    
    a1.store(0);
    x = v + 2;
    n = 0;
    while(!a1.compare_exchange_weak(x, x + 1, memory_order_acq_rel, memory_order_relaxed))
        ++n;
    ___water_test(n > 0 && x == 0 && a1.load() == x + 1);
    
    a1.store(v);
    x = 0;
    n = 0;
    while(!a1.compare_exchange_weak(x, x + 1, memory_order_acq_rel))
        ++n;
    ___water_test(n > 0 && x == v && a1.load() == x + 1);
    
    a1.store(v);
    x = a1.fetch_add(1, memory_order_acq_rel);
    ___water_test(x == v && a1.load() == x + 1);
    
    a1.store(v);
    x = a1.fetch_sub(1, memory_order_acq_rel);
    ___water_test(x == v && a1.load() == x - 1);
    
    a1.store(v);
    x = a1.fetch_add(1);
    ___water_test(x == v && a1.load() == x + 1);
    
    a1.store(v);
    x = a1.fetch_sub(1);
    ___water_test(x == v && a1.load() == x - 1);
    
    a1 = v;
    ___water_test(a1 == v);
    ++a1;
    ___water_test(a1 == v + 1);
    --a1;
    ___water_test(a1 == v);
    x = a1++;
    ___water_test(a1 == v + 1 && x == v);
    x = a1--;
    ___water_test(a1 == v && x == v + 1);
    x = a1 += 1;
    ___water_test(a1 == v + 1 && x == v + 1);
    x = a1 -= 1;
    ___water_test(a1 == v && x == v);
}

template<typename type_>
void integer() {
    atomic<type_> a1{123};
    
    type_ x = 0;
    
    a1.store(0xf);
    x = a1.fetch_and(0x7, memory_order_acq_rel);
    ___water_test(x == 0xf && a1.load() == 0x7);
    
    a1.store(0x5);
    x = a1.fetch_or(0xa, memory_order_acq_rel);
    ___water_test(x == 0x5 && a1.load() == 0xf);
    
    a1.store(0xf);
    x = a1.fetch_xor(0x5, memory_order_acq_rel);
    ___water_test(x == 0xf && a1.load() == 0xa);
    
    a1.store(0xf);
    x = a1.fetch_and(0x7);
    ___water_test(x == 0xf && a1.load() == 0x7);
    
    a1.store(0x5);
    x = a1.fetch_or(0xa);
    ___water_test(x == 0x5 && a1.load() == 0xf);
    
    a1.store(0xf);
    x = a1.fetch_xor(0x5);
    ___water_test(x == 0xf && a1.load() == 0xa);
    
    a1 = 0xf;
    x = a1 &= 0x7;
    ___water_test(x == 0x7 && a1 == 0x7);
    
    a1 = 0x5;
    x = a1 |= 0xa;
    ___water_test(x == 0xf && a1 == 0xf);
    
    a1 = 0xf;
    x = a1 ^= 0x5;
    ___water_test(x == 0xa && a1 == 0xa);
    
}

template<typename type_, bool atomic_ = is_atomic<type_>()>
struct test {
    static void all() {
        type_ v = 123;
        common<type_>(v);
        integer<type_>();
    }
};

template<typename type_>
struct test<type_*, true> {
    static void all() {
        type_ x{};
        common<type_*>(&x);
    }
};

template<typename type_>
struct test<type_, false> {
    static void all() {}
};

inline void all() {
    test<char>::all();
    test<short>::all();
    test<int>::all();
    test<long>::all();
    test<long long>::all();
    test<unsigned char>::all();
    test<unsigned short>::all();
    test<unsigned int>::all();
    test<unsigned long>::all();
    test<unsigned long long>::all();
    test<char*>::all();
    test<int*>::all();
}

}}}
#endif
