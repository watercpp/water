// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES1_TO_VOID_HPP
#define WATER_TYPES1_TO_VOID_HPP
namespace water { namespace types1 {

/*

to_void always resutls void. It can be very useful to detetect if a type can do specific things:

template<typename type_, typename void_ = void>
struct has_some_type :
    types1::false_result
{};

template<typename type_>
struct has_some_type<type_, types1::to_void<typename type_::some_type>>  :
    types1::true_result
{};

The second specialization is selected when type_ has some_type since it is more specialized.

Together with decltype and types1::make it is possible to test expressions, in this example if the
type_ has begin() and end():

template<typename type_, typename void_ = void>
struct has_begin_end :
    types1::false_result
{};

template<typename type_>
struct has_begin_end<type_, types1::to_void<decltype(types1::make<type_&>().begin() != types1::make<type_&>().end())>>  :
    types1::true_result
{};

*/

namespace _ {

    // this is needed so the template arguments of to_void are used
    template<typename ...>
    struct to_void_do {
        using result = void;
    };
    
}

template<typename ...types_> using to_void = typename _::to_void_do<types_...>::result;

}}
#endif
