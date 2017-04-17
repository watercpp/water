// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ATOMIC_SYNC_TESTS_FAKE_HPP
#define WATER_ATOMIC_SYNC_TESTS_FAKE_HPP
#include <water/atomic/tests/fake/fake.hpp>
namespace water { namespace atomic { namespace sync {

namespace fake = atomic::tests::fake;

inline void __sync_synchronize() { fake::barrier(); }

template<typename a_> bool __sync_bool_compare_and_swap(a_* a, a_ b, a_ c) { return fake::compare_set<full>(a, b, c); }
template<typename a_> a_   __sync_val_compare_and_swap (a_* a, a_ b, a_ c) { return fake::get_compare_set<full>(a, b, c); }
template<typename a_> a_   __sync_fetch_and_add        (a_* a, a_ b)       { return fake::get_add<full>(a, b); }
template<typename a_> a_   __sync_fetch_and_sub        (a_* a, a_ b)       { return fake::get_subtract<full>(a, b); }
template<typename a_> a_   __sync_add_and_fetch        (a_* a, a_ b)       { return fake::add_get<full>(a, b); }
template<typename a_> a_   __sync_sub_and_fetch        (a_* a, a_ b)       { return fake::subtract_get<full>(a, b); }
template<typename a_> a_   __sync_lock_test_and_set    (a_* a, a_ b)       { return fake::get_set<acquire>(a, b); }
template<typename a_> a_   __sync_swap                 (a_* a, a_ b)       { return fake::get_set<full>(a, b); }

}}}
#endif
