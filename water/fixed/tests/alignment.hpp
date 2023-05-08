// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_FIXED_TESTS_ALIGNMENT_HPP
#define WATER_FIXED_TESTS_ALIGNMENT_HPP
#include <water/fixed/tests/bits.hpp>
#include <water/vector.hpp>
#include <water/align_max.hpp>
namespace water { namespace fixed { namespace tests {

/*

test that memory is aligned

*/

inline void alignment(size_t bytes, size_t align) {
    vector<void*> list;
    memory_atomic<> memory{bytes};
    size_t blocks[] = {
        memory.block_size(),
        (memory.block_size() / 3) * 2,
        (memory.block_size() / 3) * 4
    };
    for(auto b : blocks) {
        bool ok = memory.allocate_block(b);
        ___water_test(ok);
    }
    auto allocator = allocator_nothrow_lock_free_for(memory);
    while(void *a = allocator.allocate(bytes)) {
        size_t x = reinterpret_cast<size_t>(static_cast<char*>(a));
        ___water_test(x % align == 0);
        list.push_back(a);
    }
    ___water_test(list.size() >= blocks[0] + blocks[1] + blocks[2]);
    for(auto a : list)
        allocator.free(a, bytes);
}

inline void alignment() {
    alignment(1, sizeof(size_t));
    alignment(hardware_destructive_interference_size, hardware_destructive_interference_size);
    alignment(align_max, align_max);
    alignment(align_max * 2, align_max);
}

}}}
#endif
