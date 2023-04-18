// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_DOWNGRADE_ITERATORS_BIDIRECTIONAL_HPP
#define WATER_DOWNGRADE_ITERATORS_BIDIRECTIONAL_HPP
#include <water/downgrade_iterators/bits.hpp>
namespace water { namespace downgrade_iterators {

template<typename iterator_, bool make_const_ = false, bool proxy_ = false>
class bidirectional
{
    static_assert(is_bidirectional_iterator<iterator_>, "must be at least bidirectional");
    using select_ = select_reference_pointer<iterator_, make_const_, proxy_>;

public:
    using iterator_category = bidirectional_iterator_tag;
    using value_type = iterator_value_type<iterator_>;
    using reference = typename select_::reference_type;
    using pointer = typename select_::pointer_type;
    using difference_type = ptrdiff_t;

private:
    iterator_ my {};
    ptrdiff_t myat = 0;
    bool myinitialized = false;

public:
    bidirectional() = default;

    explicit bidirectional(iterator_ a) :
        my{a},
        myinitialized{true}
    {}

    reference operator*() const {
        ___water_assert(myinitialized);
        return select_::reference(my);
    }

    pointer operator->() const {
        ___water_assert(myinitialized);
        return select_::pointer(my);
    }

    bidirectional& operator++() {
        ___water_assert(myinitialized);
        ++my;
        ++myat;
        return *this;
    }

    bidirectional operator++(int) {
        bidirectional r{*this};
        operator++();
        return r;
    }

    bidirectional& operator--() {
        ___water_assert(myinitialized);
        --my;
        --myat;
        return *this;
    }

    bidirectional operator--(int) {
        bidirectional r{*this};
        operator--();
        return r;
    }

    bool operator==(bidirectional const& a) const {
        ___water_assert(myinitialized && a.myinitialized);
        return my == a.my;
    }

    bool operator!=(bidirectional const& a) const {
        ___water_assert(myinitialized && a.myinitialized);
        return my != a.my;
    }

    friend iterator_ underlying(bidirectional const& a) {
        return a.my;
    }

    friend ptrdiff_t at(bidirectional const& a) {
        return a.myat;
    }

    friend bool initialized(bidirectional const& a) {
        return a.myinitialized;
    }
};

template<typename iterator_> using bidirectional_const = bidirectional<iterator_, true, false>;
template<typename iterator_> using bidirectional_proxied = bidirectional<iterator_, false, true>;
template<typename iterator_> using bidirectional_proxied_const = bidirectional<iterator_, true, true>;

template<typename iterator_>
bidirectional<iterator_> bidirectional_from(iterator_ a) {
    return bidirectional<iterator_>{a};
}

template<typename iterator_>
bidirectional_const<iterator_> bidirectional_const_from(iterator_ a) {
    return bidirectional_const<iterator_>{a};
}

template<typename iterator_>
bidirectional_proxied<iterator_> bidirectional_proxied_from(iterator_ a) {
    return bidirectional_proxied<iterator_>{a};
}

template<typename iterator_>
bidirectional_proxied_const<iterator_> bidirectional_proxied_const_from(iterator_ a) {
    return bidirectional_proxied_const<iterator_>{a};
}

}}
#endif
