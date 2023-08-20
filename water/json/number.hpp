// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_NUMBER_HPP
#define WATER_JSON_NUMBER_HPP
#include <water/json/bits.hpp>
#include <water/numeric_limits.hpp>
#include <water/cmath.hpp>
namespace water { namespace json {

//
// the safe conversions are to and from int64_t and double, the others are just dumb casts
//
class number
{
    int64_t myi = 0;
    int32_t mye = 0;
    bool myimprecise = false;

public:
    constexpr number() = default;

    number(int64_t i, int32_t e, bool imprecise = false) :
        myi(i),
        mye(e),
        myimprecise(imprecise)
    {
        if(e) normalize();
    }

    number(double a) {
        if(isinf_strict(a) || isnan_strict(a))
            myimprecise = true;
        else if(a) {
            // can still be subnormal
            // this is stolen from ministr
            double m = a;
            if(a < 0)
                m = -m;
            mye = static_cast<int32_t>(log10(m));
            auto p = mye;
            if(m < numeric_limits<double>::min()) // subnormal
                do ++p; while((m *= static_cast<double>(10)) < numeric_limits<double>::min());
            m /= pow(10., p);
            if(m < 1.) {
                m *= 10.;
                --mye;
            }
            else if(m > 10.) {
                m /= 10.;
                ++mye;
            }
            mye -= numeric_limits<int64_t>::digits10 - 1;
            myi = static_cast<int64_t>(m * pow(10., numeric_limits<int64_t>::digits10 - 1));
            if(a < 0)
                myi = -myi;
            normalize();
        }
    }

    number(char a) : number{a, 0} {}
    number(signed char a) : number{a, 0} {}
    number(unsigned char a) : number{a, 0} {}
    number(short a) : number{a, 0} {}
    number(int a) : number{a, 0} {}
    number(long a) : number{a, 0} {}
    number(long long a) : number{a, 0} {}
    number(unsigned short a) : number{static_cast<int64_t>(a), 0} {}
    number(unsigned int a) : number{static_cast<int64_t>(a), 0} {}
    number(unsigned long a) : number{static_cast<int64_t>(a), 0} {}
    number(unsigned long long a) : number{static_cast<int64_t>(a), 0} {}
    number(float a) : number{static_cast<double>(a)} {}
    number(long double a) : number{static_cast<double>(a)} {}
    
    int64_t integer() const {
        return myi;
    }

    int32_t exponent() const {
        // this is 0 when int64_t can represent the number
        return mye;
    }

    bool imprecise() const {
        // overflow (exponent > max)
        // to small (exponent < min)
        // to many digits, integer() cannot represent all digits
        return myimprecise;
    }

    bool overflow() const {
        return
            myimprecise &&
            mye == numeric_limits<int32_t>::max() &&
            (myi == numeric_limits<int64_t>::min() || myi == numeric_limits<int64_t>::max());
    }

    double to_double() const {
        // returns 0 if infinity, nan, subnormal
        double r = static_cast<double>(myi);
        if(mye) {
            auto e = mye;
            int32_t const e_small = numeric_limits<double>::min_exponent10 + (numeric_limits<double>::digits10 + 2);
            if(e <= e_small) {
                // if it's really small do two pow()
                r *= pow(10., e - e_small);
                e = e_small;
            }
            r *= pow(10., e);
        }
        if(!isfinite_strict(r))
            r = 0;
        return r;
    }

    int64_t to_int() const {
        // returns 0 when it would overflow
        if(!mye)
            return myi;
        if(mye > 0)
            return 0;
        auto i = myi;
        auto e = mye;
        while(i && e < -1) {
            ++e;
            i /= 10;
        }
        int64_t round = (i - (i / 10) * 10) / 5;
        return i / 10 + round;
    }

    explicit operator bool() const {
        return myi != 0;
    }

    explicit operator double() const {
        return to_double();
    }

    explicit operator float() const {
        auto r = static_cast<float>(to_double());
        if(!isfinite_strict(r)) r = 0;
        return r;
    }

    explicit operator long double() const {
        return to_double();
    }

    explicit operator char() const {
        return static_cast<char>(to_int());
    }

    explicit operator signed char() const {
        return static_cast<signed char>(to_int());
    }

    explicit operator unsigned char() const {
        return static_cast<unsigned char>(to_int());
    }

    explicit operator short() const {
        return static_cast<short>(to_int());
    }

    explicit operator int() const {
        return static_cast<int>(to_int());
    }

    explicit operator long() const {
        return static_cast<long>(to_int());
    }

    explicit operator long long() const {
        return static_cast<long long>(to_int());
    }

    explicit operator unsigned short() const {
        return static_cast<unsigned short>(to_int());
    }

    explicit operator unsigned int() const {
        return static_cast<unsigned int>(to_int());
    }

    explicit operator unsigned long() const {
        return static_cast<unsigned long>(to_int());
    }

    explicit operator unsigned long long() const {
        return static_cast<unsigned long long>(to_int());
    }

private:
    void normalize() {
        if(!myi)
            mye = 0;
        // if exponent > 0, try to make it 0
        while(mye > 0 && (myi < 0 ? myi >= numeric_limits<int64_t>::min() / 10 : myi <= numeric_limits<int64_t>::max() / 10)) {
            myi *= 10;
            --mye;
        }
        // drop trailing 0
        while(mye && mye != numeric_limits<int32_t>::max() && !(myi % 10)) {
            myi /= 10;
            ++mye;
        }
    }
    
};

}}
#endif
