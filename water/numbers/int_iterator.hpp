// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_INT_ITERATOR_HPP
#define WATER_NUMBERS_INT_ITERATOR_HPP
#include <water/numbers/bits.hpp>
namespace water { namespace numbers {

/*

iterate over int digits

leading_zeros makes this return that many 0 before the number

*/

template<typename int_>
class int_iterator
{
    using select_ = ifel<numeric_limits<int_>::is_signed, signed, unsigned>; // avoid warning compare -1 with unsigned, intel c++

public:
    using iterator_category = forward_iterator_tag;
    using value_type = unsigned;
    using pointer = value_type const*;
    using reference = value_type const&;
    using difference_type = ptrdiff_t;

private:
    int_
        my = 0,
        mydivide = 1,
        mybase = 10;
    unsigned
        mydigit = 0,
        myat = 0,
        my0 = 0;

public:
    int_iterator() = default;

    int_iterator(int_ a, int_ divide, unsigned base, unsigned leading_zeros = 0) :
        my(a),
        mydivide(divide ? divide : divide_1_or_minus_1(a, select_{})),
        mybase(static_cast<int_>(base >= 2 ? base : 10)),
        my0(leading_zeros)
    {
        ___water_assert(divide && 2 <= base && base <= 16 && (divide > 0 || my < 1));
        if(!my0) digit(select_{});
    }

    explicit int_iterator(unsigned at) : // use this to make end
        myat(at)
    {}

    reference operator*() const {
        return mydigit;
    }

    pointer operator->() const {
        return &mydigit;
    }

    int_iterator& operator++() {
        mydigit = 0;
        ++myat; // dont care if wrap around?
        if(my0 <= myat)
            digit(select_{});
        return *this;
    }

    int_iterator operator++(int) {
        int_iterator r(*this);
        operator++();
        return r;
    }

    bool operator==(int_iterator const& a) {
        return myat == a.myat;
    }

    bool operator!=(int_iterator const& a) {
        return myat != a.myat;
    }

private:

    void digit(signed) {
        if(mydivide) {
            if(mydivide == -1)
                mydigit = static_cast<unsigned>((my % mybase) * static_cast<int_>(-1)); // cannot divide 2s complement min with -1
            else
                mydigit = static_cast<unsigned>((my / mydivide) % mybase);
            mydivide /= mybase;
        }
    }

    void digit(unsigned) {
        if(mydivide) {
            mydigit = static_cast<unsigned>((my / mydivide) % mybase);
            mydivide /= mybase;
        }
    }

    static constexpr int_ divide_1_or_minus_1(int_ a, signed) {
        return a < static_cast<int_>(0) ? static_cast<int_>(-1) : static_cast<int_>(1);
    }
    
    static constexpr int_ divide_1_or_minus_1(int_, unsigned) {
        return static_cast<int_>(1);
    }
};

}}
#endif
