// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_NEXT_HPP
#define WATER_TYPES_NEXT_HPP
#include <water/types/collection.hpp>
#include <water/types/ifel.hpp>
#include <water/types/to_void.hpp>
namespace water { namespace types {

/*

Move to the next position in a collection
 - next<c_> is the same position in the collection as types::at<c_, 1>
 - if next<c_> is inside the collection:
   - next<next<c_> > is valid
   - next<c_>::result is the element
   - types::is_result<next<c_> > is true
 - if next<c_> is outside the collection:
   - types::is_result<next<c_> > is false

the collection_kind must have

   template<typename collection_> struct next;

*/

template<typename c_>
struct next :
    c_::collection_tag::result::template next<c_>
{};

namespace _ {

    template<typename a_, typename = void>
    struct do_next_exists :
        false_result
    {};
    
    template<typename a_>
    struct do_next_exists<a_, to_void<typename a_::template next<void> >> :
        true_result
    {};

}
// result true if
// - c_ is a collection, and the kind has next<c_>
// - c_ is not a collection, and it has next<c_>
template<typename c_>
struct next_exists :
    _::do_next_exists<typename ifel<is_collection<c_>::result, collection_kind_of<c_>, type<c_> >::result>
{};

}}
#endif
