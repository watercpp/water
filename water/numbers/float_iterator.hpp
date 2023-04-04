// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_FLOAT_ITERATOR_HPP
#define WATER_NUMBERS_FLOAT_ITERATOR_HPP
#include <water/numbers/max_digits.hpp>
namespace water { namespace numbers {

/*

iterate over float digits

fmod the float with 10 or 2 before this if needed

leading_zeros makes this return that many 0 before the number
digits limits the number of digits, after that this retirns 0
round_up will round the last digit up

*/

template<typename float_>
class float_iterator
{
public:
    using iterator_category = forward_iterator_tag;
    using value_type = unsigned;
    using pointer = void;
    using reference = value_type;
    using difference_type = ptrdiff_t;

private:
    using uint_ = unsigned long long;

private:
    float_ my = 0;
    uint_ mydigits = 0;
    uint_ mydivide = 1;
    unsigned
        mybase = 10,
        mydigits_max = 0,
        myat = 0,
        my0 = 0,
        mylast = static_cast<unsigned>(-1);
    bool myround = false;

public:

    float_iterator() = default;

    float_iterator(float_ a, unsigned base) :
        my(a),
        mybase(base)
    {
        fill();
    }

    float_iterator(float_ a, unsigned base, unsigned leading_zeros, unsigned digits, bool round_last_digit_up) :
        my(a),
        mybase(base),
        my0(leading_zeros),
        mylast(digits && digits <= static_cast<unsigned>(-1) - leading_zeros ? leading_zeros + digits - 1 : static_cast<unsigned>(-1)),
        myround(round_last_digit_up)
    {
        if(!my0) fill();
    }

    explicit float_iterator(unsigned at) : // use this to make end
        myat(at)
    {}

    reference operator*() const {
        auto r = static_cast<unsigned>((mydigits / mydivide) % mybase);
        if(myround && myat == mylast)
            ++r;
        return r;
    }

    float_iterator& operator++() {
        ++myat; // does not really matter if it wraps around
        if(myat < my0)
            ;
        else if(myat > mylast) {
            mydigits = 0;
            mydivide = 1;
        }
        else if(mydivide == 1)
            fill();
        else
            mydivide /= mybase;
        return *this;
    }

    float_iterator operator++(int) {
        float_iterator r(*this);
        operator++();
        return r;
    }

    bool operator==(float_iterator const& a) {
        return myat == a.myat;
    }

    bool operator!=(float_iterator const& a) {
        return myat != a.myat;
    }

private:

    void fill() {
        bool first = mydigits_max == 0; // my is 1.23 the first time, then 0.123
        if(first)
            mydigits_max = max_digits<float_>(mybase);
        auto const divide_max = static_cast<uint_>(-1) / (mybase * mybase);
        mydivide = 1;
        unsigned d = mydigits_max;
        while(d-- && mydivide < divide_max)
            mydivide *= mybase;
        my *= mydivide;
        mydigits = static_cast<uint_>(my);
        my -= mydigits;
        if(!first)
            mydivide /= mybase;
    }
};

}}
#endif
