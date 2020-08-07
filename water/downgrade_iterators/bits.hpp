// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_DOWNGRADE_ITERATORS_BITS_HPP
#define WATER_DOWNGRADE_ITERATORS_BITS_HPP
#include <water/iterator.hpp>
#include <water/types/types.hpp>
namespace water { namespace downgrade_iterators {

template<typename iterator_>
class reference
{
    using reference_ = typename types::type_plain<decltype(*types::make<iterator_ const&>())>::result; // type_plain because visual c++
    iterator_ my;

public:
    reference(iterator_ a) :
        my{a}
    {}

    operator reference_() const {
        return *my;
    }

    reference& operator=(typename iterator_value_type<iterator_>::result const& a) {
        *my = a;
        return *this;
    }
};

template<typename iterator_>
class reference_const
{
    using value_ = typename iterator_value_type<iterator_>::result;
    iterator_ my;

public:
    reference_const(iterator_ a) :
        my{a}
    {}

    operator value_ const& () const {
        return *my;
    }
};

template<typename iterator_>
class pointer
{
    using pointer_ = typename types::type_plain<decltype(iterator_pointer(types::make<iterator_ const&>()))>::result; // type_plain because visual c++
    iterator_ my;

public:
    pointer(iterator_ a) :
        my{a}
    {}

    pointer_ operator->() const {
        return iterator_pointer(my);
    }

    operator pointer_() const {
        return iterator_pointer(my);
    }
};

template<typename iterator_>
class pointer_const
{
    using value_ = typename iterator_value_type<iterator_>::result;
    iterator_ my;

public:
    pointer_const(iterator_ a) :
        my{a}
    {}

    value_ const* operator->() const {
        return iterator_pointer(my);
    }

    operator value_ const*() const {
        return iterator_pointer(my);
    }
};

template<typename iterator_, bool make_const_, bool proxy_>
struct select_reference_pointer
{
    using pointer_type = typename types::ifel<
        make_const_ || iterator_is_const<iterator_>::result,
        pointer_const<iterator_>,
        downgrade_iterators::pointer<iterator_>
    >::result;
    
    using reference_type = typename types::ifel<
        make_const_ || iterator_is_const<iterator_>::result,
        reference_const<iterator_>,
        downgrade_iterators::reference<iterator_>
    >::result;
    
    static pointer_type pointer(iterator_ const& a) { return pointer_type{a}; }
    static reference_type reference(iterator_ const& a) { return reference_type{a}; }
};

template<typename iterator_, bool make_const_>
struct select_reference_pointer<iterator_, make_const_, false>
{
    using value_type = typename iterator_value_type<iterator_>::result;
    using pointer_type = typename types::ifel<make_const_, value_type const*, iterator_pointer_type<iterator_>>::result;
    using reference_type = typename types::ifel<make_const_, value_type const&, iterator_reference_type<iterator_>>::result;
    
    static pointer_type pointer(iterator_ const& a) { return iterator_pointer(a); }
    static reference_type reference(iterator_ const& a) { return *a; }
};

}}
#endif
