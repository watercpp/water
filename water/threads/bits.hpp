// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_BITS_HPP
#define WATER_THREADS_BITS_HPP
#include <water/threads/configuration.hpp>
#include <water/types.hpp>
#include <water/is_no_to.hpp>
#include <water/atomic.hpp>
#include <water/int.hpp>
#include <water/numeric_limits.hpp>
namespace water { namespace threads {

struct need_system                { static unsigned constexpr need = 1 << 0; };
struct need_water                 { static unsigned constexpr need = 1 << 1; }; // thirsty
struct need_timeout               { static unsigned constexpr need = 1 << 2; };
struct need_recursive             { static unsigned constexpr need = 1 << 3; };
struct need_constexpr_constructor { static unsigned constexpr need = 1 << 4; };
struct need_trivial_destructor    { static unsigned constexpr need = 1 << 5; };
struct need_spin                  { static unsigned constexpr need = 1 << 6; }; // selected only if specifically asked for
struct need_nothing               { static unsigned constexpr need = 0; };

namespace _ {
    
    template<unsigned n_, typename ...b_>
    struct needs_do;
    
    template<unsigned n_, typename a_, typename ...b_>
    struct needs_do<n_, a_, b_...> : needs_do<n_ | a_::need, b_...>
    {};
    
    template<unsigned n_>
    struct needs_do<n_> {
        static unsigned constexpr need = n_;
    };
}

template<typename ...needs_>
struct needs : _::needs_do<0, needs_...>
{};

template<typename ...>
struct list
{};

namespace _ {

    template<typename type_> struct
    result_type {
        using result = type_;
    };
    
    template<unsigned needs_, typename ...list_>
    struct need_select_do;
    
    template<unsigned needs_, typename type_, typename ...list_>
    struct need_select_do<needs_, type_, list_...> :
        ifel<
            type_::needs::need &&
            (needs_ & type_::needs::need) == needs_ &&
            (needs_ & need_spin::need) == (type_::needs::need & need_spin::need), // select spin variant only if asked for
            result_type<type_>,
            need_select_do<needs_, list_...>
        > {};
    
    template<unsigned need_>
    struct need_select_do<need_> :
        result_type<void>
        {};
}

template<typename needs_, typename list_>
struct need_select;

template<typename needs_, typename ...list_>
struct need_select<needs_, list<list_...>> :
    _::result_type<
        #ifndef WATER_THREADS_PREFER_WATER
        _::need_select_do<needs_::need, list_...>
        #else
        if_not_void<
            typename _::need_select_do<(needs_::need & need_system::need) ? needs_::need : (needs_::need | need_water::need), list_...>::result,
            typename _::need_select_do<needs_::need, list_...>::result
        >
        #endif
    > {};

template<typename type_> constexpr bool is_system()                 { return (type_::needs::need & need_system::need) != 0; }
template<typename type_> constexpr bool is_water()                  { return (type_::needs::need & need_water::need) != 0; }
template<typename type_> constexpr bool has_timeout()               { return (type_::needs::need & need_timeout::need) != 0; }
template<typename type_> constexpr bool is_recursive()              { return (type_::needs::need & need_recursive::need) != 0; }
template<typename type_> constexpr bool is_spin()                   { return (type_::needs::need & need_spin::need) != 0; }
template<typename type_> constexpr bool has_constexpr_constructor() { return (type_::needs::need & need_constexpr_constructor::need) != 0; }
template<typename type_> constexpr bool has_trivial_destructor()    { return (type_::needs::need & need_trivial_destructor::need) != 0; }


// define WATER_THREADS_PANIC to do something.
// it could throw an exception, terminate

inline void panic() {
    #ifdef WATER_THREADS_PANIC
    WATER_THREADS_PANIC;
    #endif
}

template<typename bool_>
void panic_if(bool_ a) {
    if(a) panic();
}

template<typename bool_>
void panic_if_not(bool_ a) {
    if(!a) panic();
}

}}
#endif
