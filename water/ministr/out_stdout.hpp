// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MINISTR_OUT_STDOUT_HPP
#define WATER_MINISTR_OUT_STDOUT_HPP
#include <water/ministr/ministr.hpp>
#ifdef WATER_NO_CHEADERS
    #include <stdio.h>
#else
    #include <cstdio>
#endif
namespace water { namespace ministr {

/*

Easy way to write to stdout.

ministr::out_stdout{} << "hello world!";

This will add a newline to the end by itself.

*/

struct write_to_stdout {
    void operator()(char const* cstring, char const*) const { // always 0-terminated cstring from ministr::out
        #ifdef WATER_NO_CHEADERS
        fputs(cstring, stdout);
        #else
        std::fputs(cstring, stdout); // stdout is a macro
        #endif
    }
};

using out_stdout = out<
    write_to_stdout,
    ministr::configuration<
        typename ministr::configuration_default::number_format,
        ministr::settings_end<typename ministr::configuration_default::settings, '\n'>
    >
>;

}}
#endif
