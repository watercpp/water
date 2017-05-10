// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_OLD_TESTS_FAKE_FAKE_HPP
#define WATER_ATOMIC_OLD_TESTS_FAKE_FAKE_HPP
#include <water/atomic_old/atomic.hpp>
#ifndef WATER_ATOMIC_OLD_TESTS_FAKE_NO_MUTEX
	#include <water/threads/mutex.hpp>
#endif
namespace water { namespace atomic_old { namespace tests { namespace fake {

/*

make fake atomic functions for any type by
- using water::atomic_old if possible
- locking a global threads::mutex
- no lock

then it is possible to test underlying implementations even if they dont exist

*/

#ifdef WATER_ATOMIC_OLD_TESTS_FAKE_NO_MUTEX

template<typename mutex_ = void> struct
 lock;

#else

template<typename mutex_ = threads::mutex<threads::need_constexpr_constructor>> struct
 lock {
 	static mutex_ mutex;
 	lock() {
		mutex.lock();
		}
	~lock() {
		mutex.unlock();
		}
 	};
	template<typename mutex_> mutex_ lock<mutex_>::mutex {};

#endif

template<> struct
 lock<void> {
 	lock() {}
	~lock() {} // prevents unused warning?
 	};
	 
inline void barrier() {
	#ifndef WATER_NO_ATOMIC
	atomic_old::barrier();
	#else
	lock<> l;
	#endif
	}
	
template<barrier_t b_, typename a_> typename types::ifel_type<atomic_old::get_exists<a_, b_>, a_>::result
 get(a_ *a) {
	return atomic_old::get<b_>(*a);
	}
template<barrier_t b_, typename a_> typename types::ifel_type_not<atomic_old::get_exists<a_, b_>, a_>::result
 get(a_ *a) {
	lock<> l;
	return *a;
	}
	
template<barrier_t b_, typename a_> void
 set(a_ *a, typename types::ifel_type<atomic_old::set_exists<a_, b_>, a_>::result b) {
	atomic_old::set<b_>(*a, b);
	}
template<barrier_t b_, typename a_> void
 set(a_ *a, typename types::ifel_type_not<atomic_old::set_exists<a_, b_>, a_>::result b) {
	lock<> l;
	*a = b;
	}

template<barrier_t b_, typename a_> typename types::ifel_type<atomic_old::get_set_exists<a_, b_>, a_>::result
 get_set(a_ *a, a_ b) {
	return atomic_old::get_set<b_>(*a, b);
	}
template<barrier_t b_, typename a_> typename types::ifel_type_not<atomic_old::get_set_exists<a_, b_>, a_>::result
 get_set(a_ *a, a_ b) {
	lock<> l;
	a_ r = *a;
	*a = b;
	return r;
	}

template<barrier_t b_, typename a_> typename types::ifel_type<atomic_old::compare_set_exists<a_, b_>, bool>::result
 compare_set(a_ *a, a_ b, a_ c) {
	return atomic_old::compare_set<b_>(*a, b, c);
	}
template<barrier_t b_, typename a_> typename types::ifel_type_not<atomic_old::compare_set_exists<a_, b_>, bool>::result
 compare_set(a_ *a, a_ b, a_ c) {
	lock<> l;
	if(*a == b) {
		*a = c;
		return true;
		}
	return false;
	}

template<barrier_t b_, typename a_> typename types::ifel_type<atomic_old::get_compare_set_exists<a_, b_>, a_>::result
 get_compare_set(a_ *a, a_ b, a_ c) {
	return atomic_old::get_compare_set<b_>(*a, b, c);
	}
template<barrier_t b_, typename a_> typename types::ifel_type_not<atomic_old::get_compare_set_exists<a_, b_>, a_>::result
 get_compare_set(a_ *a, a_ b, a_ c) {
	lock<> l;
	a_ r = *a;
	if(*a == b) *a = c;
	return r;
	}

template<barrier_t b_, typename a_> typename types::ifel_type<atomic_old::add_get_exists<a_, b_>, a_>::result
 add_get(a_ *a, a_ b) {
	return atomic_old::add_get<b_>(*a, b);
	}
template<barrier_t b_, typename a_> typename types::ifel_type_not<atomic_old::add_get_exists<a_, b_>, a_>::result
 add_get(a_ *a, a_ b) {
	lock<> l;
	return *a = static_cast<a_>(*a + b); // avoid warning
	}
template<barrier_t b_, typename a_> a_
 get_add(a_* a, a_ b) {
	return fake::add_get<b_>(a, b) - b;
	}
template<barrier_t b_, typename a_> void
 add(a_* a, a_ b) {
	fake::add_get<b_>(a, b);
	}
	
template<barrier_t b_, typename a_> typename types::ifel_type<atomic_old::add1_get_exists<a_, b_>, a_>::result
 add1_get(a_ *a) {
	return atomic_old::add1_get<b_>(*a);
	}
template<barrier_t b_, typename a_> typename types::ifel_type_not<atomic_old::add1_get_exists<a_, b_>, a_>::result
 add1_get(a_ *a) {
	lock<> l;
	return ++*a;
	}
template<barrier_t b_, typename a_> a_
 get_add1(a_* a) {
	return fake::add1_get<b_>(a) - 1;
	}
template<barrier_t b_, typename a_> void
 add1(a_* a) {
	fake::add1_get<b_>(a);
	}
	
template<barrier_t b_, typename a_> typename types::ifel_type<atomic_old::subtract_get_exists<a_, b_>, a_>::result
 subtract_get(a_ *a, a_ b) {
	return atomic_old::subtract_get<b_>(*a, b);
	}
template<barrier_t b_, typename a_> typename types::ifel_type_not<atomic_old::subtract_get_exists<a_, b_>, a_>::result
 subtract_get(a_ *a, a_ b) {
	lock<> l;
	return *a = static_cast<a_>(*a - b); // avoid warning
	}
template<barrier_t b_, typename a_> a_
 get_subtract(a_* a, a_ b) {
	return fake::subtract_get<b_>(a, b) + b;
	}
template<barrier_t b_, typename a_> void
 subtract(a_* a, a_ b) {
	fake::subtract_get<b_>(a, b);
	}
	
template<barrier_t b_, typename a_> typename types::ifel_type<atomic_old::subtract1_get_exists<a_, b_>, a_>::result
 subtract1_get(a_ *a) {
	return atomic_old::subtract1_get<b_>(*a);
	}
template<barrier_t b_, typename a_> typename types::ifel_type_not<atomic_old::subtract1_get_exists<a_, b_>, a_>::result
 subtract1_get(a_ *a) {
	lock<> l;
	return --*a;
	}
template<barrier_t b_, typename a_> a_
 get_subtract1(a_* a) {
	return fake::subtract1_get<b_>(a) + 1;
	}
template<barrier_t b_, typename a_> void
 subtract1(a_* a) {
	fake::subtract1_get<b_>(a);
	}

}}}}
#endif
