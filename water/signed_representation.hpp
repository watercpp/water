// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_SIGNED_REPRESENTATION_HPP
#define WATER_SIGNED_REPRESENTATION_HPP
#include <water/types/types.hpp>
namespace water {

/*

Find out if signed integer types are repesented as twos complement, ones complement or signed magniture.
This has never been tested on anything except twos complement, does it even exist any more? DSP?
It also uses the ~ operator on signed types. But it should work in theory.

A 3-bit signed integer whould have these values in the different representations:

Bits      Ones complement    Twos complement    Signed magnitude
000 ............. 0 ................ 0 ................ 0
001 ............. 1 ................ 1 ................ 1
010 ............. 2 ................ 2 ................ 2
011 ............. 3 ................ 3 ................ 3
100 ............ -3 ............... -4 ............... -0
101 ............ -2 ............... -3 ............... -1
110 ............ -1 ............... -2 ............... -2
111 ............ -0 ............... -1 ............... -3

*/

enum class signed_representation {
	none = 0,
	ones_complement = 1,
	twos_complement = 2,
	signed_magnitude = -1
	};

namespace _ {
	template<typename int_> struct
	 signed_representation_do : types::integer<
		signed_representation,
		(static_cast<int_>(-1) >= static_cast<int_>(0)) ? signed_representation::none : // unsigned
		(~static_cast<int_>(0) == static_cast<int_>(-1)) ? signed_representation::twos_complement : // 0xffff is -1
		(~static_cast<int_>(0) == static_cast<int_>(-0)) ? signed_representation::ones_complement : // 0xffff is -0 could be a trap value
		(~static_cast<int_>(0) <= static_cast<int_>(-2)) ? signed_representation::signed_magnitude : // 0xffff is -max
		signed_representation::none
		> {};
	}

template<typename type_> constexpr signed_representation signed_representation_of() {
	return types::ifel<
		types::is_int<type_>::result,
		_::signed_representation_do<typename types::type<type_>::result>,
		types::integer<signed_representation, signed_representation::none>
		>::result;
	}

template<typename type_> constexpr bool is_ones_complement() {
	return signed_representation_of<type_>() == signed_representation::ones_complement;
	}

template<typename type_> constexpr bool is_twos_complement() {
	return signed_representation_of<type_>() == signed_representation::twos_complement;
	}

template<typename type_> constexpr bool is_signed_magnitude() {
	return signed_representation_of<type_>() == signed_representation::signed_magnitude;
	}

}
#endif
