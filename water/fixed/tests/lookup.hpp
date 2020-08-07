// Copyright 2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_FIXED_TESTS_LOOKUP_HPP
#define WATER_FIXED_TESTS_LOOKUP_HPP
#include <water/fixed/tests/bits.hpp>
namespace water { namespace fixed { namespace tests {

/*

test that memory.lookup() works

*/

struct lookup_list {
    lookup_list *list;
    size_t at;
};

inline void lookup() {
    memory_atomic<> memory{sizeof(lookup_list)};
    lookup_list *list = 0;
    
    size_t
        size = 0,
        block_sizes[] {12, 34, 56, 78, 910};
    for(auto s : block_sizes) {
        ___water_test(memory.allocate_block(s));
        size += s;
        while(s--) {
            auto allocated = static_cast<lookup_list*>(memory.allocate_lock_free());
            ___water_test(allocated);
            allocated->list = list;
            list = allocated;
            list->at = memory.lookup(list);
            ___water_test(0 < list->at && list->at <= size);
            auto pointer = static_cast<lookup_list*>(memory.lookup(list->at));
            ___water_test(pointer == list);
            auto previous_at = memory.lookup(list->list);
            auto previous_pointer = static_cast<lookup_list*>(memory.lookup(previous_at));
            ___water_test(previous_pointer == list->list);
            if(!list->list) {
                ___water_test(!previous_at);
            }
            else {
                ___water_test(0 < previous_at && previous_at <= size); // it does not need to be list->at - 1
                ___water_test(previous_at == list->list->at);
            }
        }
    }
    
    auto *l = list;
    while(l) {
        auto at = memory.lookup(l);
        ___water_test(at == l->at);
        auto pointer = memory.lookup(at);
        ___water_test(pointer == l);
        l = l->list;
    }
    
    while((l = list) != 0) {
        list = l->list;
        memory.free(l);
    }
    
    auto statistics = memory.statistics();
    ___water_test(statistics.allocations_now == 0);
    ___water_test(statistics.blocks == sizeof(block_sizes) / sizeof(block_sizes[0]));
}

}}}
#endif
