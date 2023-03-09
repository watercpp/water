// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XTR_TESTS_XTR_OPERATOR_USING_STR_HPP
#define WATER_XTR_TESTS_XTR_OPERATOR_USING_STR_HPP
#include <water/xtr/tests/bits.hpp>
#include <water/xtr_operator_using_str.hpp>
namespace water_tests { namespace xtr {

    // the example from water/xtr_operator_using_str.hpp

    template<typename>
    struct something {};

    template<typename o_, typename s_>
    water::str::out<o_>& operator<<(water::str::out<o_>& o, something<s_>) {
        return o << "something";
    }

    template<typename a_, typename b_, typename s_>
    water::xtr::expression<
        water::xtr::expression<a_, b_>,
        water::xtr_operator_using_str<something<s_>>
    >
    operator<<(water::xtr::expression<a_, b_>&& e, something<s_> const& s) {
        return {e, s};
    }


}}

namespace water { namespace xtr { namespace tests {

    inline void xtr_operator_using_str_test() {
        ___water_test(equal(xtr::no << water_tests::xtr::something<void>{}, "something"));
    }

}}}
#endif
