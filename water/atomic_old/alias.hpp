// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#if !defined(WATER_ATOMIC_OLD_ALIAS_HPP) || defined(WATER_ATOMIC_OLD_INLINE)
#ifndef WATER_ATOMIC_OLD_INLINE
#define WATER_ATOMIC_OLD_ALIAS_HPP
#include <water/int.hpp>
#include <water/atomic_old/bits.hpp>
namespace water { namespace atomic_old {
#endif

namespace _ {
	template<
		typename a_,
		typename u_ = typename underlying::atomic_type_from<typename types::if_not_void<a_, int>::result>::result,
		typename u0_ = u_
		> struct
	 do_alias_type_from :
		types::ifel<
			(underlying::atomic<u_, full>::has & (has_compare_set | has_get_compare_set)) != 0, // if has compare_set..
			u_, // ...use it, else keep the first, u0_, and try a larger type
			do_alias_type_from<a_, typename underlying::atomic_type_from<char[sizeof(u_) + 1]>::result, u0_> 
			> {};
	template<typename a_, typename u0_> struct
	 do_alias_type_from<a_, void, u0_> :
		types::type_plain<u0_>
			{};
	template<typename u_, typename u0_> struct
	 do_alias_type_from<void, u_, u0_> :
		types::type_plain<void>
			{};
	template<typename u0_> struct
	 do_alias_type_from<void, void, u0_> :
		types::type_plain<void>
			{};
	}

// result a type the underlying implementation can use with a size
// at least sizeof(type_), or void if no such type exists
//
// if atomic_old::compare_set does not exist for all types the underlying
// implementation can use, this tries to select one where it does

template<typename type_> struct
 alias_type_from :
	_::do_alias_type_from<typename types::no_const<type_>::result>
		{};

template<typename type_> struct
 alias {
	typedef ::water::atomic_old::atomic_tag atomic_tag;
	typedef typename types::if_not_void<
		alias_type_from<type_>,
		types::if_not_void<
			types::ifel<
				types::is_int<type_>::result && !types::is_unsigned<type_>::result,
				water::int_size_at_least<sizeof(type_)>,
				water::uint_size_at_least<sizeof(type_)>
				>,
			type_
			>
		>::result atomic_type;
	typedef typename types::type<type_>::result value_type;
	atomic_type my;
	atomic_type& atomic() {
		return my;
		}
	atomic_type const& atomic() const {
		return my;
		}
	};

#ifndef WATER_ATOMIC_OLD_INLINE
}}
#endif
#endif
