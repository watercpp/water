// Copyright 2017-2024 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_OUT_STDOUT_HPP
#define WATER_STR_OUT_STDOUT_HPP
#include <water/str/buffer.hpp>
#include <water/str/buffer_lines.hpp>
#ifdef WATER_NO_CHEADERS
    #include <stdio.h>
#else
    #include <cstdio>
#endif
namespace water { namespace str {

/*

Easy way to write to stdout.

str::out_stdout{} << "hello world!";

Note that this will add a newline by itself, and cut off long lines, see buffer_lines.hpp

If the program is writing to stdout using a single str::out object at a time, from a single thread,
and it needs to write long lines use:

str::out_stdout_simple_buffer{} << "hello world\n";

This will not add newlines by itself or cut off long lines, see buffer.hpp

*/

struct write_to_stdout
{
    void operator()(char const* a) const {
        #ifdef WATER_NO_CHEADERS
        fputs(a, stdout);
        #else
        std::fputs(a, stdout); // stdout is a macro
        #endif
    }
};

using out_stdout = out<buffer_lines<write_to_stdout>>;

using out_stdout_simple_buffer = out<buffer<write_to_stdout>>;

constexpr create<out_stdout> sout;

}}
#endif
