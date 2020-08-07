// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_TRIVIAL_HPP
#define WATER_THREADS_TESTS_TRIVIAL_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/barrier.hpp>
#include <water/threads/condition.hpp>
#include <water/threads/once.hpp>
#include <water/threads/spin_once.hpp>
#include <water/threads/mutex.hpp>
#include <water/threads/read_write.hpp>
#include <water/threads/semaphore.hpp>
#include <water/type_traits.hpp>
namespace water { namespace threads { namespace tests {

/*

compile time test that types have constexpr constructors and/or trivial destructors when they say they have

*/

template<typename type_, bool = has_constexpr_constructor<type_>()>
struct trivial_test_constexpr {
    type_ my;
    constexpr trivial_test_constexpr() noexcept {}
    constexpr trivial_test_constexpr(unsigned a) noexcept : my(a) {}
};

template<typename type_>
struct trivial_test_constexpr<type_, false> {
    constexpr trivial_test_constexpr(unsigned = 0) noexcept {}
};

template<typename type_>
struct trivial_test {
    static_assert(water::has_trivial_destructor<type_>::result || !has_trivial_destructor<type_>(), "");
    trivial_test_constexpr<type_> my;
    constexpr trivial_test() noexcept {}
    constexpr trivial_test(unsigned a) noexcept : my(a) {}
};

inline void trivial() {
    trivial_test_constexpr<once, true>();
    static_assert(water::has_trivial_destructor<once>::result, "");
    trivial_test_constexpr<spin_once<>, true>();
    static_assert(water::has_trivial_destructor<spin_once<>>::result, "");
    test_list<trivial_test, barrier_list>(1);
    test_list<trivial_test, condition_list>();
    test_list<trivial_test, mutex_list>();
    test_list<trivial_test, read_write_list>();
    test_list<trivial_test, semaphore_list>(1);
}

}}}
#endif
