// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#if !defined(WATER_ATOMIC_OLD_INT_HPP) || defined(WATER_ATOMIC_OLD_INLINE)
#ifndef WATER_ATOMIC_OLD_INLINE
#define WATER_ATOMIC_OLD_INT_HPP
#include <water/numeric_limits.hpp>
#include <water/atomic_old/bits.hpp>
namespace water { namespace atomic_old {
#endif

/*

similar to water/int.hpp except for atomic integer types

*/

namespace _ {

	// all int types
	typedef
		types::link<char,
		types::link<signed char,
		types::link<unsigned char,
		types::link<short,
		types::link<unsigned short,
		types::link<int,
		types::link<unsigned int,
		types::link<long,
		types::link<unsigned long,
		types::link<long long,
		types::link<unsigned long long
		> > > > > > > > > > >
			do_int_types_all;

	template<int at_ = types::size<do_int_types_all>::result - 1, typename result_ = types::link<> > struct
	 do_int_types :
		do_int_types<
			at_ - 1,
			typename types::ifel_plain<
				types::equal<
					types::at<do_int_types_all, at_>,
					typename underlying::atomic_type_from<typename types::at<do_int_types_all, at_>::result>::result
					>::result,
				types::insert<result_, typename types::at<do_int_types_all, at_>::result>,
				result_
				>::result
			> {};
	template<typename result_> struct
	 do_int_types<-1, result_> :
		types::pull<result_>
			{};

}

using int_types = _::do_int_types<>::result;
// a water::types::collection of the built-in integer types that the underlying implementation can use.
// it is an empty collecton if no atomic operations exist.
// ordered from small to large: char, short, int, long, long long

namespace _ {
	
	template<
		template<size_t, typename> class select_,
		size_t size_,
		typename at_ = int_types,
		typename result_ = void,
		bool is_result_ = types::is_result<at_>::result
		> struct
	 do_int_find :
		types::type_plain<result_>
			{};
	template<template<size_t, typename> class select_, size_t size_, typename at_> struct
	 do_int_find<select_, size_, at_, void, true> :
		do_int_find<
			select_,
			size_,
			types::next<at_>,
			typename select_<size_, typename at_::result>::result
			> {};
	
	template<size_t bits_, typename int_> struct
	 do_int_bits_at_least :
		types::ifel<water::numeric_limits<int_>::digits + (water::numeric_limits<int_>::is_signed ? 1 : 0) >= bits_, int_, void>
			{};

	template<size_t bits_, typename int_> struct
	 do_int_bits :
		types::ifel<water::numeric_limits<int_>::digits + (water::numeric_limits<int_>::is_signed ? 1 : 0) == bits_, int_, void>
			{};
	
	template<size_t size_, typename int_> struct
	 do_int_size_at_least :
		types::ifel<sizeof(int_) >= size_, int_, void>
			{};
	
	template<size_t size_, typename int_> struct
	 do_int_size :
		types::ifel<sizeof(int_) == size_, int_, void>
			{};
}

using int_largest_t = types::ifel_type<
	types::is_result<types::at<int_types, types::size<int_types>::result - 1>>,
	types::no_unsigned<types::at<int_types, types::size<int_types>::result - 1>>,
	void
	>::result;

using uint_largest_t = types::to_unsigned<int_largest_t>::result;

template<size_t bits_> using int_bits_at_least  = typename types::no_unsigned<_::do_int_find<_::do_int_bits_at_least, bits_>>::result;
template<size_t bits_> using uint_bits_at_least = typename types::to_unsigned<_::do_int_find<_::do_int_bits_at_least, bits_>>::result;

template<size_t bits_> using int_bits  = typename types::no_unsigned<_::do_int_find<_::do_int_bits, bits_>>::result;
template<size_t bits_> using uint_bits = typename types::to_unsigned<_::do_int_find<_::do_int_bits, bits_>>::result;

template<size_t bytes_> using int_size_at_least  = typename types::no_unsigned<_::do_int_find<_::do_int_size_at_least, bytes_>>::result;
template<size_t bytes_> using uint_size_at_least = typename types::to_unsigned<_::do_int_find<_::do_int_size_at_least, bytes_>>::result;

template<size_t bytes_> using int_size  = typename types::no_unsigned<_::do_int_find<_::do_int_size, bytes_>>::result;
template<size_t bytes_> using uint_size = typename types::to_unsigned<_::do_int_find<_::do_int_size, bytes_>>::result;

#ifndef WATER_ATOMIC_OLD_INLINE
}}
#endif
#endif
