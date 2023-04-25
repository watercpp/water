// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MEMORY_TRACK_STATISTICS_BY_NAME_HPP
#define WATER_MEMORY_TRACK_STATISTICS_BY_NAME_HPP
#include <water/memory_track/statistics.hpp>
#include <water/memory_track/container.hpp>
#include <water/memory_track/names.hpp>
namespace water { namespace memory_track {


struct statistics_for_name : statistics {
    memory_track::name name; // can be blank
};


class statistics_by_name
{
public:
    using value_type = statistics_for_name;

private:
    container<value_type> my;

public:
    constexpr statistics_by_name() = default;
    statistics_by_name(statistics_by_name const&) = delete;
    statistics_by_name& operator=(statistics_by_name const&) = delete;

    value_type const* begin() const noexcept {
        return my.begin();
    }

    value_type const* end() const noexcept {
        return my.end();
    }

    size_t size() const noexcept {
        return my.size();
    }
    
    void reset() noexcept {
        my.resize(0);
    }

    value_type* find(name a) noexcept {
        auto f = name_find(my.begin(), my.end(), a.begin(), a.size(), [](value_type* v) { return v->name; }).found;
        if(f != my.end())
            return f;
        return 0;
    }

    value_type const* find(name a) const noexcept {
        auto f = name_find(my.begin(), my.end(), a.begin(), a.size(), [](value_type const* v) { return v->name; }).found;
        if(f != my.end())
            return f;
        return 0;
    }

    value_type find_and_copy(name a) const noexcept {
        if(auto r = find(a))
            return *r;
        return {};
    }

    value_type* find_or_add(name a) noexcept {
        auto f = name_find(my.begin(), my.end(), a.begin(), a.size(), [](value_type* v) { return v->name; });
        if(f.found != my.end())
            return f.found;
        value_type v;
        v.name = a;
        return my.insert(f.insert_before, v);
    }
};


}}
#endif
