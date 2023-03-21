// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TESTS_BEGIN_END_HPP
#define WATER_TESTS_BEGIN_END_HPP
#include <water/water.hpp>
#include <water/test.hpp>
#include <water/begin_end.hpp>
#include <water/downgrade_iterators/downgrade_iterators.hpp>
namespace water { namespace tests {

/*

basic tests for water::begin_end

*/

namespace begin_end_tests {

    struct no_equal {};
    
    template<typename a_, typename b_, typename = void>
    struct can_compare_equal :
        types::false_result
    {};
    
    template<typename a_, typename b_>
    struct can_compare_equal<a_, b_, types::to_void<decltype(types::make<a_ const&>() == types::make<b_ const&>())>> :
        types::true_result
    {};
    
    static_assert(types::type_assert<can_compare_equal<
        begin_end<float const*>,
        begin_end<double*>
    >>::assert, "");
    
    
    static_assert(types::type_assert<types::nots<can_compare_equal<
        begin_end<no_equal*>,
        begin_end<char*>
    >>>::assert, "");
    
    static_assert(types::type_assert<can_compare_equal<
        begin_end<downgrade_iterators::forward<int const*>>,
        begin_end<downgrade_iterators::forward<long*>>
    >>::assert, "");
    
    static_assert(types::type_assert<types::nots<can_compare_equal<
        begin_end<downgrade_iterators::forward<no_equal*>>,
        begin_end<downgrade_iterators::forward<char*>>
    >>>::assert, "");
    
    template<typename ...a_>
    constexpr void unused(a_&&...) {}

}

inline void begin_end_all() {
    char16_t hello16[] = u"hello";
    char32_t hello32[] = U"hello";
    char16_t const not_equal [] = u"not equal";
    
    auto p1 = begin_end_from(hello16, hello16 + 6);
    auto p2 = begin_end_from(hello32);
    ___water_test(p1);
    ___water_test(!begin_end_from(hello16, hello16));
    ___water_test(p1.size() == 6 && p2.size() == 6);
    ___water_test(p1[1] == p2[1]);
    ___water_test(p1 == p2);
    ___water_test(!(p1 != p2));
    
    auto p3 = begin_end_from(not_equal, sizeof(not_equal) / sizeof(not_equal[0]));
    ___water_test(p1 != p3);
    
    auto f1 = begin_end_from(downgrade_iterators::forward_from(p1.begin()), downgrade_iterators::forward_from(p1.end()));
    auto f2 = begin_end_from(downgrade_iterators::forward_from(p2.begin()), downgrade_iterators::forward_from(p2.end()));
    auto f3 = begin_end_from(downgrade_iterators::forward_from(p3.begin()), downgrade_iterators::forward_from(p3.end()));
    
    ___water_test(f1 == f2);
    ___water_test(f2 != f3);
    
    begin_end_tests::no_equal no[3] {};
    auto n1 = begin_end_from(no);
    auto n2 = begin_end_from(no, 3);
    auto n3 = begin_end_from(no, no + 3);
    auto n4 = begin_end_from(n1.begin(), n1.end());
    auto n5 = begin_end_from(n1.begin(), n1.size());
    auto n6 = begin_end_from(downgrade_iterators::forward_from(n1.begin()), downgrade_iterators::forward_from(n1.end()));
    
    begin_end_tests::unused(n2, n3, n4, n5, n6);
}

}}
#endif
