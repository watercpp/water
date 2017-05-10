// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#if !defined(WATER_ATOMIC_OLD_BITS_HPP) || defined(WATER_ATOMIC_OLD_INLINE)
#ifndef WATER_ATOMIC_OLD_INLINE
#define WATER_ATOMIC_OLD_BITS_HPP
#include <water/atomic_old/atomic_tag.hpp>
#include <water/atomic_old/configuration.hpp>
#include <water/atomic_old/constants.hpp>
#include <water/atomic_old/raw_cast.hpp>
namespace water { namespace atomic_old {
#endif

#ifdef WATER_ATOMIC_OLD_INLINE

	namespace types = water::types;
	
	using water::atomic_old::atomic_tag;
	using water::atomic_old::atomic_type_of;
	using water::atomic_old::value_type_of;
	using water::atomic_old::value;
	
	using namespace water::atomic_old::constants;
	
	using water::atomic_old::raw_cast;

#endif

typedef types::no_unsigned<underlying::int_t>::result int_t;
typedef types::to_unsigned<underlying::int_t>::result uint_t;

template<typename if_, typename type_> using
 if_type = typename types::ifel_type<if_, type_>::result;

namespace _ {
	template<typename t_> struct
	 do_underlying_type_from :
		types::type_plain<typename underlying::atomic_type_from<t_>::result>
			{};
	template<> struct
	 do_underlying_type_from<void> :
		types::type_plain<void>
			{};
	template<> struct
	 do_underlying_type_from<void const> :
		types::type_plain<void>
			{};
	template<> struct
	 do_underlying_type_from<void const volatile> :
		types::type_plain<void>
			{};
	template<> struct
	 do_underlying_type_from<void volatile> :
		types::type_plain<void>
			{};
	}

// result a type with sizeof >= sizeof(type_) that the underlying implementation can use, or void if no such type exists.
template<typename type_> struct
 underlying_type_from :
	_::do_underlying_type_from<typename atomic_type_of<type_>::result>
		{};
		
namespace _ {
	template<typename a_, typename u_> struct
	 do_not_aliased :
		types::to_bool<types::ors<
			types::equal<types::no_unsigned<a_>, u_>,
			types::equal<types::to_unsigned<a_>, u_>
			>> {};
	template<typename a_> struct
	 do_not_aliased<a_, a_> :
		types::true_result
			{};
	template<typename a_> struct
	 do_not_aliased<a_, void> :
		types::false_result
			{};
	template<size_t s_, typename u_> struct
	 do_not_aliased<char[s_], u_> :
		types::bool_result<s_ == sizeof(u_)>
			{};
	template<size_t s_, typename u_> struct
	 do_not_aliased<unsigned char[s_], u_> :
		types::bool_result<s_ == sizeof(u_)>
			{};
	template<size_t s_> struct
	 do_not_aliased<char[s_], void> :
		types::false_result
			{};
	template<size_t s_> struct
	 do_not_aliased<unsigned char[s_], void> :
		types::false_result
			{};
	}

// result true if using type_ inside water::atomic_old will not break c++ aliasing rules (c++ standard 3.10.15). it is true when:
// - atomic_type_of<type_> == underlying_type_from<type_>
// - atomic_type_of<type_> is the unsigned or signed version of underlying_type_from<type_>
// - atomic_type_of<type_> is an array of char or unsigned char with the same size as underlying_type_from<type_>
template<typename type_> struct
 not_aliased :
	_::do_not_aliased<
		typename atomic_type_of<type_>::result,
		typename underlying_type_from<type_>::result
		> {};

namespace _ {
	
	template<typename t_, barrier_t b_, has_t h_, bool a_ = not_aliased<t_>::result, barrier_t r_ = b_> struct
	 barrier :
		types::ifel<
			(underlying::atomic<typename underlying_type_from<t_>::result, r_>::has & h_) != 0 ||
			r_ == full,
			types::integer<barrier_t, r_>,
			barrier<t_, b_, h_, true, r_ == none ? acquire : (r_ == acquire && b_ == none) ? release : full>
			> {};
	template<typename t_, barrier_t b_, has_t h_, barrier_t r_> struct
	 barrier<t_, b_, h_, false, r_> :
		types::integer<barrier_t, full>
			{};
	
	template<typename t_, barrier_t b_, has_t h_, bool a_ = not_aliased<t_>::result> struct
	 is_real :
		types::bool_result<(underlying::atomic<typename underlying_type_from<t_>::result, b_>::has & h_) != 0>
			{};
	template<typename t_, barrier_t b_, has_t h_> struct
	 is_real<t_, b_, h_, false> :
		types::false_result
			{};
			
	template<typename t_, barrier_t b_, has_t h_, bool a_ = not_aliased<t_>::result> struct
	 is_real_and_int :
		types::bool_result<
			types::is_int<t_>::result &&
			(underlying::atomic<typename underlying_type_from<t_>::result, b_>::has & h_)
			> {};
	template<typename t_, barrier_t b_, has_t h_> struct
	 is_real_and_int<t_, b_, h_, false> :
		types::false_result
			{};
			
	template<typename t_, barrier_t b_, has_t h_, bool a_ = not_aliased<t_>::result> struct
	 exists :
		types::bool_result<(underlying::atomic<typename underlying_type_from<t_>::result, barrier<t_, b_, h_, true>::result>::has & h_) != 0>
			{};
	template<typename t_, barrier_t b_, has_t h_> struct
	 exists<t_, b_, h_, false> :
		types::false_result
			{};
			
	template<typename t_, barrier_t b_, has_t h_, bool a_ = not_aliased<t_>::result> struct
	 exists_and_int :
		types::bool_result<
			types::is_int<t_>::result &&
			(underlying::atomic<typename underlying_type_from<t_>::result, barrier<t_, b_, h_, true>::result>::has & h_)
			> {};
	template<typename t_, barrier_t b_, has_t h_> struct
	 exists_and_int<t_, b_, h_, false> :
		types::false_result
			{};
			
	template<typename f_, typename a_ = typename atomic_type_of<f_>::result> struct
	 pointer_cast_do {
		static void* do_it(f_& f) { return &f.atomic(); }
		};
	template<typename f_> struct
	 pointer_cast_do<f_, f_> {
		static void* do_it(f_& f) { return &f; }
		};
		
	template<typename to_, typename from_> to_*
	 pointer_cast(from_& from) {
		return static_cast<to_*>(pointer_cast_do<from_>::do_it(from));
		}
	
	// this will not work for pod-structs:
	// type a;
	// type b = static_cast<volatile&>(a);
	template<typename a_> struct
	 volatile_if :
		types::ifel<
			types::is_int<a_>::result || types::is_pointer<a_>::result,
			a_ volatile,
			a_
			> {};

}

// result true if any atomic operation exists in the underlying
// implementation with this type_ and this barrier_
template<typename type_, barrier_t barrier_ = full> struct
 any_barrier : _::barrier<type_, barrier_, has_everything>
	{};

// result true if any atomic operation exists in the underlying
// implementation with this type_ and this or a stronger barrier_
template<typename type_, barrier_t barrier_ = full> struct
 any_exists : _::exists<type_, barrier_, has_everything>
	{};

// result true if any atomic operation exists in the underlying
// implementation with this type_ and barrier_
template<typename type_, barrier_t barrier_ = full> struct
 any_is_real : _::is_real<type_, barrier_, has_everything>
	{};

#ifndef WATER_ATOMIC_OLD_INLINE
}}
#endif
#endif
