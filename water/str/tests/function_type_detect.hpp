// Copyright 2017-2023 Johan Paulsson
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

    template<typename type_>
    struct result_type {
        using result = type_;
    };

    template<typename function_, typename = void>
    struct detect1 {};
    template<typename function_>
    struct detect1<
        function_,
        to_void<decltype(make_type<function_&>()(static_cast<char const*>(0), static_cast<char const*>(0)))>
    > : result_type<char>
    {};

    template<typename function_, typename = void>
    struct detect2_16 {};
    template<typename function_>
    struct detect2_16<
        function_,
        to_void<decltype(make_type<function_&>()(make_type<char16_t const*>(), make_type<char16_t const*>()))>
    > : result_type<char16_t>
    {};
    
    template<typename function_, typename = void>
    struct detect2 : detect2_16<function_> {};
    template<typename function_>
    struct detect2<
        function_,
        to_void<decltype(make_type<function_&>()(make_type<char const*>(), make_type<char const*>()))>
    > : result_type<char>
    {};

    template<typename function_, typename = void, typename = void>
    struct detect3 {};
    template<typename function_>
    struct detect3<
        function_,
        to_void<decltype(make_type<function_&>()(make_type<char const*>(), make_type<char const*>()))>,
        void
    > : result_type<char>
    {};
    
    template<typename function_>
    struct detect3<
        function_,
        void,
        to_void<decltype(make_type<function_&>()(make_type<char16_t const*>(), make_type<char16_t const*>()))>
    > : result_type<char16_t>
    {};

    template<typename function_, typename = void>
    struct detect4_char :
        result_type<void>
    {};
    
    template<typename function_>
    struct detect4_char<function_, to_void<decltype(make_type<function_&>()(make_type<char const*>(), make_type<char const*>()))>> :
        result_type<char>
    {};

    template<typename function_, typename = void>
    struct detect4_char16 :
        result_type<void>
    {};
    
    template<typename function_>
    struct detect4_char16<function_, to_void<decltype(make_type<function_&>()(make_type<char16_t const*>(), make_type<char16_t const*>()))>> :
        result_type<char16_t>
    {};

    template<typename function_, typename char_ = typename detect4_char<function_>::result, typename char16_ = typename detect4_char16<function_>::result>
    struct detect4
    {};
    
    template<typename function_>
    struct detect4<function_, char, void> : result_type<char>
    {};
    
    template<typename function_>
    struct detect4<function_, void, char16_t> : result_type<char16_t>
    {};

    template<typename function_>
    struct detect5 :
        ifel<!water::equal<void, typename detect4_char<function_>::result>, char,
        ifel<!water::equal<void, typename detect4_char16<function_>::result>, char16_t
        >> {};

    template<typename function_> void test6(...);
    template<typename function_> char test6(function_ *f, decltype((*f)(static_cast<char const*>(0), static_cast<char const*>(0)))* = 0);
    template<typename function_> char16_t test6(function_ *f, decltype((*f)(static_cast<char16_t const*>(0), static_cast<char16_t const*>(0)))* = 0);

    template<typename function_, typename char_ = decltype(test6<no_reference<function_>>(0))>
    struct detect6 : result_type<char_>
    {};
    
    template<typename function_>
    struct detect6<function_, void>
    {};
    
    template<typename char_>
    struct function {
        void operator()(char_ const*, char_ const*) {}
    };

    template<typename function_, typename char_ = typename detect6<function_>::result>
    struct create_return {
        explicit operator bool() const { return true; }
    };

    template<typename function_>
    create_return<function_> create(function_&&) {
        return {};
    }
}

template<typename char_, typename function1_, typename function2_>
bool function_type_detect(function1_&& f1, function2_&& f2) {
    using namespace function_type_detects;
    using char1 = typename detect6<function1_>::result;
    static_assert(water::equal<char_, char1>, "oh no!");
    auto r1a = create(f1);
    auto r1b = create(function1_(f1));

    using char2 = typename detect6<function2_>::result;
    static_assert(water::equal<char_, char2>, "oh no!");
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
