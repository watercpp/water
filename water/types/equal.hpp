// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_EQUAL_HPP
#define WATER_TYPES_EQUAL_HPP
#include <water/types/result.hpp>
namespace water { namespace types {

/*

equal<a_, b_>::result is true if the result of a_ and b_ are equal. They are if
- any_result<a_> and any_result<b_> have the same result_kind, and
- any_result<a_>::result and any_result<b_>::result are equal

Example:
- equal<int, int>::result is true
- equal<int, void>::result is false
- equal<int, no_const<no_pointer<int const*> > >::result is true
- equal<integer<int, 123>, integer<unsigned short, 123> >::result is true
- equal<integer<int, 123>, integer<int, 321> >::result is false
- equal<int, integer<int, 123> >::result is false


The result_kind should have:

    struct some_kind {
        template<typename a_, typename b_> struct equal
    };

equal_plain<a_, b_>::result is true if a_ and b_ are exactly the same type

*/

namespace _ {

    template<
        typename a_,
        typename b_,
        typename ta_ = typename a_::result_tag,
        typename tb_ = typename b_::result_tag,
        typename x_ = void
    >
    struct do_equal :
        false_result
    {};
    
    template<typename a_, typename b_, typename k_>
    struct do_equal<a_, b_, result_tag<k_>, result_tag<k_>, to_void<typename k_::template equal<void, void> >> :
        k_::template equal<a_, b_>
    {};
}

template<typename a_, typename b_>
struct equal :
    _::do_equal<any_result<a_>, any_result<b_> >
{};

}}
#endif
