// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_PULL_HPP
#define WATER_TYPES_PULL_HPP
#include <water/types/collection.hpp>
#include <water/types/ifel.hpp>
#include <water/types/to_void.hpp>
namespace water { namespace types {
		

/*

When a collcection has pull, it should result the plain collection type, to get it out of nested
templates.

pull<remove<insert<list<int, bool>, void>, 2> >::result should be list<void, int>

If the collection_kind has pull, it must look like

  template<typename collection_> struct pull;


It could avoid some compiler limit, and be easier to see whats in a collection from compiler output
or typeid::name() 

*/

namespace _ {
	template<typename k_, typename c_, typename = void> struct
	 do_pull :
		type_plain<c_>
			{};
	template<typename k_, typename c_> struct
	 do_pull<k_, c_, to_void<typename k_::template pull<void> >> :
		k_::template pull<c_>
			{};
	}

// if c_ is a collection, and the collection_kind has pull, result collection_kind::pull<c_> 
// otherwise results c_
template<typename c_> struct
 pull :
	_::do_pull<typename collection_kind_of<c_>::result, c_>
		{};
		
namespace _ {
	template<typename, typename = void> struct
	 do_pull_exists :
		false_result
			{};
	template<typename a_> struct
	 do_pull_exists<a_, to_void<typename a_::template pull<void> >> :
		true_result
			{};
	}
// result true if
// - c_ is a collection, and the kind has pull<c_>
// - c_ is not a collection, and it has pull<c_>
template<typename c_> struct
 pull_exists :
	_::do_pull_exists<typename ifel<is_collection<c_>::result, collection_kind_of<c_>, type<c_> >::result>
		{};

}}
#endif
