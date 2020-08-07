// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ITERATOR_HPP
#define WATER_ITERATOR_HPP
#include <water/water.hpp>
#include <water/types/types.hpp>
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
}
#endif
namespace water {

/*

Things to figure out what category a iterator has, what value_type, pointer, reference or difference_type it has, and if its const.

std::iterator_traits might work just as good as this.

*/

using std::input_iterator_tag;
using std::output_iterator_tag;
using std::forward_iterator_tag;
using std::bidirectional_iterator_tag;
using std::random_access_iterator_tag;

namespace _ {

    template<typename a_, typename = void>
    struct do_iterator_category :
        types::type_plain<void>
    {};

    template<typename a_>
    struct do_iterator_category<a_, types::to_void<typename a_::iterator_category>> :
        types::type_plain<typename a_::iterator_category>
    {};

    template<typename a_>
    struct do_iterator_category<a_*, void> :
        types::type_plain<random_access_iterator_tag>
    {};

    template<typename tag_, typename category_of_, typename = void>
    struct do_is_iterator_category :
        types::false_result
    {};

    template<typename tag_, typename category_of_>
    struct do_is_iterator_category<tag_, category_of_, types::to_void<decltype(types::make<tag_*&>() = types::make<category_of_*>())>> :
        types::true_result
    {};

}

// result the iterator category of iterator_ or void
template<typename iterator_>
struct iterator_category :
    _::do_iterator_category<typename types::no_const<iterator_>::result>
{};

template<typename iterator_>
struct is_random_access_iterator :
    _::do_is_iterator_category<random_access_iterator_tag, typename iterator_category<iterator_>::result>
{};

template<typename iterator_>
struct is_bidirectional_iterator :
    types::to_bool<types::ors<
        is_random_access_iterator<iterator_>,
        _::do_is_iterator_category<bidirectional_iterator_tag, typename iterator_category<iterator_>::result>
    >>
{};

template<typename iterator_>
struct is_forward_iterator :
    types::to_bool<types::ors<
        is_bidirectional_iterator<iterator_>,
        _::do_is_iterator_category<forward_iterator_tag, typename iterator_category<iterator_>::result>
    >>
{};

template<typename iterator_>
struct is_input_iterator :
    types::to_bool<types::ors<
        is_forward_iterator<iterator_>,
        _::do_is_iterator_category<input_iterator_tag, typename iterator_category<iterator_>::result>
    >>
{};

// result true if iterator_::iterator_category is outupt_iterator_tag.
// this does *not* result true for any writeable iterator, use iterator_is_const<iterator_>::result for that
template<typename iterator_>
struct is_output_iterator :
    _::do_is_iterator_category<output_iterator_tag, typename iterator_category<iterator_>::result>
{};

namespace _ {

    template<typename a_, typename = void>
    struct do_iterator_value_type_2 :
        types::type_plain<void>
    {};

    template<typename a_>
    struct do_iterator_value_type_2<a_, types::to_void<decltype(*types::make<a_&>())>> :
        types::no_reference<types::no_const<decltype(*types::make<a_&>())>>
    {};



    template<typename a_, typename = void>
    struct do_iterator_value_type :
        do_iterator_value_type_2<a_>
    {};

    template<typename a_>
    struct do_iterator_value_type<a_, types::to_void<typename a_::value_type>> :
        types::no_const<typename a_::value_type>
    {};

    template<typename a_>
    struct do_iterator_value_type<a_*, void> :
        types::no_const<a_>
    {};



    template<typename a_, typename = void>
    struct do_iterator_difference_type_2 :
        types::type_plain<void>
    {};

    template<typename a_>
    struct do_iterator_difference_type_2<a_, types::to_void<decltype(types::make<a_&>() - types::make<a_&>())>> :
        types::type_plain<decltype(types::make<a_&>() - types::make<a_&>())>
    {};



    template<typename a_, typename = void>
    struct do_iterator_difference_type :
        do_iterator_difference_type_2<a_>
    {};

    template<typename a_>
    struct do_iterator_difference_type<a_, types::to_void<typename a_::difference_type>> :
        types::type_plain<typename a_::difference_type>
    {};

    template<typename a_>
    struct do_iterator_difference_type<a_*, void> :
        types::type_plain<ptrdiff_t>
    {};



    template<typename a_, typename = void>
    struct do_iterator_pointer_2 :
        types::type_plain<void>
    {};

    template<typename a_>
    struct do_iterator_pointer_2<a_, types::to_void<decltype(types::make<a_&>().operator->())>> :
        types::type_plain<decltype(types::make<a_&>().operator->())>
    {};



    template<typename a_, typename = void>
    struct do_iterator_pointer :
        do_iterator_pointer_2<a_>
    {};

    template<typename a_>
    struct do_iterator_pointer<a_, types::to_void<typename a_::pointer>> :
        types::type_plain<typename a_::pointer>
    {};

    template<typename a_>
    struct do_iterator_pointer<a_*, void> :
        types::type_plain<a_*>
    {};



    template<typename a_, typename = void>
    struct do_iterator_reference_2 :
        types::type_plain<void>
    {};

    template<typename a_>
    struct do_iterator_reference_2<a_, types::to_void<decltype(*types::make<a_&>())>> :
        types::type_plain<decltype(*types::make<a_&>())>
    {};



    template<typename a_, typename = void>
    struct do_iterator_reference :
        do_iterator_reference_2<a_>
    {};

    template<typename a_>
    struct do_iterator_reference<a_, types::to_void<typename a_::reference>> :
        types::type_plain<typename a_::reference>
    {};

    template<typename a_>
    struct do_iterator_reference<a_*, void> :
        types::type_plain<a_&>
    {};



    template<typename a_, typename = void>
    struct do_iterator_is_const :
        types::true_result
    {};

    template<typename a_>
    struct do_iterator_is_const<a_, types::to_void<decltype(*types::make<a_&>() = types::make<typename do_iterator_value_type<a_>::result&&>())>> :
        types::false_result
    {};

}

// result
// - iterator_::value_type if it exists, without const
// - if iterator_ is a pointer, what it points to without const
// - decltype(*iterator_{}) without const/reference
// - void
template<typename iterator_>
struct iterator_value_type :
    _::do_iterator_value_type<typename types::no_reference<types::no_const<iterator_>>::result>
{};

// result
// - iterator_::pointer if it exists
// - if iterator_ is a pointer, iterator_
// - decltype(iterator_{}->operator->())
// - void
template<typename iterator_>
struct iterator_pointer_type :
    _::do_iterator_pointer<typename types::no_reference<types::no_const<iterator_>>::result>
{};

// result
// - iterator_::reference if it exists
// - if iterator_ is a pointer, reference of what it points to
// - decltype(*iterator_{})
// - void
template<typename iterator_>
struct iterator_reference_type :
    _::do_iterator_reference<typename types::no_reference<types::no_const<iterator_>>::result>
{};

// result
// - iterator_::difference_type if it exists
// - if iterator_ is a pointer, ptrdiff_t
// - decltype(iterator_{} - iterator_{})
// - void
//
// std says only output iterators can have difference_type = void, even though only random accesss uses the type
template<typename iterator_>
struct iterator_difference_type :
    _::do_iterator_difference_type<typename types::no_reference<types::no_const<iterator_>>::result>
{};

// result true if you cannot write to the iterator_
template<typename iterator_>
struct iterator_is_const :
    _::do_iterator_is_const<typename types::no_reference<types::no_const<iterator_>>::result>
{};

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
