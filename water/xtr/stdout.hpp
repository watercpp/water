// Copyright 2024 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XTR_STDOUT_HPP
#define WATER_XTR_STDOUT_HPP
#include <water/xtr/xtr.hpp>
#ifdef WATER_NO_CHEADERS
    #include <stdio.h>
#else
    #include <cstdio>
#endif
namespace water { namespace xtr {

// xtr::sout << "hello!"
//
// constexpr auto myout = xtr::sout;
// myout << "hello"!

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

constexpr create<write_to_stdout, end_with_linebreak> sout;

}}
#endif
