// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_THREAD_START_ALLOCATION_HPP
#define WATER_THREADS_TESTS_THREAD_START_ALLOCATION_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/thread.hpp>
namespace water { namespace threads { namespace tests {

class thread_start_allocation
{
public:
    thread_start_allocation() {
        capacity<128>();
        capacity<63>();
        capacity<1025>();
        
        // this one only uses dynamic memory
        thread_start_allocator<0> zero;
        auto p = zero.allocate(16);
        zero.free(p, 16);
    }
    
    template<size_t capacity_>
    void capacity() {
        using pointer = void*;
        thread_start_allocator<capacity_> allocator;
        pointer pointers[capacity_ + 4] {};
        size_t sizes[] = {1, 2, 3, 4, 8, 16, 32, 33, 44, 55};
        for(auto size : sizes) {
            unsigned over = 0;
            auto p = pointers;
            do {
                bool fits = allocator.memory_capacity() - allocator.memory_use() >= size;
                auto now = allocator.allocations_now();
                *p = allocator.allocate(size);
                ___water_test(*p);
                ___water_test(!fits || now + 1 == allocator.allocations_now());
                if(!fits)
                    ++over;
                ++p;
            } while(over != 4);
            
            // free reverse order
            while(p != pointers) {
                --p;
                auto use = allocator.memory_use();
                auto now = allocator.allocations_now();
                allocator.free(*p, size);
                if(over)
                    --over;
                else {
                    ___water_test(allocator.memory_use() < use);
                    ___water_test(allocator.allocations_now() == now - 1);
                }
                *p = 0;
            }
            
            ___water_test(allocator.memory_use() == 0);
            ___water_test(allocator.allocations_now() == 0);
            
            over = 0;
            do {
                bool fits = allocator.memory_capacity() - allocator.memory_use() >= size;
                auto now = allocator.allocations_now();
                *p = allocator.allocate(size);
                ___water_test(*p);
                ___water_test(!fits || now + 1 == allocator.allocations_now());
                if(!fits)
                    ++over;
                ++p;
            } while(over != 4);
            
            // free in another order
            auto x = pointers, y = p;
            while(x < y) {
                bool xy = (((x - pointers) ^ (y - pointers)) & 1) != 0;
                allocator.free(xy ? *--y : *x++, size);
            }
            
            ___water_test(allocator.memory_use() == 0);
            ___water_test(allocator.allocations_now() == 0);
        }
    }
};

}}}
#endif
