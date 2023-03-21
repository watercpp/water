// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XTR_TESTS_INTEGERS_HPP
#define WATER_XTR_TESTS_INTEGERS_HPP
#include <water/xtr/tests/bits.hpp>
#include <water/int.hpp>
namespace water { namespace xtr { namespace tests {

inline void integers() {
    
    ___water_test(equal(no
        << true << ' ' << false
        << uppercase << ' ' << true << ' ' << false
        << lowercase << ' ' << true << ' ' << false,
        "true false TRUE FALSE true false"
    ));
    
    ___water_test(equal(no
        << static_cast<unsigned short>(1) << ' ' << static_cast<short>(-1) << ' '
        << 1u << ' ' << -1 << ' '
        << 1ul << ' ' << -1l << ' '
        << 1ull << ' ' << -1ll, 
        "1 -1 1 -1 1 -1 1 -1"
    ));
    
    ___water_test(equal(no << static_cast<int const*>(0), "0x0"));
    ___water_test(equal(no << reinterpret_cast<void*>(static_cast<size_t>(0xDEAD00)), "0xDEAD00"));
    
    // to make this test work on something with signed magnitude 43 bit int:
    
    using i16 = int_bits_at_least<16>;
    using u16 = uint_bits_at_least<16>;
    i16 constexpr i16_min = -32768;
    i16 constexpr i16_max = 32767;
    u16 constexpr u16_max = 65535u;
    
    ___water_test(equal(
        no << i16_min << ' ' << i16_max << ' ' << u16_max,
        "-32768 32767 65535"
    ));
    ___water_test(equal(
        no << base<10> << i16_min << ' ' << i16_max << ' ' << u16_max,
        "-32768 32767 65535"
    ));
    ___water_test(equal(no
        << base<2> << i16_min << ' ' << i16_max << ' ' << u16_max,
        "-0b1000000000000000 0b111111111111111 0b1111111111111111"
    ));
    ___water_test(equal(no
        << base<8> << i16_min << ' ' << i16_max << ' ' << u16_max,
        "-0o100000 0o77777 0o177777"
    ));
    ___water_test(equal(no
        << base<16> << i16_min << ' ' << i16_max << ' ' << u16_max,
        "-0x8000 0x7FFF 0xFFFF"
    ));
    
    using i32 = int_bits_at_least<32>;
    using u32 = uint_bits_at_least<32>;
    i32 constexpr i32_min = static_cast<i32>(-2147483647l - 1);
    i32 constexpr i32_max = static_cast<i32>(2147483647l);
    u32 constexpr u32_max = static_cast<u32>(4294967295u);
    
    ___water_test(equal(
        no << i32_min << ' ' << i32_max << ' ' << u32_max,
        "-2147483648 2147483647 4294967295"
    ));
    ___water_test(equal(
        no << base<10> << i32_min << ' ' << i32_max << ' ' << u32_max,
        "-2147483648 2147483647 4294967295"
    ));
    ___water_test(equal(no
        << base<2> << i32_min << ' ' << i32_max << ' ' << u32_max,
        "-0b10000000000000000000000000000000 0b1111111111111111111111111111111 0b11111111111111111111111111111111"
    ));
    ___water_test(equal(no
        << base<8> << i32_min << ' ' << i32_max << ' ' << u32_max,
        "-0o20000000000 0o17777777777 0o37777777777"
    ));
    ___water_test(equal(no
        << base<16> << i32_min << ' ' << i32_max << ' ' << u32_max,
        "-0x80000000 0x7FFFFFFF 0xFFFFFFFF"
    ));
    
    using i64 = int_bits_at_least<64>;
    using u64 = uint_bits_at_least<64>;
    i64 constexpr i64_min = static_cast<i64>(-9223372036854775807ll - 1); // avoid warning
    i64 constexpr i64_max = static_cast<i64>(9223372036854775807ll);
    u64 constexpr u64_max = static_cast<u64>(18446744073709551615ull); // that is a lot
    
    ___water_test(equal(
        no << i64_min << ' ' << i64_max << ' ' << u64_max,
        "-9223372036854775808 9223372036854775807 18446744073709551615"
    ));
    ___water_test(equal(
        no << base<10> << i64_min << ' ' << i64_max << ' ' << u64_max,
        "-9223372036854775808 9223372036854775807 18446744073709551615"
    ));
    ___water_test(equal(no
        << base<2> << i64_min << ' ' << i64_max << ' ' << u64_max,
        "-0b1000000000000000000000000000000000000000000000000000000000000000 0b111111111111111111111111111111111111111111111111111111111111111 0b1111111111111111111111111111111111111111111111111111111111111111"
    ));
    ___water_test(equal(no
        << base<8> << i64_min << ' ' << i64_max << ' ' << u64_max,
        "-0o1000000000000000000000 0o777777777777777777777 0o1777777777777777777777"
    ));
    ___water_test(equal(no
        << base<16> << i64_min << ' ' << i64_max << ' ' << u64_max,
        "-0x8000000000000000 0x7FFFFFFFFFFFFFFF 0xFFFFFFFFFFFFFFFF"
    ));
    
    // base, digits, uppercase, lowercase, show_plus, hide_plus, show_base, hide_base
    
    ___water_test(equal(
        no << digits<4> << 123456 << ' ' << -123456 << ' ' << 1 << ' ' << -9 << ' ' << base<16> << 0x123456 << ' ' << -0x123456 << ' ' << 0x1 << ' ' << 0xF,
        "3456 -3456 0001 -0009 0x3456 -0x3456 0x0001 0x000F"
    ));
    
    ___water_test(equal(
        no << base<16> << uppercase << 0xABCDEF << ' ' << lowercase << 0xABCDEF << ' ' << base<2> << 1 << ' ' << uppercase << 0 << ' ' << digits<5> << 0xaa << ' ' << lowercase << 0x55,
        "0XABCDEF 0xabcdef 0b1 0B0 0B01010 0b10101"
    ));
    
    ___water_test(equal(
        no << show_plus << 123 << ' ' << -123 << ' ' << base<8> << 7 << ' ' << -7 << ' ' << hide_plus << base<10> << 123 << ' ' << -123 << ' ' << base<8> << 7 << ' ' << -7,
        "+123 -123 +0o7 -0o7 123 -123 0o7 -0o7"
    ));
    
    ___water_test(equal(
        no << hide_base << base<2> << 5 << ' ' << base<8> << 5 << ' ' << base<16> << 0xF << ' ' << show_base << base<2> << 5 << ' ' << base<8> << 5 << ' ' << base<16> << 0xF,
        "101 5 F 0b101 0o5 0xF"
    ));
    
    ___water_test(equal(
        no << digits<8> << lowercase << hide_base << show_plus << base<2> << 5 << ' ' << base<8> << 5 << ' ' << base<16> << 0xF,
        "+00000101 +00000005 +0000000f"
    ));
    
    ___water_test(equal(
        no << binary << 1 << ' ' << octal << 1 << ' ' << decimal << 1 << ' ' << hex << 1,
        "0b1 0o1 1 0x1"
    ));
    
}

}}}
#endif
