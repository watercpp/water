// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES1_SET_HPP
#define WATER_TYPES1_SET_HPP
#include <water/types1/collection.hpp>
#include <water/types1/ifel.hpp>
#include <water/types1/to_void.hpp>
namespace water { namespace types1 {

/*

make a new collection with the elements of c_, except the element at at_ is set to set_
- if c_ contains x y z, set<c_, s, 1> contains x s z

the collection_kind must have

   template<typename collection_, typename set_, int at_> struct set;

*/

template<typename c_, typename set_, int at_ = 0>
struct set :
    c_::collection_tag::result::template set<c_, set_, at_>
{};

namespace _ {

    template<typename, typename = void>
    struct do_set_exists :
        false_result
    {};
    
    template<typename a_>
    struct do_set_exists<a_, to_void<typename a_::template set<void, void, 0> >> :
        true_result
    {};
    
}

// result true if
// - c_ is a collection, and the kind has set<c_, void, 0>
// - c_ is not a collection, and it has set<c_, void, 0>
template<typename c_>
struct set_exists :
    _::do_set_exists<typename ifel<is_collection<c_>::result, collection_kind_of<c_>, type<c_> >::result>
{};

}}
#endif
