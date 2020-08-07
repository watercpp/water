// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_DOWNGRADE_ITERATORS_OUTPUT_HPP
#define WATER_DOWNGRADE_ITERATORS_OUTPUT_HPP
#include <water/downgrade_iterators/bits.hpp>
namespace water { namespace downgrade_iterators {

/*

output++ cound maybe return a proxy that converts to output const&

*/

template<typename iterator_>
class output_reference
{
    using value_ = typename iterator_value_type<iterator_>::result;
    iterator_ *my;

public:
    explicit output_reference(iterator_ *a) :
        my{&a}
    {}

    output_reference(output_reference&& a) :
        my{a.my}
    {
        a.my = 0;
    }

    void operator=(value_ const& a) {
        ___water_assert(my);
        **my = a;
        my = 0;
    }

    output_reference(output_reference const&) = delete;
    output_reference& operator=(output_reference const&) = delete;
    output_reference& operator=(output_reference&&) = delete;
};

template<typename iterator_>
class output
{
    static_assert(types::type_assert<is_forward_iterator<iterator_>>::assert, "must be at least forward");
    static_assert(types::type_assert<types::is_reference<iterator_reference_type<iterator_>>>::assert, "must be a real reference");

public:
    using iterator_category = output_iterator_tag;
    using value_type = typename iterator_value_type<iterator_>::result;
    using reference = void;
    using pointer = void;
    using difference_type = void;

private:
    mutable iterator_ my {};
    mutable ptrdiff_t myat = 0;
    mutable bool myinitialized = false;
    mutable bool mywrote = false;

public:
    output() = default;

    explicit output(iterator_ const& a) :
        my{a},
        myinitialized{true}
    {}

    output_reference<iterator_> operator*() const {
        ___water_assert(myinitialized);
        if(mywrote) {
            ++my;
            ++myat;
        }
        mywrote = true;
        return output_reference<iterator_>{&my};
    }

    output& operator++() {
        ___water_assert(myinitialized);
        mywrote = false;
        ++my;
        ++myat;
        return *this;
    }

    output operator++(int) {
        ___water_assert(myinitialized);
        output r{*this};
        operator++();
        return r;
    }

    friend iterator_ underlying(output const& a) {
        return a.my;
    }

    friend ptrdiff_t at(output const& a) {
        return a.myat;
    }

    friend bool initialized(output const& a) {
        return a.myinitialized;
    }
};

template<typename iterator_>
output<iterator_> output_from(iterator_ const& a) {
    return output<iterator_>{a};
}

}}
#endif
