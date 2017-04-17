// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MEMORY_TRACK_EXAMPLE_EXAMPLE_HPP
#define WATER_MEMORY_TRACK_EXAMPLE_EXAMPLE_HPP
#include <water/memory_track/memory_track.hpp>
#include <water/vector.hpp>
#include <water/str/out.hpp>
namespace water { namespace memory_track { namespace example {

/*

Basic example that replaces the global operator new + delete.
When the program ends, statistics are written to water::trace (the debugger output)

To use it, compile example.cpp into your program. The easiest method is probably to include the cpp
file from one of the programs cpp files.

#define WATER_DEBUG so that the program breakpoints if memory_track detects an error.

If you get access violation errors for memory addresses like 0xdeaddead it means you are using
uninitialized memory or memory that has been freed. memory_track::memory will fill everything with
0xdead.

*/

using memory_type = memory_track::memory<>;
using memory_tag_type = memory_type::tag_type;

memory_type& memory();

template<typename base_> void dump_to(str::out<base_>& to) {
	out(to, memory());
		{
		vector<statistics_for_name, underlying_allocator<>> v;
		memory().statistics_by_name_to([&v](statistics_for_name const* b, statistics_for_name const* e) {
			if(b != e)
				v.assign(b, e);
			});
		for(auto &s : v) {
			to << "tag " << s.name << '\n';
			out(to, s, 2);
			}
		}
		{
		vector<typename memory_type::cookie_type, underlying_allocator<>> v;
		memory().cookies_to([&v](typename memory_type::cookie_iterator i) {
			if(i) {
				auto e = i;
				do v.push_back(*i); while(++i != e);
				}
			});
		if(v.empty())
			to << "no cookies\n";
		else {
			to << "cookies\n";
			for(auto &c : v)
				to << "  " << c << '\n';
			}
		}
	}

}}}
#endif
