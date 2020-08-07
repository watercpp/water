// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_REVERSE_ITERATOR_HPP
#define WATER_REVERSE_ITERATOR_HPP
#include <water/iterator.hpp>
namespace water {

/*

Reverse a bidirectional or random access iterator.

Construct rbegin() with end() - 1, and rend() with begin() - 1.
Unlike std::reverse_iterator this does not store iterator + 1.

*/

namespace _ {

    template<typename a_, typename = void>
    struct reverse_iterator_converts_to_bool :
        types::false_result
        {};
    
    template<typename a_>
    struct reverse_iterator_converts_to_bool<a_, types::to_void<decltype(static_cast<bool>(types::make<a_ const&>()))>> :
        types::true_result
        {};
    
}

template<typename iterator_>
class reverse_iterator
{
    template<typename>
    struct nothing {};
    using bool_ = typename types::ifel_type<_::reverse_iterator_converts_to_bool<iterator_>, bool, nothing<bool>>::result;

public:
    using iterator_category = typename iterator_category<iterator_>::result;
    using value_type = typename iterator_value_type<iterator_>::result;
    using pointer = typename iterator_pointer_type<iterator_>::result;
    using reference = typename iterator_reference_type<iterator_>::result;
    using difference_type = typename iterator_difference_type<iterator_>::result;

private:
    iterator_ my {};

public:
    reverse_iterator() = default;
    reverse_iterator(reverse_iterator const&) = default;
    reverse_iterator& operator=(reverse_iterator const&) = default;

    explicit reverse_iterator(iterator_ a) :
        my{a}
    {}

    template<typename other_>
    reverse_iterator(reverse_iterator<other_> a, nothing<decltype(iterator_(a.underlying()))> = {}) : // iterator_( not { because visual c++ 2015
        my{a.underlying()}
    {}

    auto operator*() const -> decltype(*my) {
        return *my;
    }

    auto operator->() const -> decltype(iterator_pointer(my)) {
        return iterator_pointer(my);
    }

    auto operator[](typename types::ifel_type<is_random_access_iterator<iterator_>, difference_type, nothing<void>>::result a) const -> decltype(my[a]) {
        return my[-a];
    }

    explicit operator bool_() const {
        return static_cast<bool_>(my);
    }

    reverse_iterator& operator++() {
        --my;
        return *this;
    }

    reverse_iterator& operator--() {
        ++my;
        return *this;
    }

    reverse_iterator operator++(int) {
        reverse_iterator r(*this);
        --my;
        return r;
    }

    reverse_iterator operator--(int) {
        reverse_iterator r(*this);
        ++my;
        return r;
    }

    bool operator==(reverse_iterator const& a) const {
        return my == a.my;
    }

    bool operator!=(reverse_iterator const& a) const {
        return my != a.my;
    }

    iterator_& underlying() {
        return my;
    }

    iterator_ const& underlying() const {
        return my;
    }
};

template<typename iterator_>
reverse_iterator<iterator_> reverse_iterator_from(iterator_ a) {
    return reverse_iterator<iterator_>(a);
}

template<typename iterator_>
reverse_iterator<iterator_>& operator+=(
    reverse_iterator<iterator_>& a,
    typename types::ifel_type<is_random_access_iterator<iterator_>, iterator_difference_type<iterator_>>::result b
) {
    a.underlying() -= b;
    return a;
}

template<typename iterator_>
reverse_iterator<iterator_>& operator-=(
    reverse_iterator<iterator_>& a,
    typename types::ifel_type<is_random_access_iterator<iterator_>, iterator_difference_type<iterator_>>::result b
) {
    a.underlying() += b;
    return a;
}

template<typename iterator_>
reverse_iterator<iterator_> operator+(
    reverse_iterator<iterator_> a,
    typename types::ifel_type<is_random_access_iterator<iterator_>, iterator_difference_type<iterator_>>::result b
) {
    return a += b;
}

template<typename iterator_>
reverse_iterator<iterator_> operator+(
    typename types::ifel_type<is_random_access_iterator<iterator_>, iterator_difference_type<iterator_>>::result a,
    reverse_iterator<iterator_> b
) {
    return b += a;
}

template<typename iterator_>
reverse_iterator<iterator_> operator-(
    reverse_iterator<iterator_> a,
    typename types::ifel_type<is_random_access_iterator<iterator_>, iterator_difference_type<iterator_>>::result b
) {
    return a -= b;
}

template<typename iterator_>
typename types::ifel_type<is_random_access_iterator<iterator_>, iterator_difference_type<iterator_>>::result operator-(
    reverse_iterator<iterator_> a,
    reverse_iterator<iterator_> b
) {
    return b.underlying() - a.underlying();
}

template<typename iterator_>
bool operator<(
    reverse_iterator<iterator_> const& a,
    typename types::ifel_type<is_random_access_iterator<iterator_>, reverse_iterator<iterator_>>::result const& b
) {
    return b.underlying() < a.underlying();
}

template<typename iterator_>
bool operator>(
    reverse_iterator<iterator_> const& a,
    typename types::ifel_type<is_random_access_iterator<iterator_>, reverse_iterator<iterator_>>::result const& b
) {
    return b.underlying() > a.underlying();
}

template<typename iterator_>
bool operator<=(
    reverse_iterator<iterator_> const& a,
    typename types::ifel_type<is_random_access_iterator<iterator_>, reverse_iterator<iterator_>>::result const& b
) {
    return b.underlying() <= a.underlying();
}

template<typename iterator_>
bool operator>=(
    reverse_iterator<iterator_> const& a,
    typename types::ifel_type<is_random_access_iterator<iterator_>, reverse_iterator<iterator_>>::result const& b
) {
    return b.underlying() >= a.underlying();
}

}
#endif
