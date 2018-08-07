// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_BITS_HPP
#define WATER_NUMBERS_BITS_HPP
#include <water/water.hpp>
#include <water/int.hpp>
#include <water/iterator.hpp>
#include <water/signed_representation.hpp>
#include <water/numeric_limits.hpp>
#include <water/ascii_lower_upper.hpp>
#include <water/unicode/utf.hpp>
#include <water/cmath.hpp>
namespace water { namespace numbers {

template<typename iterator_> class
 begin_end {
	iterator_
		mybegin {},
		myend {};
	public:
		begin_end() = default;
		begin_end(iterator_ begin, iterator_ end) :
			mybegin{begin},
			myend{end}
			{}
		begin_end(iterator_ begin, size_t size) :
			mybegin{begin},
			myend{begin + size}
			{}
		iterator_ begin() const {
			return mybegin;
			}
		iterator_ end() const {
			return myend;
			}
		explicit operator bool() const {
			return mybegin != myend;
			}
	};

struct nothing {};

namespace _ {
	template<typename a_, typename = void> struct
	 is_begin_end_do :
		types::bool_result<false>
			{};
	template<typename a_> struct
	 is_begin_end_do<a_, types::to_void<decltype(types::make<a_ const&>().begin() == types::make<a_ const&>().end())>> :
		types::bool_result<true>
			{};
	// these if_begin_end_do is need for visual c++
	template<typename a_, typename result_> struct
	 if_begin_end_do : types::ifel<is_begin_end_do<a_>::result, result_>
		{};
	template<typename a_, typename result_> struct
	 if_not_begin_end_do : types::ifel<!is_begin_end_do<a_>::result, result_>
		{};
	}
template<typename a_> inline constexpr bool is_begin_end() {
	return _::is_begin_end_do<typename types::no_const<types::no_reference<a_>>::result>::result;
	}

template<typename type_, typename result_ = nothing> using if_begin_end = typename _::if_begin_end_do<type_, result_>::result;
template<typename type_, typename result_ = nothing> using if_not_begin_end = typename _::if_not_begin_end_do<type_, result_>::result;

// ascii lowercase used when parsing numbers
struct transform_lowercase {
	char32_t operator()(char32_t a) const {
		return ascii_to_lower(a);
		}
	};

// group base 10 by 3 digits, base 2 by 8 digits, others by 4 digits
struct group {
	bool operator()(unsigned distance_to_point, bool /*after_point*/, unsigned base) const {
		// distance_to_point is never 0. the number of digits to the decimal point
		unsigned every = base == 10 ? 3 : base == 2 ? 8 : 4;
		return (distance_to_point % every) == 0;
		}
	};

}}
#endif
