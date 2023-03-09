// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XTR_TESTS_SIZE_USED_HPP
#define WATER_XTR_TESTS_SIZE_USED_HPP
#include <water/atomic.hpp>
namespace water { namespace xtr { namespace tests {

/*

This can help to tune the xtr string size and extra size if the strings frequently are truncated,
or if stack memory is very limited, or just to see how it performs. It tracks how much of the stack
space reserved for a string was used.

For example, this knows how many experessions with 128-256 bytes allocated on the stack used 20-29% of that space.

Using it is a bit complicated:

In a header file, define WATER_XTR_SIZE_USED before anything from water is included.
water::trace uses xtr and almost everything in water will include water/trace.hpp.
Define it to the fully qualified name of a function taking 2 unsigned arguments:

    namespace space {
        void xtr_size_used(unsigned size, unsigned used);
    }
    #define WATER_XTR_SIZE_USED(size, used) ::space::xtr_size_used(size, used);
    
In a cpp file, implement the function from the define using a global water::xtr::tests::size_used.
Also print the result to the debug output when the program ends:

    #include <water/xtr/tests/size_used.hpp>
    #include <water/str/out_trace.hpp>
    #include <water/later.hpp>
    namespace space {
        
        water::xtr::tests::size_used size_used_global;
        
        void xtr_size_used(unsigned size, unsigned used) {
            size_used_global(size, used);
        }
        
        auto size_used_print = water::later([]{
            size_used_global.to_xtr_or_str(water::str::out_trace{});
        });
        
    }

*/

class size_used
{
    static unsigned constexpr
        buckets = 8,
        size0 = 1 << 5;
    
    atomic<size_t> my[buckets][10] {};

public:
    
    constexpr size_used() = default;
    size_used(size_used const&) = delete;
    size_used& operator=(size_used const&) = delete;
    
    void operator()(unsigned size, unsigned used) noexcept {
        if(used > size || !size)
            return;
        unsigned s = size0;
        unsigned b = 0;
        while(s < size && b != (buckets - 1)) {
            s <<= 1;
            ++b;
        }
        unsigned x = (used * 100 + 5) / (size * 10);
        my[b][x <= 9 ? x : 9].fetch_add(1, memory_order_relaxed);
    }
    
    template<typename to_>
    void to_xtr_or_str(to_&& to) {
        auto write = [&to](size_t const *bucket, unsigned min, unsigned max) {
            size_t sum = 0;
            for(unsigned a = 0; a != 10; ++a)
                sum += bucket[a];
            to << "water::xtr size used for sizes " << min << '-' << max << '\n'
                << "   0-9%   " << bucket[0] << '\n'
                << "  10-19%  " << bucket[1] << '\n'
                << "  20-29%  " << bucket[2] << '\n'
                << "  30-39%  " << bucket[3] << '\n'
                << "  40-49%  " << bucket[4] << '\n'
                << "  50-59%  " << bucket[5] << '\n'
                << "  60-69%  " << bucket[6] << '\n'
                << "  70-79%  " << bucket[7] << '\n'
                << "  80-89%  " << bucket[8] << '\n'
                << "  90-100% " << bucket[9] << '\n'
                << "     sum  " << sum << '\n';
        };
        unsigned s = size0;
        size_t total[10] {};
        for(unsigned b = 0; b != buckets; ++b) {
            unsigned s2 = s << 1;
            size_t bucket[10] {};
            for(unsigned a = 0; a != 10; ++a)
                total[a] += bucket[a] = my[b][a].load(memory_order_relaxed);
            write(bucket, b ? s : 0, b == (buckets - 1) ? static_cast<unsigned>(-1) : s2 - 1);
            s = s2;
        }
        write(total, 0, static_cast<unsigned>(-1));
    }
};


}}}
#endif
