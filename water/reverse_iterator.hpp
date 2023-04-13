// Copyright 2017-2023 Johan Paulsson
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
    struct reverse_iterator_converts_to_bool {
        static bool constexpr result = false;
    };
    
    template<typename a_>
    struct reverse_iterator_converts_to_bool<a_, to_void<decltype(static_cast<bool>(make_type<a_ const&>()))>> {
        static bool constexpr result = true;
    };
    
}

template<typename iterator_>
class reverse_iterator
{
    template<typename> struct nothing {};
    
    using bool_ = ifel<_::reverse_iterator_converts_to_bool<iterator_>::result, bool, nothing<bool>>;

public:
    using iterator_category = water::iterator_category<iterator_>;
    using value_type = iterator_value_type<iterator_>;
    using pointer = iterator_pointer_type<iterator_>;
    using reference = iterator_reference_type<iterator_>;
    using difference_type = iterator_difference_type<iterator_>;

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
    reverse_iterator(reverse_iterator<other_> a, nothing<decltype(iterator_(my))> = {}) : // iterator_( not { because visual c++ 2015
        my{a.underlying()}
    {}

    auto operator*() const -> decltype(*my) {
        return *my;
    }

    auto operator->() const -> decltype(iterator_pointer(my)) {
        return iterator_pointer(my);
    }

    auto operator[](ifel<is_random_access_iterator<iterator_>, difference_type, nothing<void>> a) const -> decltype(my[a]) {
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
    ifel<is_random_access_iterator<iterator_>, iterator_difference_type<iterator_>> b
) {
    a.underlying() -= b;
    return a;
}

template<typename iterator_>
reverse_iterator<iterator_>& operator-=(
    reverse_iterator<iterator_>& a,
    ifel<is_random_access_iterator<iterator_>, iterator_difference_type<iterator_>> b
) {
    a.underlying() += b;
    return a;
}

template<typename iterator_>
reverse_iterator<iterator_> operator+(
    reverse_iterator<iterator_> a,
    ifel<is_random_access_iterator<iterator_>, iterator_difference_type<iterator_>> b
) {
    return a += b;
}

template<typename iterator_>
reverse_iterator<iterator_> operator+(
    ifel<is_random_access_iterator<iterator_>, iterator_difference_type<iterator_>> a,
    reverse_iterator<iterator_> b
) {
    return b += a;
}

template<typename iterator_>
reverse_iterator<iterator_> operator-(
    reverse_iterator<iterator_> a,
    ifel<is_random_access_iterator<iterator_>, iterator_difference_type<iterator_>> b
) {
    return a -= b;
}

template<typename iterator_>
ifel<is_random_access_iterator<iterator_>, iterator_difference_type<iterator_>> operator-(
    reverse_iterator<iterator_> a,
    reverse_iterator<iterator_> b
) {
    return b.underlying() - a.underlying();
}

template<typename iterator_>
bool operator<(
    reverse_iterator<iterator_> const& a,
    ifel<is_random_access_iterator<iterator_>, reverse_iterator<iterator_>> const& b
) {
    return b.underlying() < a.underlying();
}

template<typename iterator_>
bool operator>(
    reverse_iterator<iterator_> const& a,
    ifel<is_random_access_iterator<iterator_>, reverse_iterator<iterator_>> const& b
) {
    return b.underlying() > a.underlying();
}

template<typename iterator_>
bool operator<=(
    reverse_iterator<iterator_> const& a,
    ifel<is_random_access_iterator<iterator_>, reverse_iterator<iterator_>> const& b
) {
    return b.underlying() <= a.underlying();
}

template<typename iterator_>
bool operator>=(
    reverse_iterator<iterator_> const& a,
    ifel<is_random_access_iterator<iterator_>, reverse_iterator<iterator_>> const& b
) {
    return b.underlying() >= a.underlying();
}

}
#endif
