// Copyright 2017-2022 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NO_ATOMIC_INTERLOCKED_HPP
#define WATER_NO_ATOMIC_INTERLOCKED_HPP
#include <water/water.hpp>
#include <water/int.hpp>
#include <water/types/types.hpp>
#ifndef WATER_NO_ATOMIC_TESTS_FAKE_INTERLOCKED_HPP
    #include <intrin.h>
#endif
namespace water { namespace no_atomic {

/*

The _Interlocked* functions built into visual c++

*/

#if defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC) || defined(WATER_NO_ATOMIC_INTERLOCKED_ARM)
    #define WATER_NO_ATOMIC_INTERLOCKED(function, arguments, order) \
        (order == interlocked_order_relaxed ? function ## _nf arguments : \
        order == interlocked_order_acquire ? function ## _acq arguments : \
        order == interlocked_order_release ? function ## _rel arguments : \
        function arguments)
#else
    #define WATER_NO_ATOMIC_INTERLOCKED(function, arguments, order) \
        (function arguments)
#endif

#if defined(_M_IX86) || defined(WATER_NO_ATOMIC_INTERLOCKED_X86)

// make the missing 64-bit functions using _InterlockedCompareExchange64 for x86

template<typename function_>
long long interlocked64_function(long long *a, function_&& function) {
    long long r = *a, v;
    do {
        v = r;
        r = _InterlockedCompareExchange64(a, function(v), v);
    } while(r != v);
    return r;
}

inline long long _InterlockedExchange64(long long *a, long long v) {
    return interlocked64_function(a, [v](long long) { return v; });
}

inline long long _InterlockedExchangeAdd64(long long *a, long long v) {
    return interlocked64_function(a, [v](long long a) { return a + v; });
}

inline long long _InterlockedAnd64(long long *a, long long v) {
    return interlocked64_function(a, [v](long long a) { return a & v; });
}

inline long long _InterlockedOr64(long long *a, long long v) {
    return interlocked64_function(a, [v](long long a) { return a | v; });
}

inline long long _InterlockedXor64(long long *a, long long v) {
    return interlocked64_function(a, [v](long long a) { return a ^ v; });
}

#endif

template<typename type_>
constexpr bool interlocked_is_atomic() {
    return types::is_pointer<type_>::result || sizeof(type_) <= sizeof(long long);
}

enum interlocked_order {
    interlocked_order_relaxed,
    interlocked_order_consume,
    interlocked_order_acquire = interlocked_order_consume,
    interlocked_order_release,
    interlocked_order_acq_rel,
    interlocked_order_seq_cst = interlocked_order_acq_rel
};

inline char interlocked_compare_exchange(char *a, char exchange, char compare, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedCompareExchange8, (a, exchange, compare), o);
}

inline short interlocked_compare_exchange(short *a, short exchange, short compare, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedCompareExchange16, (a, exchange, compare), o);
}

inline long interlocked_compare_exchange(long *a, long exchange, long compare, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedCompareExchange, (a, exchange, compare), o);
}

inline long long interlocked_compare_exchange(long long *a, long long exchange, long long compare, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedCompareExchange64, (a, exchange, compare), o);
}

inline void* interlocked_compare_exchange(void **a, void *exchange, void *compare, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedCompareExchangePointer, (a, exchange, compare), o);
}

inline char interlocked_exchange(char *a, char b, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedExchange8, (a, b), o);
}

inline short interlocked_exchange(short *a, short b, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedExchange16, (a, b), o);
}

inline long interlocked_exchange(long *a, long b, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedExchange, (a, b), o);
}

inline long long interlocked_exchange(long long *a, long long b, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedExchange64, (a, b), o);
}

inline void* interlocked_exchange(void **a, void *b, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedExchangePointer, (a, b), o);
}

inline char interlocked_fetch_add(char *a, char b, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedExchangeAdd8, (a, b), o);
}

inline short interlocked_fetch_add(short *a, short b, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedExchangeAdd16, (a, b), o);
}

inline long interlocked_fetch_add(long *a, long b, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedExchangeAdd, (a, b), o);
}

inline long long interlocked_fetch_add(long long *a, long long b, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedExchangeAdd64, (a, b), o);
}

template<typename int_>
int_ interlocked_fetch_sub(int_ *a, int_ b, interlocked_order o) {
    return interlocked_fetch_add(a, -b, o);
}

inline char interlocked_fetch_and(char *a, char b, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedAnd8, (a, b), o);
}

inline short interlocked_fetch_and(short *a, short b, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedAnd16, (a, b), o);
}

inline long interlocked_fetch_and(long *a, long b, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedAnd, (a, b), o);
}

inline long long interlocked_fetch_and(long long *a, long long b, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedAnd64, (a, b), o);
}

inline char interlocked_fetch_or(char *a, char b, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedOr8, (a, b), o);
}

inline short interlocked_fetch_or(short *a, short b, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedOr16, (a, b), o);
}

inline long interlocked_fetch_or(long *a, long b, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedOr, (a, b), o);
}

inline long long interlocked_fetch_or(long long *a, long long b, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedOr64, (a, b), o);
}

inline char interlocked_fetch_xor(char *a, char b, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedXor8, (a, b), o);
}

inline short interlocked_fetch_xor(short *a, short b, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedXor16, (a, b), o);
}

inline long interlocked_fetch_xor(long *a, long b, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedXor, (a, b), o);
}

inline long long interlocked_fetch_xor(long long *a, long long b, interlocked_order o) {
    return WATER_NO_ATOMIC_INTERLOCKED(_InterlockedXor64, (a, b), o);
}

template<typename a_>
struct interlocked_type_from :
    types::ifel<types::is_pointer<a_>::result, void*,
    types::ifel<sizeof(a_) == sizeof(char), char,
    types::ifel<sizeof(a_) <= sizeof(short), short,
    types::ifel<sizeof(a_) <= sizeof(long), long,
    types::ifel<sizeof(a_) <= sizeof(long long), long long,
    void
>>>>> {};

template<
    typename to_,
    typename from_,
    bool copy_ =
        !(types::is_pointer<to_>::result || types::is_int<to_>::result || types::is_bool<to_>::result) ||
        !(types::is_pointer<from_>::result || types::is_int<from_>::result || types::is_bool<from_>::result)
>
struct cast {
    static constexpr to_ do_it(from_ a) {
        return static_cast<to_>(a);
    }
};

template<typename from_>
struct cast<void*, from_ const*, false> {
    static constexpr void* do_it(from_ const* a) {
        // for atomic<type const*> with InterlockedCompareExchangePointer, not for writing to const
        return const_cast<from_*>(a);
    }
};

template<typename to_, typename from_>
struct cast<to_, from_, true> {
    static to_ do_it(from_ from) {
        to_ to{};
        unsigned char
            *t = static_cast<unsigned char*>(static_cast<void*>(&to)),
            *te = t + (sizeof(to_) < sizeof(from_) ? sizeof(to_) : sizeof(from_));
        unsigned char const *f = static_cast<unsigned char const*>(static_cast<void const*>(&from));
        do *t++ = *f++; while(t != te);
        return to;
    }
};

template<typename type_>
class interlocked
{
    struct not_integer;
    using int_ = typename types::ifel_type<types::is_int<type_>, type_, not_integer>::result;
    struct not_pointer;
    using ptrdiff_ = typename types::ifel_type<types::is_pointer<type_>, ptrdiff_t, not_pointer>::result;
    using atomic_ = typename interlocked_type_from<type_>::result;
    using cast_to_type = cast<type_, atomic_>;
    using cast_to_atomic = cast<atomic_, type_>;

private:
    atomic_ my {};

public:
    constexpr interlocked() noexcept = default;
    interlocked(interlocked const&) = delete;
    interlocked& operator=(interlocked const&) = delete;

    constexpr interlocked(type_ a) noexcept :
        my{cast_to_atomic::do_it(a)}
    {}

    bool is_lock_free() const noexcept {
        return true;
    }

    type_ load(interlocked_order o = interlocked_order_seq_cst) const noexcept {
        ___water_assert(o != interlocked_order_release /*&& o != interlocked_order_acq_rel*/);
        atomic_ r;
        if(o || sizeof(type_) > sizeof(void*))
            r = interlocked_compare_exchange(const_cast<atomic_*>(&my), 0, 0, o);
        else
            r = static_cast<atomic_ const volatile&>(my);
        return cast_to_type::do_it(r);
    }

    void store(type_ a, interlocked_order o = interlocked_order_seq_cst) noexcept {
        ___water_assert(o != interlocked_order_consume && o != interlocked_order_acquire /*&& o != interlocked_order_acq_rel*/);
        atomic_ aa = cast_to_atomic::do_it(a);
        if(o || sizeof(type_) > sizeof(void*))
            interlocked_exchange(&my, aa, o);
        else
            static_cast<atomic_ volatile&>(my) = aa;
    }

    type_ exchange(type_ a, interlocked_order o = interlocked_order_seq_cst) noexcept {
        return cast_to_type::do_it(interlocked_exchange(&my, cast_to_atomic::do_it(a), o));
    }

    bool compare_exchange_strong(type_& expected, type_ desired, interlocked_order o = interlocked_order_seq_cst) noexcept {
        atomic_ e = cast_to_atomic::do_it(expected);
        atomic_ r = interlocked_compare_exchange(&my, cast_to_atomic::do_it(desired), e, o);
        expected = cast_to_type::do_it(r);
        return r == e;
    }

    bool compare_exchange_strong(type_& expected, type_ desired, interlocked_order success, interlocked_order failure) noexcept {
        interlocked_order o =
            success == failure ? success :
            success == interlocked_order_relaxed ? failure :
            failure == interlocked_order_relaxed ? success :
            interlocked_order_seq_cst;
        return compare_exchange_strong(expected, desired, o);
    }

    bool compare_exchange_weak(type_& expected, type_ desired, interlocked_order o = interlocked_order_seq_cst) noexcept {
        return compare_exchange_strong(expected, desired, o);
    }

    bool compare_exchange_weak(type_& expected, type_ desired, interlocked_order success, interlocked_order failure) noexcept {
        return compare_exchange_strong(expected, desired, success, failure);
    }

    type_ fetch_add(int_ a, interlocked_order o = interlocked_order_seq_cst) noexcept {
        return cast_to_type::do_it(interlocked_fetch_add(&my, cast_to_atomic::do_it(a), o));
    }

    type_ fetch_sub(int_ a, interlocked_order o = interlocked_order_seq_cst) noexcept {
        return cast_to_type::do_it(interlocked_fetch_sub(&my, cast_to_atomic::do_it(a), o));
    }

    type_ fetch_and(int_ a, interlocked_order o = interlocked_order_seq_cst) noexcept {
        return cast_to_type::do_it(interlocked_fetch_and(&my, cast_to_atomic::do_it(a), o));
    }

    type_ fetch_or(int_ a, interlocked_order o = interlocked_order_seq_cst) noexcept {
        return cast_to_type::do_it(interlocked_fetch_or(&my, cast_to_atomic::do_it(a), o));
    }

    type_ fetch_xor(int_ a, interlocked_order o = interlocked_order_seq_cst) noexcept {
        return cast_to_type::do_it(interlocked_fetch_xor(&my, cast_to_atomic::do_it(a), o));
    }

    type_ fetch_add(ptrdiff_ a, interlocked_order o = interlocked_order_seq_cst) noexcept {
        type_ t = cast_to_type::do_it(my);
        while(!compare_exchange_strong(t, t + a, o));
        return t;
    }

    type_ fetch_sub(ptrdiff_ a, interlocked_order o = interlocked_order_seq_cst) noexcept {
        return fetch_add(-a, o);
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
};

}}
#endif
