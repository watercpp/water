// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_ATOMIC_TAG_HPP
#define WATER_ATOMIC_ATOMIC_TAG_HPP
#include <water/types/types.hpp>
namespace water { namespace atomic {

struct atomic_tag {};

namespace _ {
	
	template<typename a_> struct
	 do_atomic_tag_to_void
		{};
	template<> struct
	 do_atomic_tag_to_void<atomic_tag> : types::type_plain<void>
		{};
		
	template<typename a_, typename x_ = void> struct
	 do_has_atomic_tag :
		types::false_result
			{};
	template<typename a_> struct
	 do_has_atomic_tag<a_, typename do_atomic_tag_to_void<typename a_::atomic_tag>::result> :
		types::true_result
			{};
	}

// result true if a_::atomic_tag exists and is a typedef of atomic::atomic_tag
template<typename a_> struct
 has_atomic_tag : _::do_has_atomic_tag<typename types::type<a_>::result>
	{};

namespace _ {
	template<typename a_, typename x_ = void> struct
	 do_atomic_type_of :
		types::type_plain<a_>
			{};
	template<typename a_> struct
	 do_atomic_type_of<a_, typename do_atomic_tag_to_void<typename a_::atomic_tag>::result> :
		types::type_plain<typename a_::atomic_type>
			{};
	}

// result
// - a_::atomic_type if atomic::has_atomic_tag<a_>
// - else a_ 
template<typename a_> struct
 atomic_type_of : _::do_atomic_type_of<typename types::type<a_>::result>
	{};


namespace _ {
	template<typename a_, typename x_ = void> struct
	 do_value_type_of :
		types::type_plain<a_>
			{};
	template<typename a_> struct
	 do_value_type_of<a_, typename do_atomic_tag_to_void<typename a_::atomic_tag>::result> :
		types::type_plain<typename a_::value_type>
			{};
	}

// result
// - a_::value_type if atomic::has_atomic_tag<a_>
// - else a_ 
template<typename a_> struct
 value_type_of : _::do_value_type_of<typename types::type<a_>::result>
	{};

template<typename a_> using
 value = typename value_type_of<a_>::result;

}}
#endif
