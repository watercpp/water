// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_INT_HPP
#define WATER_INT_HPP
#include <water/water.hpp>
#include <water/numeric_limits.hpp>
#include <water/types.hpp>
namespace water {

/*

using int32_t = int_bits_at_least<32>;
using uint32_t = uint_bits_at_least<32>;

Select integer types with at least or exactly a specific number of bits, bytes or decimal digits.
Similar to the fixed width integer types from cstdint / stdint.h except these are templates.

Always selects the smallest possible type, and prefers to select char, unsigned char, int, unsigned int (in this order).
If there is no matching type, the type is void.

int_largest_t + uint_largest_t are the largest integer types

*/

namespace _ { namespace int_uint {

    template<typename type_>
    struct result_type {
        using result = type_;
    };

    // lookup in order of preference

    template<typename sign_, unsigned at_> struct lookup;

    template<> struct lookup<signed, 0> : result_type<ifel<char(-1) >= char(0), void, char>> {};
    template<> struct lookup<signed, 1> : result_type<int> {};
    template<> struct lookup<signed, 2> : result_type<signed char> {};
    template<> struct lookup<signed, 3> : result_type<short> {};
    template<> struct lookup<signed, 4> : result_type<long> {};
    template<> struct lookup<signed, 5> : result_type<long long> {};

    template<> struct lookup<unsigned, 0> : result_type<ifel<char(-1) >= char(0), char, void>> {};
    template<> struct lookup<unsigned, 1> : result_type<unsigned char> {};
    template<> struct lookup<unsigned, 2> : result_type<unsigned int> {};
    template<> struct lookup<unsigned, 3> : result_type<unsigned short> {};
    template<> struct lookup<unsigned, 4> : result_type<unsigned long> {};
    template<> struct lookup<unsigned, 5> : result_type<unsigned long long> {};

    unsigned constexpr lookup_size = 6;

    template<
        template<size_t, typename, typename> class select_,
        typename sign_,
        size_t size_ = 0,
        unsigned at_ = 0,
        typename result_ = void
    >
    struct find :
        find<
            select_,
            sign_,
            size_,
            at_ + 1,
            typename select_<size_, result_, typename lookup<sign_, at_>::result>::result
        > {};
    template<
        template<size_t, typename, typename> class select_,
        typename sign_,
        size_t size_,
        typename result_
    >
    struct find<select_, sign_, size_, lookup_size, result_> :
        result_type<result_>
    {};

    template<size_t digits_, typename old_, typename new_>
    struct digits2_at_least :
        result_type<ifel<(digits_ <= numeric_limits<new_>::digits && numeric_limits<old_>::digits > numeric_limits<new_>::digits), new_, old_>>
    {};

    template<size_t digits_>
    struct digits2_at_least<digits_, void, void> :
        result_type<void>
    {};

    template<size_t digits_, typename old_>
    struct digits2_at_least<digits_, old_, void> :
        result_type<old_>
    {};

    template<size_t digits_, typename new_>
    struct digits2_at_least<digits_, void, new_> :
        result_type<ifel<digits_ <= numeric_limits<new_>::digits, new_, void>>
    {};



    template<size_t size_, typename old_, typename new_>
    struct size_at_least :
        result_type<ifel<(size_ <= sizeof(new_) && sizeof(old_) > sizeof(new_)), new_, old_>>
    {};

    template<size_t size_>
    struct size_at_least<size_, void, void> :
        result_type<void>
    {};

    template<size_t size_, typename old_>
    struct size_at_least<size_, old_, void> :
        result_type<old_>
    {};

    template<size_t size_, typename new_>
    struct size_at_least<size_, void, new_> :
        result_type<ifel<size_ <= sizeof(new_), new_, void>>
    {};



    template<size_t digits_, typename old_, typename new_>
    struct digits10_at_least :
        result_type<ifel<(digits_ <= numeric_limits<new_>::digits10 && numeric_limits<old_>::digits10 > numeric_limits<new_>::digits10), new_, old_>>
    {};

    template<size_t digits_>
    struct digits10_at_least<digits_, void, void> :
        result_type<void>
    {};

    template<size_t digits_, typename old_>
    struct digits10_at_least<digits_, old_, void> :
        result_type<old_>
    {};

    template<size_t digits_, typename new_>
    struct digits10_at_least<digits_, void, new_> :
        result_type<ifel<digits_ <= numeric_limits<new_>::digits10, new_, void>>
    {};



    template<size_t, typename old_, typename new_>
    struct largest :
        result_type<ifel<(numeric_limits<old_>::digits >= numeric_limits<new_>::digits), old_, new_>>
    {};



    template<typename int_, size_t size_>
    struct if_size_exactly :
        result_type<ifel<sizeof(int_) == size_, int_, void>>
    {};

    template<size_t size_>
    struct if_size_exactly<void, size_> :
        result_type<void>
    {};

}}

using int_largest_t  = typename _::int_uint::find<_::int_uint::largest, signed>::result;
using uint_largest_t = typename _::int_uint::find<_::int_uint::largest, unsigned>::result;

template<size_t bits_> using int_bits_at_least  = typename _::int_uint::find<_::int_uint::digits2_at_least, signed, bits_ ? bits_ - 1 : bits_>::result;
template<size_t bits_> using uint_bits_at_least = typename _::int_uint::find<_::int_uint::digits2_at_least, unsigned, bits_>::result;

template<size_t bits_> using int_bits  = ifel<numeric_limits<int_bits_at_least<bits_>>::digits + 1 == bits_, int_bits_at_least<bits_>, void>;
template<size_t bits_> using uint_bits = ifel<numeric_limits<uint_bits_at_least<bits_>>::digits == bits_, uint_bits_at_least<bits_>, void>;

template<size_t size_> using int_size_at_least  = typename _::int_uint::find<_::int_uint::size_at_least, signed, size_>::result;
template<size_t size_> using uint_size_at_least = typename _::int_uint::find<_::int_uint::size_at_least, unsigned, size_>::result;

template<size_t size_> using int_size  = typename _::int_uint::if_size_exactly<int_size_at_least<size_>, size_>::result;
template<size_t size_> using uint_size = typename _::int_uint::if_size_exactly<uint_size_at_least<size_>, size_>::result;

template<size_t digits_> using int_digits10_at_least  = typename _::int_uint::find<_::int_uint::digits10_at_least, signed, digits_>::result;
template<size_t digits_> using uint_digits10_at_least = typename _::int_uint::find<_::int_uint::digits10_at_least, unsigned, digits_>::result;

}
#endif
