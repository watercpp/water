// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_PARSED_TO_INT_HPP
#define WATER_NUMBERS_PARSED_TO_INT_HPP
#include <water/numbers/bits.hpp>
namespace water { namespace numbers {

/*

convert parsed to integer or bool

*/

template<typename int_>
class parsed_to_int
{
    int_ my = 0;
    bool
        myerror = false,
        myinexact = false, // if there was a decimal point
        myoverflow = false;

public:
    template<typename parsed_>
    explicit parsed_to_int(parsed_ const& parsed) {
        using limits = numeric_limits<int_>;
        if(parsed.is_infinity())
            myoverflow = true;
        else if(parsed.error() || !parsed.anything() || parsed.is_nan() || parsed.base() < 2)
            myerror = true;
        else {
            int_ const base = static_cast<int_>(parsed.base());
            bool is_signed = limits::is_signed; // not const to avoid warning C4127: conditional expression is constant :(
            unsigned round = 0;
            auto digits = parsed.digits();
            if(parsed.leading_zeros() >= parsed.point_at()) { // both can be 0 for .123
                // 0.xx or 0
                if(parsed.digits_size()) {
                    myinexact = true;
                    if(parsed.leading_zeros() == parsed.point_at())
                        round = *digits.begin();
                }
            }
            else if(!is_signed && parsed.is_minus()) // at least 1 nonzero digit before point, and negative
                myoverflow = true;
            else {
                // at least 1 nonzero digit before point
                bool const minus = is_signed && parsed.is_minus();
                auto
                    di = digits.begin(),
                    de = parsed.point_at() < parsed.digits_size() ? di + parsed.point_at() : digits.end();
                while(di != de && !myoverflow) {
                    ___water_assert(*di < parsed.base());
                    int_ d = static_cast<int_>(*di++);
                    if(minus && my >= limits::min() / base && (my *= base) >= limits::min() + d)
                        my -= d;
                    else if(!minus && my <= limits::max() / base && (my *= base) <= limits::max() - d)
                        my += d;
                    else
                        myoverflow = true;
                }
                if(!myoverflow && di != digits.end())
                    round = *di;
                if(!myoverflow && static_cast<unsigned>(-1) - parsed.leading_zeros() > parsed.digits_size() && parsed.point_at() > parsed.leading_zeros() + parsed.digits_size()) {
                    // 123xxx
                    // parsed.leading_zeros() + parsed.digits_size() cannot overflow above?
                    unsigned n = parsed.point_at() - (parsed.leading_zeros() + parsed.digits_size());
                    while(n && !myoverflow) {
                        --n;
                        if(minus && my >= limits::min() / base)
                            my *= base;
                        else if(!minus && my <= limits::max() / base)
                            my *= base;
                        else
                            myoverflow = true;
                    }
                    myinexact = !myoverflow && parsed.overflow();
                }
                if(!myoverflow && di != digits.end()) {
                    // inexact if any nonzero digits after point
                    round = *di;
                    if(round || parsed.overflow())
                        myinexact = true;
                    else
                        while(++di != digits.end() && (myinexact = *di != 0) == false);
                }
            }
            if(round && round >= parsed.base() / 2 + (parsed.base() & 1)) {
                if(parsed.is_minus() && my != limits::min())
                    --my;
                else if(!parsed.is_minus() && my != limits::max())
                    ++my;
                else
                    myoverflow = true;
            }
        }
        if(myoverflow)
            my = parsed.is_minus() ? limits::min() : limits::max();
    }

    operator int_() const {
        return my;
    }

    bool error() const {
        return myerror;
    }

    bool inexact() const {
        // could happen because
        // - digits after the decimal point
        // - parsed.inexact()
        return myinexact;
    }

    bool overflow() const {
        // less than the minimum or greater than the maximum value the type_ can represent. happens with -1 for unsigned types
        return myoverflow;
    }
};


template<>
class parsed_to_int<bool>
{
    bool
        my = 0,
        myerror = false,
        myinexact = false, // if there was a decimal point
        myoverflow = false;

public:
    template<typename parsed_>
    explicit parsed_to_int(parsed_ const& parsed) {
        parsed_to_int<unsigned> x(parsed);
        unsigned u = x;
        my = u != 0;
        myinexact = x.inexact(); // in a way anything != is true
        myoverflow = u > 1 || x.overflow();
        myerror = x.error();
    }

    operator bool() const {
        return my;
    }

    bool error() const {
        return myerror;
    }

    bool inexact() const {
        return myinexact;
    }

    bool overflow() const {
        return myoverflow;
    }
};

}}
#endif
