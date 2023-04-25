// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MEMORY_TRACK_OUT_HPP
#define WATER_MEMORY_TRACK_OUT_HPP
#include <water/memory_track/memory.hpp>
#include <water/str/str.hpp>
namespace water { namespace memory_track {


/*

functions to output statistics from memory_track::memory to water::str::out

*/


template<typename o_, typename t_>
str::out<o_>& operator<<(str::out<o_>& o, cookie<t_> c) {
    o << c.pointer << ' ' << c.bytes << " #" << c.id << ' ';
    if(c.name)
        o << c.name;
    else
        o << "unnamed";
    return o;
}


template<typename o_>
void out(str::out<o_>& o, statistics const& s, unsigned indent = 0) {
    auto i = str::repeat(' ', indent);
    o   << i << "success_count . " << s.success_count << '\n'
        << i << "success_bytes . " << s.success_bytes << '\n'
        << i << "failed_count .. " << s.failed_count << '\n'
        << i << "failed_bytes .. " << s.failed_bytes << '\n'
        << i << "count_now ..... " << s.count_now << '\n'
        << i << "count_peak .... " << s.count_peak << '\n'
        << i << "bytes_now ..... " << s.bytes_now << '\n'
        << i << "bytes_peak .... " << s.bytes_peak << '\n'
        << i << "smallest_bytes  " << s.smallest_bytes << '\n'
        << i << "largest_bytes . " << s.largest_bytes << '\n'
        << i << "address_lowest  " << s.address_lowest << '\n'
        << i << "address_highest " << s.address_highest << '\n';
}


template<typename o_, typename a_, typename t_>
void out_allocations(str::out<o_>& o, memory<a_, t_>& m, unsigned indent = 0) {
    auto i = str::repeat(' ', indent);
    container<typename memory<a_, t_>::cookie_type> cookies;
    auto count = m.statistics_total().count_now;
    if(count) {
        cookies.reserve(count);
        m.cookies_to([&cookies](typename memory<a_, t_>::cookie_iterator c) {
            if(c) {
                auto e = c;
                do cookies.push_back(*c); while(++c != e);
            }
        });
    }
    if(cookies.empty())
        o << i << "no allocations\n";
    else
        for(auto &c : cookies)
            o << i << c << '\n';
}


template<typename o_, typename a_, typename t_>
void out_statistics_by_name(str::out<o_>& o, memory<a_, t_>& m) {
    container<statistics_for_name> names;
    m.statistics_by_name_to(
        [&names](statistics_for_name const* b, statistics_for_name const* e) {
            if(b != e && names.resize(static_cast<size_t>(e - b))) {
                auto t = names.begin();
                do *t++ = *b++; while(b != e);
            }
        }
    );
    for(auto &s : names) {
        o << "statistics_by_name " << s.name << '\n';
        out(o, s, 2);
    }
}


template<typename o_, typename a_, typename t_>
void out_statistics_for_bytes(str::out<o_>& o, memory<a_, t_>& m) {
    size_t s = 0;
    while(true) {
        auto b = m.statistics_for_bytes(s);
        o << "statistics_for_bytes " << b.min << "..." << b.max << '\n';
        if(b.empty())
            o << str::repeat(' ', 2) << "empty\n";
        else
            out(o, b, 2);
        if(!b.max || b.max == static_cast<size_t>(-1))
            break;
        s = b.max + 1;
    }
}


template<typename o_, typename a_, typename t_>
void out(str::out<o_>& o, memory<a_, t_>& m) {
    // output all statistics
    o << "statistics_total\n";
    out(o, m.statistics_total(), 2);
    o << "statistics_current\n";
    out(o, m.statistics_current(), 2);
    out_statistics_for_bytes(o, m);
    out_statistics_by_name(o, m);
}


}}
#endif
