// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_TEST_LIST_HPP
#define WATER_THREADS_TESTS_TEST_LIST_HPP
#include <water/threads/bits.hpp>
namespace water { namespace threads { namespace tests {

template<template<typename> class test_, typename type_, unsigned need_ = type_::needs::need> struct
 test_list_if {
 	template<typename ...arguments_>
 	 void operator()(arguments_... a) {
 	 	test_<type_> t(a...); // gcc will test_<type_> t({}); when arguments is empty
 	 	}
 	void operator()() {
 	 	test_<type_>{};
 	 	}
 	};
template<template<typename> class test_, typename type_> struct
 test_list_if<test_, type_, 0> {
 	template<typename ...arguments_> void operator()(arguments_...) {}
 	};

template<template<typename> class test_, typename ...list_> struct
 test_list_do;
 
template<template<typename> class test_, typename type_, typename ...list_> struct
 test_list_do<test_, type_, list_...> {
 	template<typename ...arguments_> void operator()(arguments_... a) {
 		test_list_if<test_, type_>()(a...);
 		test_list_do<test_, list_...>()(a...);
 		}
	};

template<template<typename> class test_> struct
 test_list_do<test_> {
 	template<typename ...arguments_> void operator()(arguments_...) {}
 	};

template<template<typename> class test_, typename list_> struct
 test_list_expand;

template<template<typename> class test_, template<typename ...> class class_, typename ...list_> struct
 test_list_expand<test_, class_<list_...> > {
 	template<typename ...arguments_>
 	 void operator()(arguments_... a) {
 		test_list_do<test_, list_...>()(a...);
 		}
 	};

template<template<typename> class test_, typename list_, typename ...arguments_> void
 test_list(arguments_... a) {
 	test_list_expand<test_, list_>()(a...);
 	}

}}}
#endif
