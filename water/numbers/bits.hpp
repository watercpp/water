// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_BITS_HPP
#define WATER_NUMBERS_BITS_HPP
#include <water/water.hpp>
#include <water/int.hpp>
#include <water/iterator.hpp>
#include <water/signed_representation.hpp>
#include <water/numeric_limits.hpp>
#include <water/ascii_lower_upper.hpp>
#include <water/unicode/utf.hpp>
#include <water/cmath.hpp>
#include <water/char8.hpp>
#include <water/begin_end.hpp>
#include <water/types.hpp>
#include <water/is_no_to.hpp>
namespace water { namespace numbers {



namespace _ {

    template<typename a_, typename = void>
    struct has_begin_end_do {
        static bool constexpr result = false;
    };
    
    template<typename a_>
    struct has_begin_end_do<a_, to_void<decltype(make_type<a_ const&>().begin() == make_type<a_ const&>().end())>> {
        static bool constexpr result = true;
    };

}

template<typename a_>
bool constexpr has_begin_end = _::has_begin_end_do<no_const_or_reference<a_>>::result;



// ascii lowercase used when parsing numbers
struct transform_lowercase {
    char32_t operator()(char32_t a) const {
        return ascii_to_lower(a);
    }
};



// group base 10 by 3 digits, base 2 by 8 digits, others by 4 digits
struct group {
    bool operator()(unsigned distance_to_point, bool /*after_point*/, unsigned base) const {
        // distance_to_point is never 0. the number of digits to the decimal point
        unsigned every = base == 10 ? 3 : base == 2 ? 8 : 4;
        return (distance_to_point % every) == 0;
    }
};



}}
#endif
