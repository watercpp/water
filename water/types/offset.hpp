// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_OFFSET_HPP
#define WATER_TYPES_OFFSET_HPP
#include <water/types/collection.hpp>
#include <water/types/ifel.hpp>
namespace water { namespace types {

/*

result the current offset in the collection

the collection_kind must have

   template<typename collection_> struct offset;

*/

template<typename c_>
struct offset :
    c_::collection_tag::result::template offset<c_>
{};

namespace _ {

    template<typename a_, typename = void>
    struct do_offset_exists :
        false_result
    {};
    
    template<typename a_>
    struct do_offset_exists<a_, to_void<typename a_::template offset<void> >> :
        true_result
    {};
    
}

// result true if
// - c_ is a collection, and the kind has offset<c_>
// - c_ is not a collection, and it has offset<c_>
template<typename c_>
struct offset_exists :
    _::do_offset_exists<typename ifel<is_collection<c_>::result, collection_kind_of<c_>, type<c_> >::result>
{};

}}
#endif
