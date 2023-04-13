// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XTR_TESTS_BITS_HPP
#define WATER_XTR_TESTS_BITS_HPP
#include <water/water.hpp>
#include <water/test.hpp>
#include <water/trace.hpp>
#include <water/types.hpp>
#include <water/xtr/xtr.hpp>
#include <water/cstring.hpp>
#include <water/begin_end.hpp>
#include <water/char8.hpp>
namespace water { namespace xtr { namespace tests {

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
    trace << "xtr::tests::equal not equal:\n  A: " << begin_end_from(ab, ae) << "\n  B: " << begin_end_from(bb, be);
    return false;
}

template<typename a_, typename b_, typename char_, size_t s_>
bool equal(expression<a_, b_>&& x, char_ const(&c)[s_]) {
    auto xs = string(x);
    auto cs = begin_end_from(c, s_ - (c[s_ - 1] ? 0 : 1));
    return equal(xs.begin(), xs.end(), cs.begin(), cs.end());
}

template<typename a_, typename b_, typename range_>
auto equal(expression<a_, b_>&& x, range_&& r) -> decltype(static_cast<bool>(r.begin() == r.end())) {
    auto xs = string(x);
    return equal(xs.begin(), xs.end(), r.begin(), r.end());
}

}}}
#endif
