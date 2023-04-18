// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_DOWNGRADE_ITERATORS_INPUT_HPP
#define WATER_DOWNGRADE_ITERATORS_INPUT_HPP
#include <water/downgrade_iterators/bits.hpp>
namespace water { namespace downgrade_iterators {

template<typename iterator_, bool proxy_ = false>
class input
{
    static_assert(is_forward_iterator<iterator_>, "must be at least forward iterator");
    using select_ = select_reference_pointer<iterator_, true, proxy_>;

public:
    using iterator_category = input_iterator_tag;
    using value_type = iterator_value_type<iterator_>;
    using reference = typename select_::reference_type;
    using pointer = typename select_::pointer_type;
    using difference_type = ptrdiff_t;

private:
    mutable iterator_ my {};
    mutable ptrdiff_t myat = 0;
    mutable bool myinitialized = false;
    mutable bool myread = false;

public:
    input() = default;

    explicit input(iterator_ a) :
        my{a},
        myinitialized{true}
    {}

    reference operator*() const {
        ___water_assert(myinitialized);
        if(myread) {
            ++my;
            ++myat;
        }
        myread = true;
        return select_::reference(my);
    }

    pointer operator->() const {
        ___water_assert(myinitialized);
        if(myread) {
            ++my;
            ++myat;
        }
        myread = true;
        return select_::pointer(my);
    }

    input& operator++() {
        ___water_assert(myinitialized);
        myread = false;
        ++my;
        ++myat;
        return *this;
    }

    input operator++(int) {
        ___water_assert(myinitialized);
        input r{*this};
        operator++();
        return r;
    }

    bool operator==(input const& a) const {
        ___water_assert(myinitialized);
        return my == a.my;
    }

    bool operator!=(input const& a) const {
        ___water_assert(myinitialized);
        return my != a.my;
    }

    friend iterator_ underlying(input const& a) {
        return a.my;
    }

    friend ptrdiff_t at(input const& a) {
        return a.myat;
    }

    friend bool initialized(input const& a) {
        return a.myinitialized;
    }
};

template<typename iterator_> using input_proxied = input<iterator_, true>;

template<typename iterator_>
input<iterator_> input_from(iterator_ a) {
    return input<iterator_>{a};
}

template<typename iterator_>
input_proxied<iterator_> input_proxied_from(iterator_ a) {
    return input_proxied<iterator_>{a};
}

}}
#endif
