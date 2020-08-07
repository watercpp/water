// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_HPP
#define WATER_ATOMIC_HPP
#include <water/water.hpp>

/*

Select atomic operations:
- std::atomic is prefered
- water::no_atomic is a fallback
- #define WATER_NO_ATOMIC if there are no atomic operations

*/

#if defined(WATER_NO_ATOMIC)
namespace water {

bool constexpr atomic_exists = false;

template<typename type_>
struct atomic {
    type_ load() const; // for decltype
};

using atomic_int = atomic<ptrdiff_t>;
using atomic_uint = atomic<size_t>;

enum memory_order {
    memory_order_relaxed
    memory_order_consume
    memory_order_acquire
    memory_order_release
    memory_order_acq_rel
    memory_order_seq_cst
};

}

#elif defined(WATER_NO_STD) || defined(WATER_NO_STD_ATOMIC)
#include <water/no_atomic/atomic.hpp>
namespace water {

bool constexpr atomic_exists = true;

using no_atomic::atomic;
using atomic_int = atomic<ptrdiff_t>;
using atomic_uint = atomic<size_t>;

using no_atomic::memory_order;
using no_atomic::memory_order_relaxed;
using no_atomic::memory_order_consume;
using no_atomic::memory_order_acquire;
using no_atomic::memory_order_release;
using no_atomic::memory_order_acq_rel;
using no_atomic::memory_order_seq_cst;

}

#else
#include <atomic>
namespace water {

bool constexpr atomic_exists = true;

using std::atomic;
using atomic_int = atomic<ptrdiff_t>;
using atomic_uint = atomic<size_t>;

using std::memory_order;
using std::memory_order_relaxed;
using std::memory_order_consume;
using std::memory_order_acquire;
using std::memory_order_release;
using std::memory_order_acq_rel;
using std::memory_order_seq_cst;

}

#endif
#endif
