// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#if !defined(WATER_ATOMIC_TESTS_INT_HPP) || defined(WATER_ATOMIC_INLINE)
#ifndef WATER_ATOMIC_INLINE
#define WATER_ATOMIC_TESTS_INT_HPP
#include <water/atomic/tests/bits.hpp>
namespace water { namespace atomic { namespace tests {
#else
namespace str = water::str;
namespace types = water::types;
using water::numeric_limits;
#endif

/*

test atomic/int.hpp, stolen from water/int.hpp test

compile time, just include this

*/

template<size_t bits_> constexpr int int_bits_test() {	
 	static_assert(!types::equal<void, int_bits_at_least<bits_>>::result || bits_ > numeric_limits<int_largest_t>::digits + 1, "");
 	static_assert(numeric_limits<int_bits<bits_>>::digits == 0 || (numeric_limits<int_bits<bits_>>::digits == numeric_limits<int_bits_at_least<bits_>>::digits && numeric_limits<int_bits<bits_>>::digits == (bits_ ? bits_ - 1 : 0)), "");
	static_assert(!types::equal<void, uint_bits_at_least<bits_>>::result || bits_ > numeric_limits<uint_largest_t>::digits, "");
 	static_assert(numeric_limits<uint_bits<bits_>>::digits == 0 || (numeric_limits<uint_bits<bits_>>::digits == numeric_limits<uint_bits_at_least<bits_>>::digits && numeric_limits<uint_bits<bits_>>::digits == bits_), "");
	return 1;
	}

template<typename type_> constexpr size_t int_sizeof_or_0() { return sizeof(type_); }
template<> constexpr size_t int_sizeof_or_0<void>() { return 0; }

template<size_t size_> constexpr int int_size_test() {	
 	static_assert(!types::equal<void, int_size_at_least<size_>>::result || size_ > int_sizeof_or_0<int_largest_t>(), "");
 	static_assert(int_sizeof_or_0<int_size<size_>>() == 0 || (int_sizeof_or_0<int_size<size_>>() == int_sizeof_or_0<int_size_at_least<size_>>() && int_sizeof_or_0<int_size<size_>>() == size_), "");
	static_assert(!types::equal<void, uint_size_at_least<size_>>::result || size_ > int_sizeof_or_0<uint_largest_t>(), "");
 	static_assert(int_sizeof_or_0<uint_size<size_>>() == 0 || (int_sizeof_or_0<uint_size<size_>>() == int_sizeof_or_0<uint_size_at_least<size_>>() && int_sizeof_or_0<uint_size<size_>>() == size_), "");
	return 1;
	}

/*template<size_t digits_> constexpr int int_digits10_test() {
 	static_assert(!types::equal<void, int_digits10_at_least<digits_>>::result || digits_ > numeric_limits<int_largest_t>::digits10, "");
	static_assert(!types::equal<void, uint_digits10_at_least<digits_>>::result || digits_ > numeric_limits<uint_largest_t>::digits10, "");
 	return 1;
	}*/

template<size_t at_ = 0, bool end_ = (at_ >= numeric_limits<uint_largest_t>::digits + numeric_limits<uint_largest_t>::digits / 2)> struct
 int_bits_all {
	static constexpr int do_it() { // reduce template depth
		return
			int_bits_test<at_ + 0>() +
			int_bits_test<at_ + 1>() +
			int_bits_test<at_ + 2>() +
			int_bits_test<at_ + 3>() +
			int_bits_test<at_ + 4>() +
			int_bits_test<at_ + 5>() +
			int_bits_test<at_ + 6>() +
			int_bits_test<at_ + 7>() +
			int_bits_all<at_ + 8>::do_it();
		}
	};
template<size_t at_> struct
 int_bits_all<at_, true> {
	static constexpr int do_it() { return 0; }
	};
 
template<size_t at_ = 0, bool end_ = (at_ >= sizeof(uint_largest_t) * 2)> struct
 int_size_all {
	static constexpr int do_it() {
		return
			int_size_test<at_ + 0>() +
			int_size_test<at_ + 1>() +
			int_size_test<at_ + 2>() +
			int_size_test<at_ + 3>() +
			int_size_all<at_ + 4>::do_it();
		}
	};
template<size_t at_> struct
 int_size_all<at_, true> {
	static constexpr int do_it() { return 0; }
	};

/*template<size_t at_ = 0, bool end_ = (at_ >= numeric_limits<uint_largest_t>::digits10 * 2)> struct
 int_digits10_all {
	static constexpr int do_it() {
		return
			int_digits10_test<at_ + 0>() +
			int_digits10_test<at_ + 1>() +
			int_digits10_test<at_ + 2>() +
			int_digits10_test<at_ + 3>() +
			int_digits10_all<at_ + 4>::do_it();
		}
	};
template<size_t at_> struct
 int_digits10_all<at_, true> {
	static constexpr int do_it() { return 0; }
	};*/
 	
 int constexpr int_all = int_bits_all<>::do_it() + int_size_all<>::do_it() /*+ int_digits10_all<>::do_it()*/;

#ifndef WATER_ATOMIC_INLINE
}}}
#endif
#endif
