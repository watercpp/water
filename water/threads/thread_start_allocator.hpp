// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_THREAD_START_ALLOCATOR_HPP
#define WATER_THREADS_THREAD_START_ALLOCATOR_HPP
#include <water/threads/bits.hpp>
#include <water/allocator_nothrow.hpp>
namespace water { namespace threads {


/*

Used by threads::run_copy to avoid dynamic memory allocation most of the time.
It allocates memory from a static char array, and uses dynamic memory only if the char array is full.
Starting lots of threads at the same time might fill the array.

To change the array size:
#define WATER_THREADS_THREAD_START_ALLOCATOR_SIZE 1234

To always use dynamic memory allocation:
#define WATER_THREADS_THREAD_START_ALLOCATOR_SIZE 0

*/


size_t constexpr thread_start_allocator_bytes = 
    #ifdef WATER_THREADS_THREAD_START_ALLOCATOR_SIZE
    WATER_THREADS_THREAD_START_ALLOCATOR_SIZE;
    #else
    sizeof(void*) * 128;
    #endif


template<size_t bytes_ = thread_start_allocator_bytes>
class thread_start_allocator
{
    static constexpr size_t divide_round(size_t a) {
        return a / align_max + (a % align_max ? 1 : 0);
    }
    
    static size_t constexpr
        capacity = divide_round(bytes_),
        shift = numeric_limits<size_t>::digits / 2,
        max = (static_cast<size_t>(1) << shift) - 1;
    
    static_assert(capacity < max, "too many bytes");

private:
    atomic<size_t> my {};
    unsigned char mymemory[capacity * align_max] {};

public:
    
    constexpr thread_start_allocator() = default;
    
    thread_start_allocator(thread_start_allocator const&) = delete;
    thread_start_allocator& operator=(thread_start_allocator const&) = delete;
    
    void* allocate(size_t bytes) noexcept {
        void *r = 0;
        size_t
            size = divide_round(bytes),
            load = my.load(memory_order_relaxed),
            at,
            count;
        do {
            at = load & max;
            count = load >> shift;
            if(at + size <= capacity && count != max) {
                r = mymemory + at * align_max;
                at += size;
                ++count;
            }
            else
                r = 0;
        } while(!my.compare_exchange_weak(load, at | (count << shift)));
        if(r)
            return r;
        ___water_debug(
            trace
                << "water::threads::thread_start_allocator could not fit another " << bytes << " bytes. "
                << "it holds " << (at * align_max) << " bytes in " << count << " allocations. "
                << "using dynamic memory instead";
        )
        return allocator_nothrow{}.allocate(bytes);
    }
    
    void free(void *pointer, size_t bytes) noexcept {
        if(pointer < mymemory || mymemory + sizeof(mymemory) <= pointer) {
            allocator_nothrow{}.free(pointer, bytes);
            return;
        }
        size_t
            size = divide_round(bytes),
            load = my.load(memory_order_relaxed),
            at,
            count;
        do {
            at = load & max;
            count = load >> shift;
            if(pointer == mymemory + (at - size) * align_max)
                at -= size;
            if(count && !--count)
                at = 0;
        } while(!my.compare_exchange_weak(load, at | (count << shift)));
    }
    
    size_t memory_use() const noexcept {
        return (my.load(memory_order_relaxed) & max) * align_max;
    }
    
    constexpr size_t memory_capacity() const noexcept {
        return capacity * align_max;
    }
    
    size_t allocations_now() const noexcept {
        return my.load(memory_order_relaxed) >> shift;
    }
    
};


template<>
class thread_start_allocator<0>
{
public:
    constexpr thread_start_allocator() = default;
    
    thread_start_allocator(thread_start_allocator const&) = delete;
    thread_start_allocator& operator=(thread_start_allocator const&) = delete;
    
    void* allocate(size_t bytes) noexcept {
        return allocator_nothrow{}.allocate(bytes);
    }
    
    void free(void *pointer, size_t bytes) noexcept {
        allocator_nothrow{}.free(pointer, bytes);
    }
};


namespace _ {

    template<typename = void>
    thread_start_allocator<> thread_start_global_allocator;

}


inline void* thread_start_allocate(size_t bytes) {
    return _::thread_start_global_allocator<>.allocate(bytes);
}

inline void thread_start_free(void *pointer, size_t bytes) {
    return _::thread_start_global_allocator<>.free(pointer, bytes);
}


}}
#endif
