// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_DOWNGRADE_ITERATORS_FORWARD_HPP
#define WATER_DOWNGRADE_ITERATORS_FORWARD_HPP
#include <water/downgrade_iterators/bits.hpp>
namespace water { namespace downgrade_iterators {

template<typename iterator_, bool make_const_ = false, bool proxy_ = false>
class forward
{
    static_assert(types::type_assert<is_forward_iterator<iterator_>>::assert, "must be at least forward iterator");
    using select_ = select_reference_pointer<iterator_, make_const_, proxy_>;

public:
    using iterator_category = forward_iterator_tag;
    using value_type = typename iterator_value_type<iterator_>::result;
    using reference = typename select_::reference_type;
    using pointer = typename select_::pointer_type;
    using difference_type = ptrdiff_t;

private:
    iterator_ my {};
    ptrdiff_t myat = 0;
    bool myinitialized = false;

public:
    forward() = default;

    explicit forward(iterator_ const& a) :
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

    forward& operator++() {
        ___water_assert(myinitialized);
        ++my;
        ++myat;
        return *this;
    }

    forward operator++(int) {
        forward r{*this};
        operator++();
        return r;
    }

    bool operator==(forward const& a) const {
        ___water_assert(myinitialized && a.myinitialized);
        return my == a.my;
    }

    bool operator!=(forward const& a) const {
        ___water_assert(myinitialized && a.myinitialized);
        return my != a.my;
    }

    friend iterator_ underlying(forward const& a) {
        return a.my;
    }

    friend ptrdiff_t at(forward const& a) {
        return a.myat;
    }

    friend bool initialized(forward const& a) {
        return a.myinitialized;
    }
};

template<typename iterator_> using forward_const = forward<iterator_, true, false>;
template<typename iterator_> using forward_proxied = forward<iterator_, false, true>;
template<typename iterator_> using forward_proxied_const = forward<iterator_, true, true>;

template<typename iterator_>
forward<iterator_> forward_from(iterator_ const& a) {
    return forward<iterator_>{a};
}

template<typename iterator_>
forward_const<iterator_> forward_const_from(iterator_ const& a) {
    return forward_const<iterator_>{a};
}

template<typename iterator_>
forward_proxied<iterator_> forward_proxied_from(iterator_ const& a) {
    return forward_proxied<iterator_>{a};
}

template<typename iterator_>
forward_proxied_const<iterator_> forward_proxied_const_from(iterator_ const& a) {
    return forward_proxied_const<iterator_>{a};
}

}}
#endif
