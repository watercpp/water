// Copyright 2017-2024 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_OUT_TRACE_HPP
#define WATER_STR_OUT_TRACE_HPP
#include <water/str/buffer_lines.hpp>
#include <water/str/create.hpp>
#include <water/trace.hpp>
namespace water { namespace str {

/*

Easy way to write to water::trace

    str::trace << "hello world";

or

    str::out_trace{} << "hello world!";

Note that this will add a newline by itself, see buffer_lines.hpp

*/

struct write_to_trace
{
    void operator()(char const* a) const {
        water::trace(a);
    }
};

using out_trace = out<buffer_lines<write_to_trace>>;

constexpr create<out_trace> trace;

}}
#endif
