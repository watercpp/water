// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NO_ATOMIC_GCCLANG_HPP
#define WATER_NO_ATOMIC_GCCLANG_HPP
#include <water/water.hpp>
#include <water/int.hpp>
#include <water/types.hpp>
#include <water/is_no_to.hpp>
namespace water { namespace no_atomic {

/*

The __atomic_* functions built in to GCC and Clang

https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html

__atomic_always_lock_free(size) is compile time constant

*/

enum gcclang_order {
    gcclang_order_relaxed = __ATOMIC_RELAXED,
    gcclang_order_consume = __ATOMIC_CONSUME,
    gcclang_order_acquire = __ATOMIC_ACQUIRE,
    gcclang_order_release = __ATOMIC_RELEASE,
    gcclang_order_acq_rel = __ATOMIC_ACQ_REL,
    gcclang_order_seq_cst = __ATOMIC_SEQ_CST
};

template<typename type_>
constexpr bool gcclang_is_atomic() {
    return __atomic_always_lock_free(sizeof(type_), 0);
}

constexpr unsigned gcclang_order_strength(gcclang_order a) {
    return
        a == gcclang_order_consume ? 1 :
        a == gcclang_order_acquire ? 2 :
        a == gcclang_order_release ? 2 :
        a == gcclang_order_acq_rel ? 3 :
        a == gcclang_order_seq_cst ? 4 :
        0;
}

template<typename type_>
class gcclang
{
    struct not_integer;
    using int_ = ifel<is_int<type_>, type_, not_integer>;
    struct not_pointer;
    using ptrdiff_ = ifel<is_pointer<type_>, ptrdiff_t, not_pointer>;

private:
    type_ my {};

public:
    constexpr gcclang() noexcept = default;
    gcclang(gcclang const&) = delete;
    gcclang& operator=(gcclang const&) = delete;

    constexpr gcclang(type_ a) noexcept :
        my{a}
    {}

    bool is_lock_free() const noexcept {
        return __atomic_is_lock_free(sizeof(type_), &my);
    }

    type_ load(gcclang_order o = gcclang_order_seq_cst) const noexcept {
        ___water_assert(o != gcclang_order_release && o != gcclang_order_acq_rel);
        type_ r;
        __atomic_load(&my, &r, o);
        return r;
    }

    void store(type_ a, gcclang_order o = gcclang_order_seq_cst) noexcept {
        ___water_assert(o != gcclang_order_consume && o != gcclang_order_acquire && o != gcclang_order_acq_rel);
        __atomic_store(&my, &a, o);
    }

    type_ exchange(type_ a, gcclang_order o = gcclang_order_seq_cst) noexcept {
        type_ r;
        __atomic_exchange(&my, &a, &r, o);
        return r;
    }

    bool compare_exchange_strong(type_& expected, type_ desired, gcclang_order success, gcclang_order failure) noexcept {
        return compare_exchange_do(true, expected, desired, success, failure);
    }

    bool compare_exchange_weak(type_& expected, type_ desired, gcclang_order success, gcclang_order failure) noexcept {
        return compare_exchange_do(true, expected, desired, success, failure);
    }

    bool compare_exchange_strong(type_& expected, type_ desired, gcclang_order o = gcclang_order_seq_cst) noexcept {
        return compare_exchange_do(false, expected, desired, o, o == gcclang_order_acq_rel ? gcclang_order_acquire : o == gcclang_order_release ? gcclang_order_relaxed : o);
    }

    bool compare_exchange_weak(type_& expected, type_ desired, gcclang_order o = gcclang_order_seq_cst) noexcept {
        return compare_exchange_do(true, expected, desired, o, o == gcclang_order_acq_rel ? gcclang_order_acquire : o == gcclang_order_release ? gcclang_order_relaxed : o);
    }

    type_ fetch_add(int_ a, gcclang_order o = gcclang_order_seq_cst) noexcept {
        return __atomic_fetch_add(&my, a, o);
    }

    type_ fetch_sub(int_ a, gcclang_order o = gcclang_order_seq_cst) noexcept {
        return __atomic_fetch_sub(&my, a, o);
    }

    type_ fetch_and(int_ a, gcclang_order o = gcclang_order_seq_cst) noexcept {
        return __atomic_fetch_and(&my, a, o);
    }

    type_ fetch_or(int_ a, gcclang_order o = gcclang_order_seq_cst) noexcept {
        return __atomic_fetch_or(&my, a, o);
    }

    type_ fetch_xor(int_ a, gcclang_order o = gcclang_order_seq_cst) noexcept {
        return __atomic_fetch_xor(&my, a, o);
    }

    type_ fetch_add(ptrdiff_ a, gcclang_order o = gcclang_order_seq_cst) noexcept {
        return __atomic_fetch_add(&my, a * static_cast<ptrdiff_t>(sizeof(no_pointer<type_>)), o);
    }

    type_ fetch_sub(ptrdiff_ a, gcclang_order o = gcclang_order_seq_cst) noexcept {
        return __atomic_fetch_sub(&my, a * static_cast<ptrdiff_t>(sizeof(no_pointer<type_>)), o);
    }

    operator type_() const {
        return load();
    }

    type_ operator=(type_ a) {
        store(a);
        return a;
    }

    type_ operator++() {
        return fetch_add(1) + 1;
    }

    type_ operator--() {
        return fetch_sub(1) - 1;
    }

    type_ operator++(int) {
        return fetch_add(1);
    }

    type_ operator--(int) {
        return fetch_sub(1);
    }

    type_ operator+=(int_ a) {
        return fetch_add(a) + a;
    }

    type_ operator-=(int_ a) {
        return fetch_sub(a) - a;
    }

    type_ operator&=(int_ a) {
        return fetch_and(a) & a;
    }

    type_ operator|=(int_ a) {
        return fetch_or(a) | a;
    }

    type_ operator^=(int_ a) {
        return fetch_xor(a) ^ a;
    }

    type_ operator+=(ptrdiff_ a) {
        return fetch_add(a) + a;
    }

    type_ operator-=(ptrdiff_ a) {
        return fetch_sub(a) - a;
    }

private:
    bool compare_exchange_do(bool weak, type_& expected, type_ desired, gcclang_order success, gcclang_order failure) noexcept {
        ___water_assert(failure != gcclang_order_release && failure != gcclang_order_acq_rel);
        ___water_assert(gcclang_order_strength(failure) <= gcclang_order_strength(success));
        return __atomic_compare_exchange(&my, &expected, &desired, weak, success, failure);
    }
    
};

}}
#endif
