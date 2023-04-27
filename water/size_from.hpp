// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_SIZE_FROM_HPP
#define WATER_SIZE_FROM_HPP
#include <water/water.hpp>
#include <water/types.hpp>
namespace water {

/*

Get the size from something:

- If its is an array, the array size
- something.size() if it exists and converts to size_t
- something.end() - something.begin() if it exists and converts to size_t
- loop over something.begin() ... something.end() as a last resort

If the size is negative, this returns 0.

It expects someting.begin() and something.end() to be at least forward iterators.

The code using this probably expects that something.size() is the size of the range someting.begin() ... something.end()

*/

template<typename begin_, typename end_, typename = void>
bool constexpr size_from_minus = false;

template<typename begin_, typename end_>
bool constexpr size_from_minus<begin_, end_, to_void<decltype(static_cast<size_t>(make_type<begin_&>() - make_type<end_&>()))>> = true;


template<typename type_, typename = void>
bool constexpr size_from_size = false;

template<typename type_>
bool constexpr size_from_size<type_, to_void<decltype(static_cast<size_t>(make_type<type_&>().size()))>> = true;


template<typename int_, typename = void>
bool constexpr size_from_signed_cast = false;

template<typename int_>
bool constexpr size_from_signed_cast<int_, ifel<(static_cast<int_>(-1) < static_cast<int_>(0)), void>> = true;


template<typename int_>
constexpr auto size_from_cast(int_ a) -> ifel<size_from_signed_cast<int_>, size_t> {
    return a < static_cast<int_>(0) ? static_cast<size_t>(0) : static_cast<size_t>(a);
}

template<typename int_>
constexpr auto size_from_cast(int_ a) -> ifel<!size_from_signed_cast<int_>, size_t> {
    return static_cast<size_t>(a);
}


template<typename begin_, typename end_>
constexpr auto size_from(begin_ begin, end_ end) -> ifel<size_from_minus<begin_, end_>, size_t> {
    return size_from_cast(end - begin);
}

template<typename begin_, typename end_>
auto size_from(begin_ begin, end_ end) -> ifel<!size_from_minus<begin_, end_>, size_t> {
    size_t r = 0;
    while(begin != end) {
        ++begin;
        ++r;
    }
    return r;
}


template<typename type_>
constexpr auto size_from(type_&& a) -> ifel<size_from_size<type_>, size_t>  {
    return size_from_cast(a.size());
}

template<typename type_>
constexpr auto size_from(type_&& a) -> ifel<!size_from_size<type_>, first<size_t, decltype(a.begin() != a.end()), decltype(*a.begin())>> {
    return size_from(a.begin(), a.end());
}

template<typename type_, size_t size_>
constexpr size_t size_from(type_ const (&)[size_]) {
    return size_;
}


}
#endif
