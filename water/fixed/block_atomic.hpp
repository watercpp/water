// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_FIXED_BLOCK_ATOMIC_HPP
#define WATER_FIXED_BLOCK_ATOMIC_HPP
#include <water/water.hpp>
#include <water/atomic.hpp>
#include <water/hardware_interference_size.hpp>
#include <water/new_here.hpp>
namespace water { namespace fixed {

// this is used by memory_atomic, dont use it alone

class block_atomic
{
    using unsigned_ = decltype(atomic_uint{}.load());
    
    atomic_uint my{};
    unsigned_
        mysize, // number of allocations, not size in bytes
        mymask = 0; // number of bits needed for mysize, the rest of my is a change counter
    block_atomic *mylist;
    void *mymemory;
    void *myallocation;
    size_t myallocation_size;

private:

    block_atomic(size_t bytes, size_t size, block_atomic *list, void* memory, void* allocation, size_t allocation_size) :
        mysize{static_cast<unsigned_>(size)},
        mylist{list},
        mymemory{memory},
        myallocation{allocation},
        myallocation_size{allocation_size}
        
    {
        while((mymask = (mymask << 1) | 1) < mysize);
        char *m = static_cast<char*>(mymemory);
        size_t i = 0;
        do {
            *static_cast<size_t*>(static_cast<void*>(m + bytes * i)) = i + 1;
        } while(++i != mysize);
    }

public:
    
    template<typename allocator_>
    static block_atomic* create(allocator_ *allocator, size_t bytes, size_t size, block_atomic *list) {
        // align this and the first allocation to hardware_destructive_interference_size
        // if bytes is a multiple of hardware_destructive_interference_size, every allocation is aligned to it
        // the size of the returned memory block can be >= size because of the extra space used for alignment
        size_t constexpr this_size = round_up_to_hardware_destructive_interference_size(sizeof(block_atomic));
        auto s = this_size + hardware_destructive_interference_size + bytes * size;
        auto a = static_cast<char*>(allocator->allocate(s));
        if(!a)
            return 0;
        auto align = reinterpret_cast<size_t>(a) % hardware_destructive_interference_size;
        if(align)
            align = hardware_destructive_interference_size - align;
        size = (s - (align + this_size)) / bytes;
        return new(here(a + align)) block_atomic(bytes, size, list, a + align + this_size, a, s);
    }
    
    template<typename allocator_>
    void destroy(allocator_ *allocator) {
        auto a = myallocation;
        auto s = myallocation_size;
        this->~block_atomic();
        allocator->free(a, s);
    }

    void* allocate(size_t bytes, bool& more) {
        unsigned_ a = my.load(memory_order_acquire);
        void *r = 0;
        unsigned_ at, count;
        do {
            at = a & mymask;
            count = a & ~mymask; // change count is only changed by free
            if(at >= mysize)
                r = 0;
            else {
                r = static_cast<char*>(memory()) + at * bytes;
                at = static_cast<unsigned_>(*static_cast<size_t*>(r));
            }
            more = at < mysize;
        } while(!my.compare_exchange_weak(a, at | count));
        return r;
    }

    void free(void *pointer, size_t bytes) {
        unsigned_
            p = static_cast<unsigned_>(static_cast<char*>(pointer) - static_cast<char*>(memory())) / bytes,
            a = my.load(memory_order_relaxed),
            next,
            count;
        do {
            next = a & mymask;
            count = (a & ~mymask) + (mymask + 1); // change change count, will wrap around
            *static_cast<size_t*>(pointer) = next;
        } while(!my.compare_exchange_weak(a, p | count));
    }

    bool inside(void const *pointer, size_t bytes) {
        return memory() <= pointer && pointer <= static_cast<char*>(memory()) + mysize * bytes;
    }

    block_atomic* list() const {
        return mylist;
    }

    size_t size() const {
        return mysize;
    }

    void* memory() {
        return mymemory;
    }
    
    size_t memory_use() const {
        return myallocation_size;
    }
};

}}
#endif
