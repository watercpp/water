// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_SIZE_HPP
#define WATER_TYPES_SIZE_HPP
#include <water/types/collection.hpp>
#include <water/types/ifel.hpp>
#include <water/types/to_void.hpp>
namespace water { namespace types {

/*

result the number of elements in the collection c_ from the current position to the last
- if size<c_>::result is 10, size<types::at<c_, 7> >::result is 3
- result is always 0 or greater

the collection_kind must have:

  template<typename collection_> struct size;

*/

template<typename c_>
struct size :
    c_::collection_tag::result::template size<c_>
{};

namespace _ {

    template<typename, typename = void>
    struct do_size_exists :
        false_result
    {};
    
    template<typename a_>
    struct do_size_exists<a_, to_void<typename a_::template size<void>>> :
        true_result
    {};
    
}

// result true if
// - c_ is a collection, and the kind has size<c_>
// - c_ is not a collection, and it has size<c_>
template<typename c_>
struct size_exists :
    _::do_size_exists<typename ifel<is_collection<c_>::result, collection_kind_of<c_>, type<c_> >::result>
{};

}}
#endif
