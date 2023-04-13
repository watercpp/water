// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ITERATOR_HPP
#define WATER_ITERATOR_HPP
#include <water/water.hpp>
#include <water/types.hpp>
#include <water/is_no_to.hpp>
#ifndef WATER_NO_STD
    #include <iterator>
#else
    namespace std {
        // this is all that is needed here, but do this only when WATER_NO_STD is defined because in
        // the real std library these structs can be declared in a different namespace
        struct input_iterator_tag;
        struct output_iterator_tag;
        struct forward_iterator_tag; // inherits input_iterator_tag
        struct bidirectional_iterator_tag; // inherits forward_iterator_tag
        struct random_access_iterator_tag; // inherits bidirectional_iterator_tag
        struct contiguous_iterator_tag; // inherits random_access_iterator_tag
}
#endif
namespace water {


/*

Things to figure out what category a iterator has, what value_type, pointer, reference or difference_type it has, and if its const.

This should recognize contiguous_iterator_tag, but water::iterator_category<int*> is random_access_iterator_tag.

When WATER_NO_STD is *not* defined, this will use std::iterator_traits<iterator_> to get the types from the iterator

*/


using std::input_iterator_tag;
using std::output_iterator_tag;
using std::forward_iterator_tag;
using std::bidirectional_iterator_tag;
using std::random_access_iterator_tag;

#ifndef WATER_NO_STD

struct not_contiguous_iterator_tag;

using contiguous_iterator_tag_or_not = ifel<
    equal<random_access_iterator_tag, std::iterator_traits<int*>::iterator_category>,
    not_contiguous_iterator_tag,
    std::iterator_traits<int*>::iterator_category
>;

template<typename iterator_>
using iterator_or_traits = std::iterator_traits<no_const_or_reference<iterator_>>;

#else

using contiguous_iterator_tag_or_not = std::contiguous_iterator_tag;

template<typename iterator_>
using iterator_or_traits = no_const_or_reference<iterator_>;

#endif



namespace _ {

    template<typename a_, bool = is_pointer<a_>, typename = void>
    struct do_iterator_category {
        using result = void;
    };

    template<typename a_>
    struct do_iterator_category<a_, false, to_void<typename iterator_or_traits<a_>::iterator_category>> {
        using category = typename iterator_or_traits<a_>::iterator_category;
        using result = ifel<
            equal<category, contiguous_iterator_tag_or_not>,
            random_access_iterator_tag,
            category
        >;
    };

    template<typename a_>
    struct do_iterator_category<a_*, true, void> {
        using result = random_access_iterator_tag;
    };

    template<typename tag_, typename category_of_, typename = void>
    bool constexpr do_is_iterator_category = false;

    template<typename tag_, typename category_of_>
    bool constexpr do_is_iterator_category<
        tag_,
        category_of_,
        to_void<decltype(make_type<tag_*&>() = make_type<category_of_*>())>
    > = true;

}


// the iterator category of iterator_ or void
// note that this will be random_access_iterator_tag even if the the iterator_ has contiguous_iterator_tag
template<typename iterator_>
using iterator_category = typename _::do_iterator_category<no_const_or_reference<iterator_>>::result;


template<typename iterator_>
bool constexpr is_random_access_iterator =
    _::do_is_iterator_category<random_access_iterator_tag, iterator_category<iterator_>>;


template<typename iterator_>
bool constexpr is_bidirectional_iterator =
    is_random_access_iterator<iterator_> ||
    _::do_is_iterator_category<bidirectional_iterator_tag, iterator_category<iterator_>>;


template<typename iterator_>
bool constexpr is_forward_iterator =
    is_bidirectional_iterator<iterator_> ||
    _::do_is_iterator_category<forward_iterator_tag, iterator_category<iterator_>>;


template<typename iterator_>
bool constexpr is_input_iterator =
    is_forward_iterator<iterator_> ||
    _::do_is_iterator_category<input_iterator_tag, iterator_category<iterator_>>;


// true if iterator_::iterator_category is outupt_iterator_tag.
// this is *not* true for any writeable iterator, use iterator_is_const<iterator_> for that
template<typename iterator_>
bool constexpr is_output_iterator =
    _::do_is_iterator_category<output_iterator_tag, iterator_category<iterator_>>;




namespace _ {

    template<typename a_, typename = void>
    struct do_iterator_value_type_2 {
        using result = void;
    };

    template<typename a_>
    struct do_iterator_value_type_2<a_, to_void<decltype(*make_type<a_&>())>> {
        using result = no_const_or_reference<decltype(*make_type<a_&>())>;
    };



    template<typename a_, bool = is_pointer<a_>, typename = void>
    struct do_iterator_value_type :
        do_iterator_value_type_2<a_>
    {};

    template<typename a_>
    struct do_iterator_value_type<a_, false, to_void<if_not_void<typename iterator_or_traits<a_>::value_type>>> {
        using result = no_const<typename iterator_or_traits<a_>::value_type>;
    };

    template<typename a_>
    struct do_iterator_value_type<a_*, true, void> {
        using result = no_const<a_>;
    };



    template<typename a_, typename = void>
    struct do_iterator_difference_type_2 {
        using result = void;
    };
    
    // plain int will work with int - int, but not *int
    template<typename a_>
    struct do_iterator_difference_type_2<a_, to_void<decltype(make_type<a_&>() - make_type<a_&>()), decltype(*make_type<a_&>())>> {
        using result = decltype(make_type<a_&>() - make_type<a_&>());
    };



    template<typename a_, bool = is_pointer<a_>, typename = void>
    struct do_iterator_difference_type :
        do_iterator_difference_type_2<a_>
    {};

    template<typename a_>
    struct do_iterator_difference_type<a_, false, to_void<if_not_void<typename iterator_or_traits<a_>::difference_type>>> {
        using result = typename iterator_or_traits<a_>::difference_type;
    };

    template<typename a_>
    struct do_iterator_difference_type<a_*, true, void> {
        using result = ptrdiff_t;
    };



    template<typename a_, typename = void>
    struct do_iterator_pointer_2 {
        using result = void;
    };

    template<typename a_>
    struct do_iterator_pointer_2<a_, to_void<decltype(make_type<a_&>().operator->())>> {
        using result = decltype(make_type<a_&>().operator->());
    };



    template<typename a_, bool = is_pointer<a_>, typename = void>
    struct do_iterator_pointer :
        do_iterator_pointer_2<a_>
    {};

    template<typename a_>
    struct do_iterator_pointer<a_, false, to_void<if_not_void<typename iterator_or_traits<a_>::pointer>>> {
        using result = typename iterator_or_traits<a_>::pointer;
    };

    template<typename a_>
    struct do_iterator_pointer<a_*, true, void> {
        using result = a_*;
    };


    
    template<typename a_, typename = void>
    struct do_iterator_reference_2 {
        using result = void;
    };

    // output iterators can have a reference that only allows = value_type, let that be void
    template<typename a_>
    struct do_iterator_reference_2<a_, to_void<decltype(static_cast<typename do_iterator_value_type<a_>::result const&>(*make_type<a_&>()))>> {
        using result = decltype(*make_type<a_&>());
    };



    template<typename a_, bool = is_pointer<a_>, typename = void>
    struct do_iterator_reference :
        do_iterator_reference_2<a_>
    {};

    template<typename a_>
    struct do_iterator_reference<a_, false, to_void<if_not_void<typename iterator_or_traits<a_>::reference>>> {
        using result = typename iterator_or_traits<a_>::reference;
    };

    template<typename a_>
    struct do_iterator_reference<a_*, true, void> {
        using result = a_&;
    };



    template<typename a_, typename = void>
    bool constexpr do_iterator_is_const = true;

    template<typename a_>
    bool constexpr do_iterator_is_const<
        a_,
        to_void<decltype(*make_type<a_&>() = make_type<typename do_iterator_value_type<a_>::result&&>())>
    > = false;

}


// result
// - std::iterator_traits<iterator_>::value_type if it exists and is not void, without const
// - if iterator_ is a pointer, what it points to without const
// - decltype(*iterator_{}) without const/reference
// - void
template<typename iterator_>
using iterator_value_type = typename _::do_iterator_value_type<no_const_or_reference<iterator_>>::result;


// result
// - std::iterator_traits<iterator_>::pointer if it exists and is not void
// - if iterator_ is a pointer, iterator_
// - decltype(iterator_{}->operator->())
// - void
template<typename iterator_>
using iterator_pointer_type = typename _::do_iterator_pointer<no_const_or_reference<iterator_>>::result;


// result
// - std::iterator_traits<iterator_>::reference if it exists and is not void
// - if iterator_ is a pointer, reference of what it points to
// - decltype(*iterator_{})
// - void
template<typename iterator_>
using iterator_reference_type = typename _::do_iterator_reference<no_const_or_reference<iterator_>>::result;


// result
// - std::iterator_traits<iterator_>::difference_type if it exists and is not void
// - if iterator_ is a pointer, ptrdiff_t
// - decltype(iterator_{} - iterator_{})
// - void
template<typename iterator_>
using iterator_difference_type = typename _::do_iterator_difference_type<no_const_or_reference<iterator_>>::result;



// true if you cannot write to the iterator_
template<typename iterator_>
bool constexpr iterator_is_const = _::do_iterator_is_const<no_const_or_reference<iterator_>>;



template<typename type_>
type_* iterator_pointer(type_ *a) {
    return a;
}

template<typename iterator_>
auto iterator_pointer(iterator_&& a) -> decltype(a.operator->()) {
    return a.operator->();
}



}
#endif
