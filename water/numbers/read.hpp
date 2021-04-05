// Copyright 2017-2020 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_READ_HPP
#define WATER_NUMBERS_READ_HPP
#include <water/numbers/parse.hpp>
#include <water/numbers/parsed.hpp>
#include <water/numbers/parsed_to_int.hpp>
#include <water/numbers/parsed_to_float.hpp>
#include <water/numbers/max_digits.hpp>
#include <water/numbers/locale.hpp>
namespace water { namespace numbers {

/*

read floatingpoint, integer or bool from text

*/

template<typename type_>
class read
{
    parse myparse;
    type_ my {};
    size_t myused = 0;
    bool
        myerror = false,
        myinexact = false,
        myoverflow = false,
        myinfinity = false,
        mynan = false;

public:
    read() = default;

    read(settings a) :
        myparse{a}
    {}

    read(parse a) :
        myparse{a}
    {}

    template<typename forward_iterator_, typename locale_, typename transform_>
    forward_iterator_ operator()(forward_iterator_ begin, forward_iterator_ end, locale_ const& locale, transform_&& transform) {
        return do_it(begin, end, locale, transform, static_cast<typename types::ifel<numeric_limits<type_>::is_integer, int, float>::result*>(0));
    }

    template<typename forward_iterator_, typename locale_>
    forward_iterator_ operator()(forward_iterator_ begin, forward_iterator_ end, locale_ const& locale) {
        return operator()(begin, end, locale, transform_lowercase{});
    }

    template<typename forward_iterator_>
    forward_iterator_ operator()(forward_iterator_ begin, forward_iterator_ end) {
        return operator()(begin, end, locale{}, transform_lowercase{});
    }

    operator type_() const {
        return my;
    }
    
    type_ value() const {
        return my;
    }

    size_t used() const {
        // number of characters
        return myused;
    }

    bool error() const {
        // if parsing failed. this is unrelated to inexact, overflow, infinity, nan
        return myerror;
    }

    bool inexact() const {
        return myinexact;
    }

    bool overflow() const {
        return myoverflow;
    }

    bool infinity() const {
        return myinfinity;
    }

    bool nan() const {
        return mynan;
    }

    bool any_problem() const {
        return myerror || myinexact || myoverflow || myinfinity || mynan;
    }

private:
    void reset() {
        my = {};
        myused = 0;
        myerror = false;
        myinexact = false,
        myoverflow = false;
        myinfinity = false;
        mynan = false;
    }

    template<typename forward_iterator_, typename locale_, typename transform_>
    forward_iterator_ do_it(forward_iterator_ begin, forward_iterator_ end, locale_ const& locale, transform_&& transform, int*) {
        // integer
        reset();
        auto copy = myparse;
        copy.integer(true);
        parsed<max_binary_digits<type_>() + 8> t;
        forward_iterator_ r = copy(t, begin, end, locale, transform);
        if(r != begin) {
            parsed_to_int<type_> x(t);
            myerror = x.error();
            myinexact = x.inexact();
            myoverflow = x.overflow();
            if(!myerror) {
                my = x;
                myused = copy.used();
                return r;
            }
        }
        myerror = true;
        return begin;
    }

    template<typename forward_iterator_, typename locale_, typename transform_>
    forward_iterator_ do_it(forward_iterator_ begin, forward_iterator_ end, locale_ const& locale, transform_&& transform, float*) {
        // floatingpoint
        reset();
        auto copy = myparse;
        copy.integer(false);
        parsed<max_binary_digits<type_>() + 8> m;
        // max_binary_digits<int>() + 8 on a separate line from parsed<> avoided an internal compiler error on visual c++ 15.7.5
        unsigned constexpr e_digits = max_binary_digits<int>() + 8;
        parsed<e_digits> e;
        forward_iterator_ r = copy.mantissa_exponent(m, e, begin, end, locale, transform);
        if(r != begin) {
            parsed_to_float<type_> x(m, e);
            myerror = x.error();
            myinexact = x.inexact();
            myoverflow = x.overflow();
            myinfinity = x.infinity();
            mynan = x.nan();
            if(!myerror) {
                my = x;
                myused = copy.used();
                return r;
            }
        }
        myerror = true;
        return begin;
    }
};

}}
#endif
