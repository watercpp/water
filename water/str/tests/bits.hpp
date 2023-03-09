// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_TESTS_BITS_HPP
#define WATER_STR_TESTS_BITS_HPP
#include <water/str/str.hpp>
#include <water/str/bytes.hpp>
#include <water/str/hexdump.hpp>
#include <water/str/type_name.hpp>
#include <water/begin_end.hpp>
#include <water/test.hpp>
namespace water { namespace str { namespace tests {

template<typename char_>
inline char32_t equal_cast(char_ a) {
    return static_cast<char32_t>(a);
}

inline char32_t equal_cast(char a) {
    return static_cast<char32_t>(static_cast<unsigned char>(a));
}

template<typename ia_, typename ib_>
bool equal(ia_ ab, ia_ ae, ib_ bb, ib_ be) {
    auto a = ab;
    auto b = bb;
    while(a != ae && b != be && equal_cast(*a) == equal_cast(*b))
        ++a, ++b;
    if(a == ae && b == be)
        return true;
    trace << "str::tests::equal not equal:\n  A: " << begin_end_from(ab, ae) << "\n  B: " << begin_end_from(bb, be);
    return false;
}

template<typename ia_, typename ib_>
bool equal(ia_ ab, size_t as, ib_ bb, size_t bs) {
    return equal(ab, ab + as, bb, bb + bs);
}

}}}
#endif
