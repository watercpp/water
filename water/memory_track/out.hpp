// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MEMORY_TRACK_OUT_HPP
#define WATER_MEMORY_TRACK_OUT_HPP
#include <water/memory_track/memory.hpp>
#include <water/str/str.hpp>
namespace water { namespace memory_track {

template<typename o_>
str::out<o_>& operator<<(str::out<o_>& o, str::specific<tag> a) {
    auto t = a.get();
    o << (t.name() ? t.name() : "unnamed");
    return o;
}

template<typename o_, typename t_>
str::out<o_>& operator<<(str::out<o_>& o, cookie<t_> c) {
    o << c.pointer << ' ' << c.bytes << " #" << c.id << ' ' << c.tag;
    return o;
}

template<typename o_>
void out(str::out<o_>& o, statistics const& s, unsigned indent = 0) {
    auto i = str::repeat(' ', indent);
    o << i << "success_count . " << s.success_count << '\n'
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
void out(str::out<o_>& o, memory<a_, t_>& m) {
    // output statistics + buckets
    o << "statistics_total\n";
    out(o, m.statistics_total(), 2);
    o << "statistics_current\n";
    out(o, m.statistics_current(), 2);
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

}}
#endif
