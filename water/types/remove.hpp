// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_REMOVE_HPP
#define WATER_TYPES_REMOVE_HPP
#include <water/types/collection.hpp>
#include <water/types/ifel.hpp>
#include <water/types/to_void.hpp>
namespace water { namespace types {

/*

Make a new collection with all the elements of c_ except the element at at_
- if c_ contains x y z, remove<c_, 1> contains x z
the collection_kind must have

   template<typename collection_, int at_> struct remove;

*/

template<typename c_, int at_ = 0> struct
 remove :
	c_::collection_tag::result::template remove<c_, at_>
		{};

namespace _ {
	template<typename a_, typename = void> struct
	 do_remove_exists :
		false_result
			{};
	template<typename a_> struct
	 do_remove_exists<a_, to_void<typename a_::template remove<void, 0> >> :
		true_result
			{};
	}

// result true if
// - c_ is a collection, and the kind has remove<c_, 0>
// - c_ is not a collection, and it has remove<c_, 0>
template<typename c_> struct
 remove_exists :
	_::do_remove_exists<typename ifel<is_collection<c_>::result, collection_kind_of<c_>, type<c_> >::result>
		{};

}}
#endif
