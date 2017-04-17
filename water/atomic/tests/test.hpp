// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#if !defined(WATER_ATOMIC_TESTS_TEST_HPP) || defined(WATER_ATOMIC_INLINE)
#ifndef WATER_ATOMIC_INLINE
#define WATER_ATOMIC_TESTS_TEST_HPP
#include <water/atomic/tests/bits.hpp>
namespace water { namespace atomic { namespace tests {
#else
namespace str = water::str;
namespace types = water::types;
using water::numeric_limits;
#endif

/*

test all functions

*/

inline void test(); // run this. its defined at the bottom

template<unsigned s_> struct
 bytes {
	unsigned char my_[s_];
	bool operator==(bytes const& a) const {
		unsigned i = 0;
		while(my_[i] == a.my_[i] && ++i != s_);
		return i == s_;
		}
	bool operator!=(bytes const& a) const {
		return !this->operator==(a);
		}
	void make0() {
		unsigned i = 0;
		do my_[i] = 0; while(++i != s_);
		}
	void make1() {
		unsigned i = 0;
		do my_[i] = static_cast<unsigned char>(static_cast<unsigned>(-1) ^ i); while(++i != s_);
		}
	};

template<typename a_> class
 make0 {
	public:
		operator a_() const { return static_cast<a_>(0); }
	};
template<unsigned s_> class
 make0<bytes<s_> > {
	enum { assert = types::if_assert<sizeof(bytes<s_>) == s_>::assert };
	bytes<s_> my_;
	public:
		make0() { my_.make0(); }
		operator bytes<s_> const&() const { return my_; }
	};
template<typename a_> class
 make0<alias<a_> > {
	typedef typename alias<a_>::value_type v_;
	make0<v_> my_;
	public:
		operator v_() const { return my_; }
		operator alias<a_>() const {
			alias<a_> r;
			r.atomic() = water::atomic::raw_cast<typename alias<a_>::atomic_type>(static_cast<v_>(my_));
			return r;
			};
	};
	
template<typename a_> class
 make1 {
	public:
		operator a_() const { return numeric_limits<a_>::max(); }
	};
template<> class
 make1<float> {
	public:
		operator float() const { return 123.456e7f; }
	};
template<> class
 make1<double> {
	public:
		operator double() const { return 123.456e7; }
	};
template<> class
 make1<bool> {
	public:
		operator bool() const { return true; }
	};
template<typename a_> class
 make1<a_*> {
	char myarray[sizeof(void*) * 2];
	void *my;
	public:
		make1() { my = myarray; }
		operator a_*() const { return static_cast<a_*>(my); }
	};
template<unsigned s_> class
 make1<bytes<s_> > {
	enum { assert = types::if_assert<sizeof(bytes<s_>) == s_>::assert };
	bytes<s_> my;
	public:
		make1() { my.make1(); }
		operator bytes<s_> const&() const { return my; }
	};
template<typename a_> class
 make1<alias<a_> > {
	typedef typename alias<a_>::value_type v_;
	make1<v_> my;
	public:
		operator v_() const { return my; }
		operator alias<a_>() const {
			alias<a_> r;
			r.atomic() = water::atomic::raw_cast<typename alias<a_>::atomic_type>(static_cast<v_>(my));
			return r;
			};
	};
	
template<typename a_> bool equal(a_ const& a, typename types::type_plain<a_>::result const& b) {
	return a == b;
	}
template<typename a_> bool equal(alias<a_> const& a, typename types::type_plain<alias<a_> >::result const& b) {
	return a.atomic() == b.atomic();
	}
template<typename a_> bool equal(alias<a_> const& a, a_ const& b) {
	return raw_cast<a_>(a.atomic()) == b;
	}

// get
/////////////////////////////////////////////////////////////////////

template<typename a_, barrier_t b_> struct
 get_test_do {
	template<typename v_> static void do_it(a_&, v_&) {}
	};
template<typename a_> struct
 get_test_do<a_, full> {
	template<typename v_> static void do_it(a_& a, v_& v) { v = get(a); }
	};

template<typename a_, barrier_t b_> typename types::ifel_type<get_exists<a_, b_>, bool>::result get_test() {
	typedef typename water::atomic::value_type_of<a_>::result v_;
	make0<a_> m0;
	make1<a_> m1;
	a_ a = m0;
	v_ v = get<b_>(a);
	___water_test(equal(v, m0));
	a = m1;
	v = get<b_>(a);
	___water_test(equal(v, m1));
	get_test_do<a_, b_>::do_it(a, v);
	return true;
	}
template<typename a_, barrier_t b_> typename types::ifel_type_not<get_exists<a_, b_>, bool>::result get_test() {
	return true;
	}
	
// set
/////////////////////////////////////////////////////////////////////

template<typename a_, barrier_t b_> struct
 set_test_do {
	template<typename v_> static void do_it(a_&, v_) {}
	};
template<typename a_> struct
 set_test_do<a_, full> {
	template<typename v_> static void do_it(a_& a, v_ v) { set(a, v); }
	};

template<typename a_, barrier_t b_> typename types::ifel_type<set_exists<a_, b_>, bool>::result set_test() {
	make0<a_> m0;
	make1<a_> m1;
	a_ a;
	set<b_>(a, m0);
	___water_test(equal(a, m0));
	set<b_>(a, m1);
	___water_test(equal(a, m1));
	set_test_do<a_, b_>::do_it(a, m0);
	return true;
	}
template<typename a_, barrier_t b_> typename types::ifel_type_not<set_exists<a_, b_>, bool>::result set_test() {
	return true;
	}
	
// get_set
/////////////////////////////////////////////////////////////////////

template<typename a_, barrier_t b_> struct
 get_set_test_do {
	template<typename v_> static void do_it(a_&, v_&) {}
	};
template<typename a_> struct
 get_set_test_do<a_, full> {
	template<typename v_> static void do_it(a_& a, v_& v) { v = get_set(a, v); }
	};

template<typename a_, barrier_t b_> typename types::ifel_type<get_set_exists<a_, b_>, bool>::result get_set_test() {
	typedef typename water::atomic::value_type_of<a_>::result v_;
	make0<a_> m0;
	make1<a_> m1;
	a_ a = m0;
	v_ v = get_set<b_>(a, m1);
	___water_test(equal(a, m1) && equal(v, m0));
	a = m1;
	v = get_set<b_>(a, m0);
	___water_test(equal(a, m0) && equal(v, m1));
	get_set_test_do<a_, b_>::do_it(a, v);
	return true;
	}
template<typename a_, barrier_t b_> typename types::ifel_type_not<get_set_exists<a_, b_>, bool>::result get_set_test() {
	return true;
	}

// compare_set
/////////////////////////////////////////////////////////////////////

template<typename a_, barrier_t b_> struct
 compare_set_test_do {
	static void do_it() {}
	};
template<typename a_> struct
 compare_set_test_do<a_, full> {
	static bool do_it() {
		make0<a_> m0;
		a_ a = m0;
		return compare_set(a, m0, m0);
		}
	};

template<typename a_, barrier_t b_> typename types::ifel_type<compare_set_exists<a_, b_>, bool>::result compare_set_test() {
	make0<a_> m0;
	make1<a_> m1;
	a_ a = m0;
	bool b = compare_set<b_>(a, m1, m0);
	___water_test(!b);
	___water_test(equal(a, m0));
	b = compare_set<b_>(a, m0, m1);
	___water_test(b);
	___water_test(equal(a, m1));
	a = m1;
	b = compare_set<b_>(a, m0, m1);
	___water_test(!b);
	___water_test(equal(a, m1));
	b = compare_set<b_>(a, m1, m0);
	___water_test(b);
	___water_test(equal(a, m0));
	compare_set_test_do<a_, b_>::do_it();
	return true;
	}
template<typename a_, barrier_t b_> typename types::ifel_type_not<compare_set_exists<a_, b_>, bool>::result compare_set_test() {
	return true;
	}

// compare_set_else_get
/////////////////////////////////////////////////////////////////////

template<typename a_, barrier_t b_> struct
 compare_set_else_get_test_do {
	static void do_it() {}
	};
template<typename a_> struct
 compare_set_else_get_test_do<a_, full> {
	static bool do_it() {
		make0<a_> m0;
		a_ a = m0;
		value<a_> g;
		return compare_set_else_get(a, m0, m0, g);
		}
	};

template<typename a_, barrier_t b_> typename types::ifel_type<compare_set_else_get_exists<a_, b_>, bool>::result compare_set_else_get_test() {
	make0<a_> m0;
	make1<a_> m1;
	a_ a = m0;
	value<a_> g;
	bool b = compare_set_else_get<b_>(a, m1, m0, g);
	___water_test(!b);
	___water_test(equal(a, m0));
	___water_test(equal(a, g));
	b = compare_set_else_get<b_>(a, m0, m1, g);
	___water_test(b);
	___water_test(equal(a, m1));
	a = m1;
	b = compare_set_else_get<b_>(a, m0, m1, g);
	___water_test(!b);
	___water_test(equal(a, m1));
	___water_test(equal(a, g));
	b = compare_set_else_get<b_>(a, m1, m0, g);
	___water_test(b);
	___water_test(equal(a, m0));
	compare_set_test_do<a_, b_>::do_it();
	return true;
	}
template<typename a_, barrier_t b_> typename types::ifel_type_not<compare_set_else_get_exists<a_, b_>, bool>::result compare_set_else_get_test() {
	return true;
	}

// compare_set_else_non_atomic_get
/////////////////////////////////////////////////////////////////////

template<typename a_, barrier_t b_> struct
 compare_set_else_non_atomic_get_test_do {
	static void do_it() {}
	};
template<typename a_> struct
 compare_set_else_non_atomic_get_test_do<a_, full> {
	static bool do_it() {
		make0<a_> m0;
		a_ a = m0;
		value<a_> g;
		return compare_set_else_non_atomic_get(a, m0, m0, g);
		}
	};

template<typename a_, barrier_t b_> typename types::ifel_type<compare_set_else_non_atomic_get_exists<a_, b_>, bool>::result compare_set_else_non_atomic_get_test() {
	make0<a_> m0;
	make1<a_> m1;
	a_ a = m0;
	value<a_> g;
	bool b = compare_set_else_non_atomic_get<b_>(a, m1, m0, g);
	___water_test(!b);
	___water_test(equal(a, m0));
	___water_test(equal(a, g));
	b = compare_set_else_non_atomic_get<b_>(a, m0, m1, g);
	___water_test(b);
	___water_test(equal(a, m1));
	a = m1;
	b = compare_set_else_non_atomic_get<b_>(a, m0, m1, g);
	___water_test(!b);
	___water_test(equal(a, m1));
	___water_test(equal(a, g));
	b = compare_set_else_non_atomic_get<b_>(a, m1, m0, g);
	___water_test(b);
	___water_test(equal(a, m0));
	compare_set_test_do<a_, b_>::do_it();
	return true;
	}
template<typename a_, barrier_t b_> typename types::ifel_type_not<compare_set_else_non_atomic_get_exists<a_, b_>, bool>::result compare_set_else_non_atomic_get_test() {
	return true;
	}

// get_compare_set
/////////////////////////////////////////////////////////////////////

template<typename a_, typename v_, barrier_t b_> struct
 get_compare_set_test_do {
	static void do_it() {}
	};
template<typename a_, typename v_> struct
 get_compare_set_test_do<a_, v_, full> {
	static bool do_it() {
		make0<a_> m0;
		a_ a = m0;
		v_ v = m0;
		v = get_compare_set(a, v, v);
		return v == m0;
		}
	};

template<typename a_, barrier_t b_> typename types::ifel_type<get_compare_set_exists<a_, b_>, bool>::result get_compare_set_test() {
	typedef typename water::atomic::value_type_of<a_>::result v_;
	make0<a_> m0;
	make1<a_> m1;
	a_ a = m0;
	v_ v = get_compare_set<b_>(a, m1, m0);
	___water_test(equal(a, m0) && equal(v, m0));
	v = get_compare_set<b_>(a, m0, m1);
	___water_test(equal(a, m1) && equal(v, m0));
	a = m1;
	v = get_compare_set<b_>(a, m0, m1);
	___water_test(equal(a, m1) && equal(v, m1));
	v = get_compare_set<b_>(a, m1, m0);
	___water_test(equal(a, m0) && equal(v, m1));
	get_compare_set_test_do<a_, v_, b_>::do_it();
	return true;
	}
template<typename a_, barrier_t b_> typename types::ifel_type_not<get_compare_set_exists<a_, b_>, bool>::result get_compare_set_test() {
	return true;
	}
	
// add
/////////////////////////////////////////////////////////////////////

template<typename a_, barrier_t b_> struct
 add_test_do {
	static void do_it() {}
	};
template<typename a_> struct
 add_test_do<a_, full> {
	static a_ do_it() {
		a_ a = 0;
		add(a, 1);
		return a;
		}
	};

template<typename a_, barrier_t b_> typename types::ifel_type<add_exists<a_, b_>, bool>::result add_test() {
	a_ const
		min = numeric_limits<a_>::min(),
		max = numeric_limits<a_>::max();
	a_ a = 0;
	add<b_>(a, max);
	___water_test(a == max);
	add<b_>(a, min);
	___water_test(a == static_cast<a_>(max + min));
	a = min;
	add<b_>(a, max);
	___water_test(a == static_cast<a_>(min + max));
	add_test_do<a_, b_>::do_it();
	return true;
	}
template<typename a_, barrier_t b_> typename types::ifel_type_not<add_exists<a_, b_>, bool>::result add_test() {
	return true;
	}
	
// add_get get_add
/////////////////////////////////////////////////////////////////////

template<typename a_, barrier_t b_> struct
 add_get_test_do {
	static void do_it() {}
	};
template<typename a_> struct
 add_get_test_do<a_, full> {
	static a_ do_it() {
		a_ a = 0;
		a_ b = add_get(a, 1);
		b = get_add(a, 1);
		return b;
		}
	};

template<typename a_, barrier_t b_> typename types::ifel_type<add_get_exists<a_, b_>, bool>::result add_get_test() {
	a_ const
		min = numeric_limits<a_>::min(),
		max = numeric_limits<a_>::max();
	a_ a, b;
	// get_add
	a = 0;
	b = get_add<b_>(a, max);
	___water_test(a == max && b == static_cast<a_>(0));
	b = get_add<b_>(a, min);
	___water_test(a == static_cast<a_>(max + min) && b == max);
	a = min;
	b = get_add<b_>(a, max);
	___water_test(a == static_cast<a_>(min + max) && b == min);
	// add_get
	a = 0;
	b = add_get<b_>(a, max);
	___water_test(a == max && b == a);
	b = add_get<b_>(a, min);
	___water_test(a == static_cast<a_>(max + min) && b == a);
	a = min;
	b = add_get<b_>(a, max);
	___water_test(a == static_cast<a_>(min + max) && b == a);
	add_get_test_do<a_, b_>::do_it();
	return true;
	}
template<typename a_, barrier_t b_> typename types::ifel_type_not<add_get_exists<a_, b_>, bool>::result add_get_test() {
	return true;
	}
	
// add1
/////////////////////////////////////////////////////////////////////

template<typename a_, barrier_t b_> struct
 add1_test_do {
	static void do_it() {}
	};
template<typename a_> struct
 add1_test_do<a_, full> {
	static a_ do_it() {
		a_ a = 0;
		add1(a);
		return a;
		}
	};

template<typename a_, barrier_t b_> typename types::ifel_type<add1_exists<a_, b_>, bool>::result add1_test() {
	a_ const
		min = numeric_limits<a_>::min(),
		max = numeric_limits<a_>::max();
	a_ a = static_cast<a_>(123);
	add1<b_>(a);
	___water_test(a == static_cast<a_>(124));
	a = static_cast<a_>(max - static_cast<a_>(1));
	add1<b_>(a);
	___water_test(a == max);
	a = min;
	add1<b_>(a);
	___water_test(a == static_cast<a_>(min + static_cast<a_>(1)));
	add1_test_do<a_, b_>::do_it();
	return true;
	}
template<typename a_, barrier_t b_> typename types::ifel_type_not<add1_exists<a_, b_>, bool>::result add1_test() {
	return true;
	}
	
// add1_get get_add1
/////////////////////////////////////////////////////////////////////

template<typename a_, barrier_t b_> struct
 add1_get_test_do {
	static void do_it() {}
	};
template<typename a_> struct
 add1_get_test_do<a_, full> {
	static a_ do_it() {
		a_ a = 0;
		a_ b = add1_get(a);
		b = get_add1(a);
		return b;
		}
	};

template<typename a_, barrier_t b_> typename types::ifel_type<add1_get_exists<a_, b_>, bool>::result add1_get_test() {
	a_ const
		min = numeric_limits<a_>::min(),
		max = numeric_limits<a_>::max();
	a_ a, b;
	// get_add1
	a = 123;
	b = get_add1<b_>(a);
	___water_test(a == static_cast<a_>(124) && b == static_cast<a_>(123));
	a = static_cast<a_>(max - static_cast<a_>(1));
	b = get_add1<b_>(a);
	___water_test(a == max && b == static_cast<a_>(max - static_cast<a_>(1)));
	a = min;
	b = get_add1<b_>(a);
	___water_test(a == static_cast<a_>(min + static_cast<a_>(1)) && b == min);
	// add1_get
	a = 123;
	b = add1_get<b_>(a);
	___water_test(a == static_cast<a_>(124) && b == a);
	a = static_cast<a_>(max - static_cast<a_>(1));
	b = add1_get<b_>(a);
	___water_test(a == max && b == a);
	a = min;
	b = add1_get<b_>(a);
	___water_test(a == static_cast<a_>(min + static_cast<a_>(1)) && b == a);
	add1_get_test_do<a_, b_>::do_it();
	return true;
	}
template<typename a_, barrier_t b_> typename types::ifel_type_not<add1_get_exists<a_, b_>, bool>::result add1_get_test() {
	return true;
	}
	
// subtract
/////////////////////////////////////////////////////////////////////

template<typename a_, barrier_t b_> struct
 subtract_test_do {
	static void do_it() {}
	};
template<typename a_> struct
 subtract_test_do<a_, full> {
	static a_ do_it() {
		a_ a = 1;
		subtract(a, 1);
		return a;
		}
	};

template<typename a_, barrier_t b_> typename types::ifel_type<subtract_exists<a_, b_>, bool>::result subtract_test() {
	a_ const
		min = numeric_limits<a_>::min(),
		max = numeric_limits<a_>::max();
	a_ a = 0;
	subtract<b_>(a, max);
	___water_test(a == static_cast<a_>(static_cast<a_>(0) - max));
	subtract<b_>(a, min);
	___water_test(a == static_cast<a_>((static_cast<a_>(0) - max) - min));
	a = min;
	subtract<b_>(a, min);
	___water_test(a == static_cast<a_>(0));
	subtract_test_do<a_, b_>::do_it();
	return true;
	}
template<typename a_, barrier_t b_> typename types::ifel_type_not<subtract_exists<a_, b_>, bool>::result subtract_test() {
	return true;
	}
	
// subtract_get get_subtract
/////////////////////////////////////////////////////////////////////

template<typename a_, barrier_t b_> struct
 subtract_get_test_do {
	static void do_it() {}
	};
template<typename a_> struct
 subtract_get_test_do<a_, full> {
	static a_ do_it() {
		a_ a = 2; 
		a_ b = subtract_get(a, 1);
		b = get_subtract(a, 1);
		return b;
		}
	};

template<typename a_, barrier_t b_> typename types::ifel_type<subtract_get_exists<a_, b_>, bool>::result subtract_get_test() {
	a_ const
		min = numeric_limits<a_>::min(),
		max = numeric_limits<a_>::max();
	a_ a, b, c;
	// get_subtract
	a = 0;
	b = get_subtract<b_>(a, max);
	c = static_cast<a_>(static_cast<a_>(0) - max);
	___water_test(a == c && b == static_cast<a_>(0));
	b = get_subtract<b_>(a, min);
	___water_test(a == static_cast<a_>(c - min) && b == c);
	a = min;
	b = get_subtract<b_>(a, min);
	___water_test(a == static_cast<a_>(0) && b == min);
	// subtract_get
	a = 0;
	b = subtract_get<b_>(a, max);
	c = static_cast<a_>(static_cast<a_>(0) - max);
	___water_test(a == c && b == a);
	b = subtract_get<b_>(a, min);
	___water_test(a == static_cast<a_>(c - min) && b == a);
	a = min;
	b = subtract_get<b_>(a, min);
	___water_test(a == static_cast<a_>(0) && b == a);
	subtract_get_test_do<a_, b_>::do_it();
	return true;
	}
template<typename a_, barrier_t b_> typename types::ifel_type_not<subtract_get_exists<a_, b_>, bool>::result subtract_get_test() {
	return true;
	}
	
// subtract1
/////////////////////////////////////////////////////////////////////

template<typename a_, barrier_t b_> struct
 subtract1_test_do {
	static void do_it() {}
	};
template<typename a_> struct
 subtract1_test_do<a_, full> {
	static a_ do_it() {
		a_ a = 1;
		subtract1(a);
		return a;
		}
	};

template<typename a_, barrier_t b_> typename types::ifel_type<subtract1_exists<a_, b_>, bool>::result subtract1_test() {
	a_ const
		min = numeric_limits<a_>::min(),
		max = numeric_limits<a_>::max();
	a_ a = static_cast<a_>(123);
	subtract1<b_>(a);
	___water_test(a == static_cast<a_>(122));
	a = max;
	subtract1<b_>(a);
	___water_test(a == static_cast<a_>(max - static_cast<a_>(1)));
	a = static_cast<a_>(min + static_cast<a_>(1));
	subtract1<b_>(a);
	___water_test(a == min);
	subtract1_test_do<a_, b_>::do_it();
	return true;
	}
template<typename a_, barrier_t b_> typename types::ifel_type_not<subtract1_exists<a_, b_>, bool>::result subtract1_test() {
	return true;
	}
	
// subtract1_get get_subtract1
/////////////////////////////////////////////////////////////////////

template<typename a_, barrier_t b_> struct
 subtract1_get_test_do {
	static void do_it() {}
	};
template<typename a_> struct
 subtract1_get_test_do<a_, full> {
	static a_ do_it() {
		a_ a = static_cast<a_>(2);
		a_ b = subtract1_get(a);
		b = get_subtract1(a);
		return b;
		}
	};

template<typename a_, barrier_t b_> typename types::ifel_type<subtract1_get_exists<a_, b_>, bool>::result subtract1_get_test() {
	a_ const
		min = numeric_limits<a_>::min(),
		max = numeric_limits<a_>::max();
	a_ a, b;
	// get_subtract1
	a = 123;
	b = get_subtract1<b_>(a);
	___water_test(a == static_cast<a_>(122) && b == static_cast<a_>(123));
	a = max;
	b = get_subtract1<b_>(a);
	___water_test(a == static_cast<a_>(max - static_cast<a_>(1)) && b == max);
	a = static_cast<a_>(min + static_cast<a_>(1));
	b = get_subtract1<b_>(a);
	___water_test(a == min && b == static_cast<a_>(min + static_cast<a_>(1)));
	// subtract1_get
	a = 123;
	b = subtract1_get<b_>(a);
	___water_test(a == static_cast<a_>(122) && b == a);
	a = max;
	b = subtract1_get<b_>(a);
	___water_test(a == static_cast<a_>(max - static_cast<a_>(1)) && b == a);
	a = static_cast<a_>(min + static_cast<a_>(1));
	b = subtract1_get<b_>(a);
	___water_test(a == min && b == a);
	subtract1_get_test_do<a_, b_>::do_it();
	return true;
	}
template<typename a_, barrier_t b_> typename types::ifel_type_not<subtract1_get_exists<a_, b_>, bool>::result subtract1_get_test() {
	return true;
	}


// test
/////////////////////////////////////////////////////////////////////

template<typename a_, barrier_t b_> void test_functions_with_type_and_barrier() {
	get_test<a_, b_>();
	set_test<a_, b_>();
	get_set_test<a_, b_>();
	compare_set_test<a_, b_>();
	compare_set_else_get_test<a_, b_>();
	compare_set_else_non_atomic_get_test<a_, b_>();
	get_compare_set_test<a_, b_>();
	add_test<a_, b_>();
	add_get_test<a_, b_>();
	add1_test<a_, b_>();
	add1_get_test<a_, b_>();
	subtract_test<a_, b_>();
	subtract_get_test<a_, b_>();
	subtract1_test<a_, b_>();
	subtract1_get_test<a_, b_>();
	}

template<typename a_> void test_functions_with_type() {
	test_functions_with_type_and_barrier<a_, none>();
	test_functions_with_type_and_barrier<a_, acquire>();
	test_functions_with_type_and_barrier<a_, release>();
	test_functions_with_type_and_barrier<a_, full>();
	}
	
template<size_t s_> typename types::ifel<sizeof(bytes<s_>) == s_, bool>::result test_functions_with_bytes() {
	test_functions_with_type<alias<bytes<s_> > >();
	return true;
	}
template<size_t s_> typename types::ifel<sizeof(bytes<s_>) != s_, bool>::result test_functions_with_bytes() {
	return true;
	}

inline void test() {
	barrier();
	test_functions_with_type<char>();
	test_functions_with_type<signed char>();
	test_functions_with_type<unsigned char>();
	test_functions_with_type<short>();
	test_functions_with_type<unsigned short>();
	test_functions_with_type<int>();
	test_functions_with_type<unsigned int>();
	test_functions_with_type<long>();
	test_functions_with_type<unsigned long>();
	test_functions_with_type<long long>();
	test_functions_with_type<unsigned long long>();
	test_functions_with_type<int*>();
	test_functions_with_type<int const volatile*>();
	test_functions_with_type<alias<bool> >();
	test_functions_with_type<alias<int*> >();
	test_functions_with_type<alias<int const volatile*> >();
	test_functions_with_type<alias<float> >();
	test_functions_with_type<alias<double> >();
	test_functions_with_bytes<sizeof(void*) * 1>();
	test_functions_with_bytes<sizeof(void*) * 2>();
	test_functions_with_bytes<sizeof(void*) * 3>();
	test_functions_with_bytes<sizeof(void*) * 4>();
	}

#ifndef WATER_ATOMIC_INLINE
}}}
#endif
#endif
