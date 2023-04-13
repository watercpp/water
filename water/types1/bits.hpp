// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES1_BITS_HPP
#define WATER_TYPES1_BITS_HPP
namespace water { namespace types1 {

using size_t = decltype(sizeof(0));
using ptrdiff_t = decltype(static_cast<int*>(0) - static_cast<int*>(0));

template<typename type_> type_ make();

template<typename result_> using result = typename result_::result;

//forward
template<typename kind_ = void> struct result_tag;
struct integer_kind;
struct type_kind;
struct nothing;

typedef bool true_size;
typedef bool (&false_size)[2];

// namespace _ {
//   template<typename a_> types1::true_size test_some_type(typename a_::some_type*);
//   template<typename a_> types1::false_size test_some_type(...);
//   }
// template<typename a_> struct
//  has_some_type :
//   types1::bool_result<
//     sizeof(_::test_some_type<typename types1::type<a_>::result>(0)) == sizeof(true)
//     > {};
//
// to make it compile on more compilers and witout warnings:
// - have test-functions outside the result struct
// - always qualify test-function with namespace
// - do not use the sizeof(test_func<...>()) as a default template argument
// - do not pass non-pod types to ...



// tag_overload_is_exact, workaround for codewarrior probably not needed any more
namespace _ {

    struct do_tag_overload_tag;
    
    struct do_test_tag_overload { typedef void tag; };
    
    template<typename t_ = do_test_tag_overload, typename tag_ = do_tag_overload_tag>
    struct do_tag_overload_is_exact {
        enum type { result = true };
    };
    
    template<typename t_>
    struct do_tag_overload_is_exact<t_, typename t_::tag> {
        enum type { result = false };
    };
}

bool constexpr tag_overload_is_exact = _::do_tag_overload_is_exact<>::result;

}}
#endif
