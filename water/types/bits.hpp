// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_BITS_HPP
#define WATER_TYPES_BITS_HPP
namespace water { namespace types {

template<typename result_> using result = typename result_::result;

template<typename result_>
constexpr typename result_::type value = result_::result;

// forward
template<typename kind_ = void> struct result_tag;
struct nontype_kind;
struct type_kind;

using nontype_tag = result_tag<nontype_kind>;
using type_tag = result_tag<type_kind>;

template<bool result_>
struct bool_result {
    using result_tag = types::result_tag<nontype_kind>;
    using type = bool;
    static bool constexpr result = result_;
};

using false_result = bool_result<false>;
using true_result = bool_result<true>;


template<typename a_, typename b_>
struct equal_plain :
    false_result
{};

template<typename a_>
struct equal_plain<a_, a_> :
    true_result
{};


namespace _ {

    // this is needed so the template arguments of to_void are used
    template<typename ...>
    struct to_void_do {
        using result = void;
    };
    
}

template<typename ...types_> using to_void = typename _::to_void_do<types_...>::result;


struct nothing;

}}
#endif
