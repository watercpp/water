// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_OUT_TRACE_HPP
#define WATER_STR_OUT_TRACE_HPP
#include <water/str/buffer_lines.hpp>
#include <water/trace.hpp>
namespace water { namespace str {

/*

Easy way to write to water::trace

str::out_trace{} << "hello world!";

Note that this will add a newline by itself, see buffer_lines.hpp

*/

struct write_to_trace {
	void operator()(char const* cstring, char const*) const { // always 0-terminated cstring from buffer_lines
		trace(cstring);
		}
	};

using out_trace = out<buffer_lines<write_to_trace>>;

}}
#endif
