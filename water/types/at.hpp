// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_AT_HPP
#define WATER_TYPES_AT_HPP
#include <water/types/collection.hpp>
#include <water/types/ifel.hpp>
namespace water { namespace types {

/*

at: move around in a collection
- at<c_, 123> is a collection positioned at types::offset<c_>::result + 123
- at<at<c_, 3>, -2> is like at<c_, 1>
- if at<c_, 123> is inside the collection
  - types::is_result<at<c_, 123> > is true
  - at<c_, 123>::result is the element
- if at<c_, 123> is outside the collection
  - types::is_result<at<c_, 123> > is false

it must be possible to move outside the collection, and then move back inside it.
- types::at and types::is_result must work outside the collection

if c_ has at least 5 items, can contain types, and types::insert can be used with it, a and b in the
code below contain the same elements:

   typedef insert<c_, int> p;
   typedef at<insert<p, void, 7>, 7> a;
   typedef insert<at<p, 7>, void> b;
   typename at<a, -7>::result ai = 0;
   typename at<b, -7>::result bi = 0;

the collection_kind must have

   template<typename collection_, int at_> struct at;

*/

template<typename c_, int i_ = 0>
struct at :
    c_::collection_tag::result::template at<c_, i_>
{};

// is types::at<c_, at_::result>
template<typename c_, typename at_>
struct at_type :
    c_::collection_tag::result::template at<c_, at_::result>
{};

namespace _ {

    template<typename a_, typename = void>
    struct do_at_exists :
        false_result
    {};
    
    template<typename a_>
    struct do_at_exists<a_, to_void<typename a_::template at<void, 0> >> :
        true_result
    {};

}

// result true if
// - c_ is a collection, and the kind has at<c_, 0>
// - c_ is not a collection, and it has at<c_, 0> (for testing the collection_kind directly)
template<typename c_>
struct at_exists :
    _::do_at_exists<typename ifel<is_collection<c_>::result, collection_kind_of<c_>, type<c_> >::result>
{};

}}
#endif
