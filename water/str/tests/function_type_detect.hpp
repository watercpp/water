// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_TESTS_FUNCTION_TYPE_DETECT_HPP
#define WATER_STR_TESTS_FUNCTION_TYPE_DETECT_HPP
#include <water/str/tests/bits.hpp>
namespace water { namespace str { namespace tests {

/*

find something that works with lambdas on visual c++ 2015 and 2017

*/

namespace function_type_detects {

	template<typename function_, typename = void> struct detect1 {};
	template<typename function_> struct detect1<
		function_,
		types::to_void<decltype(types::make<function_&>()(static_cast<char const*>(0), static_cast<char const*>(0)))>
		> : types::type_plain<char>
			{};

	template<typename function_, typename = void> struct detect2_16 {};
	template<typename function_> struct detect2_16<
		function_,
		types::to_void<decltype(types::make<function_&>()(types::make<char16_t const*>(), types::make<char16_t const*>()))>
		> : types::type_plain<char16_t>
			{};
	
	template<typename function_, typename = void> struct detect2 : detect2_16<function_> {};
	template<typename function_> struct detect2<
		function_,
		types::to_void<decltype(types::make<function_&>()(types::make<char const*>(), types::make<char const*>()))>
		> : types::type_plain<char>
			{};

	template<typename function_, typename = void, typename = void> struct detect3 {};
	template<typename function_> struct detect3<
		function_,
		types::to_void<decltype(types::make<function_&>()(types::make<char const*>(), types::make<char const*>()))>,
		void
		> : types::type_plain<char>
			{};
	template<typename function_> struct detect3<
		function_,
		void,
		types::to_void<decltype(types::make<function_&>()(types::make<char16_t const*>(), types::make<char16_t const*>()))>
		> : types::type_plain<char16_t>
			{};

	template<typename function_, typename = void> struct detect4_char : types::type_plain<void> {};
	template<typename function_> struct detect4_char<function_, types::to_void<decltype(types::make<function_&>()(types::make<char const*>(), types::make<char const*>()))>> : types::type_plain<char> {};

	template<typename function_, typename = void> struct detect4_char16 : types::type_plain<void> {};
	template<typename function_> struct detect4_char16<function_, types::to_void<decltype(types::make<function_&>()(types::make<char16_t const*>(), types::make<char16_t const*>()))>> : types::type_plain<char16_t> {};

	template<typename function_, typename char_ = typename detect4_char<function_>::result, typename char16_ = typename detect4_char16<function_>::result> struct detect4 {};
	template<typename function_> struct detect4<function_, char, void> : types::type_plain<char> {};
	template<typename function_> struct detect4<function_, void, char16_t> : types::type_plain<char16_t> {};

	template<typename function_> struct detect5 :
		types::ifel<!types::equal<void, detect4_char<function_>>::result, char,
		types::ifel<!types::equal<void, detect4_char16<function_>>::result, char16_t
		>> {};

	template<typename function_> void test6(...);
	template<typename function_> char test6(function_ *f, decltype((*f)(static_cast<char const*>(0), static_cast<char const*>(0)))* = 0);
	template<typename function_> char16_t test6(function_ *f, decltype((*f)(static_cast<char16_t const*>(0), static_cast<char16_t const*>(0)))* = 0);

	template<typename function_, typename char_ = decltype(test6<typename types::no_reference<function_>::result>(0))> struct detect6 : types::type_plain<char_> {};
	template<typename function_> struct detect6<function_, void> {};
	
	template<typename char_> struct
	 function {
		void operator()(char_ const*, char_ const*) {}
		};

	template<typename function_, typename char_ = typename detect6<function_>::result> struct
	 create_return {
		explicit operator bool() const { return true; }
		};

	template<typename function_> create_return<function_> create(function_&&) {
		return {};
		}
}

template<typename char_, typename function1_, typename function2_> bool function_type_detect(function1_&& f1, function2_&& f2) {
	using namespace function_type_detects;
	using char1 = typename detect6<function1_>::result;
	static_assert(types::equal<char_, char1>::result, "oh no!");
	auto r1a = create(f1);
	auto r1b = create(function1_(f1));

	using char2 = typename detect6<function2_>::result;
	static_assert(types::equal<char_, char2>::result, "oh no!");
	auto r2a = create(f2);
	auto r2b = create(function2_(f2));
	
	return r1a && r1b && r2a && r2b; // no unused warning
	}

inline void function_type_detect_all() {
	function_type_detect<char>(function_type_detects::function<char>{}, [](char const*, char const*) {});
	function_type_detect<char16_t>(function_type_detects::function<char16_t>{}, [](char16_t const*, char16_t const*) {});
	}

}}}
#endif
