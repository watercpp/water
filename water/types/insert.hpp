// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_INSERT_HPP
#define WATER_TYPES_INSERT_HPP
#include <water/types/collection.hpp>
#include <water/types/ifel.hpp>
namespace water { namespace types {

/*

make a new collection with the elements of c_ and insert_ inserted before at_
- if c_ contains x y z, insert<c_, i, 1> contains x i y z

the collection_kind must have

   template<typename collection_, typename insert_, int at_> struct insert;

*/

template<typename c_, typename insert_, int at_ = 0>
struct insert :
    c_::collection_tag::result::template insert<c_, insert_, at_>
{};

namespace _ {

    template<typename, typename = void>
    struct do_insert_exists :
        false_result
    {};
    
    template<typename a_>
    struct do_insert_exists<a_, to_void<typename a_::template insert<void, void, 0> >> :
        true_result
    {};
    
}

// result true if
// - c_ is a collection, and the kind has insert<c_, void, 0>
// - c_ is not a collection, and it has insert<c_, void, 0>
template<typename c_>
struct insert_exists :
    _::do_insert_exists<typename ifel<is_collection<c_>::result, collection_kind_of<c_>, type<c_> >::result>
{};

}}
#endif
