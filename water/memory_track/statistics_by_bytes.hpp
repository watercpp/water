// Copyright 2017-2022 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MEMORY_TRACK_STATISTICS_BY_BYTES_HPP
#define WATER_MEMORY_TRACK_STATISTICS_BY_BYTES_HPP
#include <water/memory_track/statistics.hpp>
#include <water/memory_track/container.hpp>
namespace water { namespace memory_track {

struct statistics_for_bytes : statistics {
    size_t
        min = 0, // the min max bytes of this
        max = 0;
};

class statistics_by_bytes
{
public:
    using value_type = statistics_for_bytes;
    
    static size_t constexpr bytes_max_default =
        static_cast<size_t>(1024 * 1024 * 1024) > 0 ?
        static_cast<size_t>(1024 * 1024 * 1024) :
        static_cast<size_t>(-1);

private:
    container<value_type> my;

public:
    constexpr statistics_by_bytes() = default;
    statistics_by_bytes(statistics_by_bytes const&) = delete;
    statistics_by_bytes& operator=(statistics_by_bytes const&) = delete;

    explicit operator bool() const noexcept {
        return !my.empty();
    }

    value_type* begin() noexcept {
        return my.begin();
    }

    value_type const* begin() const noexcept {
        return my.begin();
    }

    value_type* end() noexcept {
        return my.end();
    }

    value_type const* end() const noexcept {
        return my.end();
    }

    size_t size() const noexcept {
        return my.size();
    }

    void reset() noexcept {
        if(!my.empty()) {
            auto i = my.begin();
            do static_cast<statistics&>(*i) = {}; while(++i != my.end());
        }
    }

    template<typename function_>
    size_t resize(function_&& function, size_t max = 0) noexcept {
        if(!max)
            max = bytes_max_default;

        size_t
            last = 0,
            count = 0;
        do {
            ++count;
            size_t now = function(last);
            ___water_assert(now > last);
            last = now;
        } while(last < max);
        if(!my.resize(count))
            return 0;
        auto i = my.begin();
        last = 0;
        do {
            size_t now = function(last);
            ___water_assert(now > last);
            *i = {};
            i->min = last + 1;
            i->max = now;
            last = now;
        } while(++i != my.end());
        my.begin()->min = 0;
        my.end()[-1].max = static_cast<size_t>(-1);
        return my.size();
    }

    size_t resize() noexcept {
        return resize([](size_t s){ return s ? (s << 1) : 4; });
    }

    value_type* find(size_t a) noexcept {
        return find_do(begin(), end(), a);
    }

    value_type const* find(size_t a) const noexcept {
        return find_do(begin(), end(), a);
    }

    value_type find_and_copy(size_t a) const noexcept {
        value_type r;
        if(auto b = find_do(begin(), end(), a))
            r = *b;
        else
            r.max = static_cast<size_t>(-1);
        return r;
    }

private:
    template<typename iterator_>
    static iterator_ find_do(iterator_ b, iterator_ e, size_t a) noexcept {
        if(b == e)
            return 0;
        while(b->max < a) {
            auto i = b + (e - b) / 2;
            if(a < i->min) {
                ++b;
                e = i;
            }
            else
                b = i;
        }
        return b;
    }
    
};


}}
#endif
