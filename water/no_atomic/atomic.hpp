// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NO_ATOMIC_ATOMIC_HPP
#define WATER_NO_ATOMIC_ATOMIC_HPP
#include <water/water.hpp>

#ifdef WATER_COMPILER_MICROSOFT
#include <water/no_atomic/interlocked.hpp>
namespace water { namespace no_atomic {
template<typename type_> using atomic = interlocked<type_>;
using memory_order = interlocked_order;
memory_order constexpr memory_order_relaxed = interlocked_order_relaxed;
memory_order constexpr memory_order_consume = interlocked_order_consume;
memory_order constexpr memory_order_acquire = interlocked_order_acquire;
memory_order constexpr memory_order_release = interlocked_order_release;
memory_order constexpr memory_order_acq_rel = interlocked_order_acq_rel;
memory_order constexpr memory_order_seq_cst = interlocked_order_seq_cst;
}}

#else
#include <water/no_atomic/gcclang.hpp>
namespace water { namespace no_atomic {
template<typename type_> using atomic = gcclang<type_>;
using memory_order = gcclang_order;
memory_order constexpr memory_order_relaxed = gcclang_order_relaxed;
memory_order constexpr memory_order_consume = gcclang_order_consume;
memory_order constexpr memory_order_acquire = gcclang_order_acquire;
memory_order constexpr memory_order_release = gcclang_order_release;
memory_order constexpr memory_order_acq_rel = gcclang_order_acq_rel;
memory_order constexpr memory_order_seq_cst = gcclang_order_seq_cst;
}}

#endif
#endif
