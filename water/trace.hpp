// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#include <water/water.hpp>
// water.hpp included outside the header guard macro, otherwise a program that inlcudes only
// this file with WATER_DEBUG defined will fail in macros.hpp that is included from water.hpp
#ifndef WATER_TRACE_HPP
#define WATER_TRACE_HPP
#include <water/xtr/base.hpp>
#ifndef WATER_TRACE
    #if defined(WATER_SYSTEM_WINDOWS) && !defined(WATER_COMPILER_GCC)
        #ifdef WATER_TRACE_NO_UTF16
            #include <water/windows.hpp>
            namespace water { namespace _ { namespace trace_hpp {
                WATER_WINDOWS_FUNCTION(void, OutputDebugStringA, (char const*));
        }}}
        #else
            #include <water/windows.hpp>
            #include <water/unicode/utf_iterators.hpp>
            namespace water { namespace _ { namespace trace_hpp {
                WATER_WINDOWS_FUNCTION(void, OutputDebugStringW, (wchar_t const*));
            }}}
        #endif
    #elif defined(WATER_NO_CHEADERS)
        #include <stdio.h>
    #else
        #include <cstdio>
    #endif
#endif
namespace water {

/*

water::trace("hello world\n");
water::trace << "hello world";

water::trace writes the string to stdout except on windows where it writes to OutputDebugString. The
second variant is a water::xtr, it will append a linebreak automatically.

This is used by water when WATER_DEBUG is defined.

You can replace it by defining WATER_TRACE(text) to do something else. Example:
#define WATER_TRACE(text) __android_log_write(ANDROID_LOG_INFO, "my_application", text)

*/

class trace_class
{
public:
    void operator()(char const* c) const noexcept {
        if(c && *c) {
            #if defined(WATER_TRACE)
            WATER_TRACE(c);
            #elif defined(WATER_SYSTEM_WINDOWS) && !defined(WATER_COMPILER_GCC) && defined(WATER_TRACE_NO_UTF16)
            _::trace_hpp::OutputDebugStringA(c);
            #elif defined(WATER_SYSTEM_WINDOWS) && !defined(WATER_COMPILER_GCC)
            auto i = unicode::utf_iterator_from_cstring<16>(c);
            constexpr unsigned size = 4096;
            wchar_t w[size], x = 0;
            while(i) {
                unsigned s = 0;
                if(x) w[s++] = x;
                do w[s++] = *i; while(++i && s != size - 1);
                if(unicode::utf16_is_1_of_2(w[s - 1])) x = w[--s];
                else x = 0;
                w[s] = 0;
                if(s) _::trace_hpp::OutputDebugStringW(w);
            }
            #elif defined(WATER_NO_CHEADERS)
            fputs(c, stdout);
            #else
            std::fputs(c, stdout); // stdout is a macro
            #endif
        }
    }
};

constexpr trace_class trace;

using trace_expression = xtr::expression<
    trace_class,
    xtr::configuration<
        xtr::configuration_default::number_format,
        xtr::settings_end<xtr::configuration_default::settings, '\n'>
    >
>;

template<typename type_>
auto operator<<(trace_class, type_&& a) noexcept -> decltype(trace_expression{} << a) {
    return trace_expression{} << a;
}

}
#endif
