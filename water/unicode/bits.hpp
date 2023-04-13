// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_UNICODE_BITS_HPP
#define WATER_UNICODE_BITS_HPP
#include <water/water.hpp>
#include <water/types.hpp>
#include <water/is_no_to.hpp>
#include <water/iterator.hpp>
#include <water/char8.hpp>
#include <water/begin_end.hpp>
namespace water { namespace unicode {


using uchar_t = unsigned char;


template<typename char_>
constexpr char32_t cast(char_ a)        { return static_cast<char32_t>(a); }
constexpr char16_t cast(char16_t a)     { return a; }
constexpr uchar_t  cast(uchar_t a)      { return a; }
constexpr uchar_t  cast(char a)         { return static_cast<uchar_t>(a); }
constexpr uchar_t  cast(signed char a)  { return static_cast<uchar_t>(a); }
constexpr uchar_t  cast(char8_or_not a) { return static_cast<uchar_t>(a); } // the normal utf-8 type for this namespace is still unsigned char


}}
#endif
