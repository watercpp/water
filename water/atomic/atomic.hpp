// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_ATOMIC_HPP
#define WATER_ATOMIC_ATOMIC_HPP
#include <water/atomic/add.hpp>
#include <water/atomic/add1.hpp>
#include <water/atomic/add1_get.hpp>
#include <water/atomic/add_get.hpp>
#include <water/atomic/alias.hpp>
#include <water/atomic/barrier.hpp>
#include <water/atomic/get.hpp>
#include <water/atomic/get_add.hpp>
#include <water/atomic/get_add1.hpp>
#include <water/atomic/get_compare_set.hpp>
#include <water/atomic/get_set.hpp>
#include <water/atomic/get_subtract.hpp>
#include <water/atomic/get_subtract1.hpp>
#include <water/atomic/compare_set.hpp>
#include <water/atomic/compare_set_else_get.hpp>
#include <water/atomic/compare_set_else_non_atomic_get.hpp>
#include <water/atomic/int.hpp>
#include <water/atomic/set.hpp>
#include <water/atomic/subtract.hpp>
#include <water/atomic/subtract1.hpp>
#include <water/atomic/subtract1_get.hpp>
#include <water/atomic/subtract_get.hpp>
#endif

#if 0
namespace water { namespace atomic {

// this is what water::atomic contains:

using int_t = built_in_signed_integer;
using uint_t = built_in_unsigned_integer;

template<typename type_> struct alias;

enum barrier_t {
	none,
	acquire,
	release,
	full
	};

void barrier();

template<barrier_t barrier_, typename type_> type_ get    (type_& atomic);
template<barrier_t barrier_, typename type_> void  set    (type_& atomic, type_ set);
template<barrier_t barrier_, typename type_> type_ get_set(type_& atomic, type_ set);

template<barrier_t barrier_, typename type_> bool  compare_set                    (type_& atomic, type_ compare, type_ set);
template<barrier_t barrier_, typename type_> type_ get_compare_set                (type_& atomic, type_ compare, type_ set);
template<barrier_t barrier_, typename type_> bool  compare_set_else_get           (type_& atomic, type_ compare, type_ set, type_& get);
template<barrier_t barrier_, typename type_> bool  compare_set_else_non_atomic_get(type_& atomic, type_ compare, type_ set, type_& get);

template<barrier_t barrier_, typename int_> void add    (int_& atomic, int_ add);
template<barrier_t barrier_, typename int_> int_ add_get(int_& atomic, int_ add);
template<barrier_t barrier_, typename int_> int_ get_add(int_& atomic, int_ add);

template<barrier_t barrier_, typename int_> void add1    (int_& atomic);
template<barrier_t barrier_, typename int_> int_ add1_get(int_& atomic);
template<barrier_t barrier_, typename int_> int_ get_add1(int_& atomic);

template<barrier_t barrier_, typename int_> void subtract    (int_& atomic, int_ subtract);
template<barrier_t barrier_, typename int_> int_ subtract_get(int_& atomic, int_ subtract);
template<barrier_t barrier_, typename int_> int_ get_subtract(int_& atomic, int_ subtract);
 
template<barrier_t barrier_, typename int_> void subtract1    (int_& atomic);
template<barrier_t barrier_, typename int_> int_ subtract1_get(int_& atomic);
template<barrier_t barrier_, typename int_> int_ get_subtract1(int_& atomic);

// all functions have versions without barrier_, using full barrier

template<typename type_> type_ get    (type_& atomic);
template<typename type_> void  set    (type_& atomic, type_ set);
template<typename type_> type_ get_set(type_& atomic, type_ set);

template<typename type_> bool  compare_set                    (type_& atomic, type_ compare, type_ set);
template<typename type_> type_ get_compare_set                (type_& atomic, type_ compare, type_ set);
template<typename type_> bool  compare_set_else_get           (type_& atomic, type_ compare, type_ set, type_& get);
template<typename type_> bool  compare_set_else_non_atomic_get(type_& atomic, type_ compare, type_ set, type_& get);

template<typename int_> void add    (int_& atomic, int_ add);
template<typename int_> int_ add_get(int_& atomic, int_ add);
template<typename int_> int_ get_add(int_& atomic, int_ add);

template<typename int_> void add1    (int_& atomic);
template<typename int_> int_ add1_get(int_& atomic);
template<typename int_> int_ get_add1(int_& atomic);

template<typename int_> void subtract    (int_& atomic, int_ subtract);
template<typename int_> int_ subtract_get(int_& atomic, int_ subtract);
template<typename int_> int_ get_subtract(int_& atomic, int_ subtract);
 
template<typename int_> void subtract1    (int_& atomic);
template<typename int_> int_ subtract1_get(int_& atomic);
template<typename int_> int_ get_subtract1(int_& atomic);

}}
#endif