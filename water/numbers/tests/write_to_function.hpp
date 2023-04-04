// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_NUMBERS_TESTS_WRITE_TO_FUNCTION_HPP
#define WATER_NUMBERS_TESTS_WRITE_TO_FUNCTION_HPP
#include <water/numbers/tests/bits.hpp>
namespace water { namespace numbers { namespace tests {

/*

test that write.buffered(function) works with different types of functions

*/

using write_to_function_type_t = void(*)();

template<typename type_> void write_to_function_type_id() {}

template<typename type_>
constexpr write_to_function_type_t write_to_function_type() {
    return &write_to_function_type_id<type_>;
}

template<typename type_>
constexpr write_to_function_type_t write_to_function_type(type_ const&) {
    return &write_to_function_type_id<type_>;
}

inline void write_to_plain_function(char16_t const*, char16_t const*) {}

struct write_to_function_template {
    write_to_function_type_t *type;
    template<typename iterator_> void operator()(iterator_ a, iterator_) {
        *type = write_to_function_type(*a);
    }
};

struct write_to_function_overloaded {
    write_to_function_type_t *type;
    void operator()(char const*,     char const*    ) { *type = write_to_function_type<char>(); }
    void operator()(char16_t const*, char16_t const*) { *type = write_to_function_type<char16_t>(); }
    void operator()(char32_t const*, char32_t const*) { *type = write_to_function_type<char32_t>(); }
    void operator()(wchar_t const*,  wchar_t const* ) { *type = write_to_function_type<wchar_t>(); }
    void operator()(int const*,      int const*     ) { *type = write_to_function_type<int>(); }
    void operator()(void const*,     void const*    ) { *type = write_to_function_type<void>(); }
};

inline void write_to_function_all() {
    write<int> w0{0};
    
    write_to_function_type_t t = 0;
    
    w0.buffered(write_to_plain_function);
    
    w0.buffered(write_to_function_template{&t});
    ___water_test(t == write_to_function_type<char>());
    
    w0.buffered(write_to_function_overloaded{&t});
    ___water_test(t == write_to_function_type<char>());
    
    w0.buffered([&t](auto b, auto) {
        t = write_to_function_type(*b);
    });
    ___water_test(t == write_to_function_type<char>());
    
    w0.buffered([](char8_or_char const*, char8_or_char const*) {});
    w0.buffered([](char16_t const*, char16_t const*) {});
    w0.buffered([](char32_t const*, char32_t const*) {});
    w0.buffered([](wchar_t const*, wchar_t const*) {});
    w0.buffered([](unsigned char const*, unsigned char const*) {});
    
    write<int, 16> w16{0};
    
    w16.buffered(write_to_plain_function);
    
    w16.buffered(write_to_function_template{&t});
    ___water_test(t == write_to_function_type<char16_t>());
    
    w16.buffered(write_to_function_overloaded{&t});
    ___water_test(t == write_to_function_type<char16_t>());
    
    w16.buffered([&t](auto b, auto) {
        t = write_to_function_type(*b);
    });
    ___water_test(t == write_to_function_type<char16_t>());
    
    w16.buffered([](char16_t const*, char16_t const*) {});
    
    write<int, 8> w8{0};
    
    w8.buffered(write_to_function_template{&t});
    ___water_test(t == write_to_function_type<char>());
    
    w8.buffered(write_to_function_overloaded{&t});
    ___water_test(t == write_to_function_type<char>());
    
    w8.buffered([&t](auto b, auto) {
        t = write_to_function_type(*b);
    });
    ___water_test(t == write_to_function_type<char>());
    
    w8.buffered([](char const*, char const*) {});
}

}}}
#endif
