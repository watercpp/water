// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_FIXED_MEMORY_ATOMIC_HPP
#define WATER_FIXED_MEMORY_ATOMIC_HPP
#include <water/mutex.hpp>
#include <water/allocator_nothrow.hpp>
#include <water/new_here.hpp>
#include <water/types.hpp>
#include <water/fixed/allocator.hpp>
#include <water/fixed/block_atomic.hpp>
namespace water { namespace fixed {

unsigned constexpr memory_atomic_block_size = 512;

struct memory_atomic_statistics {
    size_t
        memory_use,
        blocks,
        allocate_quick, // allocated using the "hint", this should be larger than all others
        allocate_lock_free, // allocate_lock_free or allocate did not have to lock
        allocate_lock_free_failed, // allocate_lock_free returned 0
        allocate_locked, // means a new block was allocated
        allocate_locked_failed, // block allocation failed
        free,
        allocations_now;
};

template<bool>
struct memory_atomic_statistics_if {
    atomic_uint
        allocate_quick{},
        allocate_lock_free{},
        allocate_lock_free_failed{},
        allocate_locked{},
        allocate_locked_failed{},
        free{};
    
    memory_atomic_statistics copy() {
        return {
            0,
            0,
            allocate_quick.load(memory_order_relaxed),
            allocate_lock_free.load(memory_order_relaxed),
            allocate_lock_free_failed.load(memory_order_relaxed),
            allocate_locked.load(memory_order_relaxed),
            allocate_locked_failed.load(memory_order_relaxed),
            free.load(memory_order_relaxed),
            0
        };
    }
};

template<>
struct memory_atomic_statistics_if<false> {
    memory_atomic_statistics copy() {
        return {};
    }
};


// allocator_ must not throw

template<typename allocator_ = void, bool statistics_ = false>
class memory_atomic
{
public:
    using allocator_type = if_not_void<allocator_, water::allocator_nothrow>;
    static unsigned constexpr align = sizeof(size_t);

private:
    atomic<block_atomic*>
        mylist {},
        myhint {}; // last seen block with free space
    mutex mymutex;
    allocator_type *myallocator = 0;
    char myallocator_memory[sizeof(allocator_type)] {}; // need {} for constexpr constructor
    size_t
        mybytes,
        myblocksize;
    memory_atomic_statistics_if<statistics_> mystatistics;

public:
    constexpr memory_atomic(size_t bytes, size_t block_size = 0) :
        mybytes(align_bytes(bytes)),
        myblocksize(block_size ? block_size : memory_atomic_block_size)
    {}

    memory_atomic(memory_atomic const&) = delete;
    memory_atomic& operator=(memory_atomic const&) = delete;

    ~memory_atomic() {
        block_atomic *list = mylist.load(memory_order_acquire);
        if(list) {
            lock_guard lock{mymutex};
            do {
                block_atomic *f = list;
                list = list->list();
                f->destroy(myallocator);
            } while(list);
            myallocator->~allocator_type();
        }
    }

    size_t bytes() const {
        return mybytes;
    }

    memory_atomic& block_size(size_t a) {
        lock_guard lock{mymutex};
        if(a) myblocksize = a;
        return *this;
    }

    size_t block_size() {
        lock_guard lock{mymutex};
        return myblocksize;
    }

    void* allocate() {
        return allocate(false);
    }

    void* allocate_lock_free() {
        return allocate(true);
    }

    void free(void* pointer) {
        block_atomic *list = mylist.load(memory_order_acquire);
        while(list && !list->inside(pointer, mybytes))
            list = list->list();
        if(list) {
            list->free(pointer, mybytes);
            myhint.store(list, memory_order_relaxed);
            statistics_free(mystatistics);
        }
        ___water_assert(list);
    }

    bool allocate_block(size_t block_size = 0) {
        // could throw
        lock_guard lock{mymutex};
        return allocate_block_locked(mylist.load(), block_size) != 0;
    }

    size_t lookup(void const* find) const {
        return lookup(find, memory_order_acquire);
    }

    void* lookup(size_t find) const {
        return lookup(find, memory_order_acquire);
    }

    size_t lookup_unordered(void const* find) const {
        // useful if the current thread just allocated find or read find using aquire or higher order
        return lookup(find, memory_order_relaxed);
    }

    void* lookup_unordered(size_t find) const {
        // useful if the current thread read find using aquire or higher order
        return lookup(find, memory_order_relaxed);
    }

    size_t memory_use() {
        block_atomic *list = mylist.load(memory_order_acquire);
        size_t r = 0;
        while(list) {
            r += list->memory_use();
            list = list->list();
        }
        return r;
    }

    memory_atomic_statistics statistics() {
        block_atomic *list = mylist.load(memory_order_acquire);
        memory_atomic_statistics r = mystatistics.copy();
        while(list) {
            ++r.blocks;
            r.memory_use += list->memory_use();
            list = list->list();
        }
        // allocations now is sum of all allocations - free
        r.allocations_now = (r.allocate_quick + r.allocate_lock_free + r.allocate_locked) - r.free;
        return r;
    }

private:
    static constexpr size_t align_bytes(size_t a) {
        return a + (a % align ? align - (a % align) : 0);
    }

    void* allocate(bool lock_free) {
        bool more;
        auto block = myhint.load(memory_order_relaxed);
        if(block)
            if(void *r = block->allocate(mybytes, more)) {
                statistics_allocate_quick(mystatistics);
                return r;
            }
        auto list = mylist.load(memory_order_acquire);
        block = list;
        bool did_lock = false;
        while(true) {
            while(block) {
                void *r = block->allocate(mybytes, more);
                if(r) {
                    if(more)
                        myhint.store(block, memory_order_relaxed);
                    statistics_allocate(mystatistics, did_lock);
                    return r;
                }
                block = block->list();
            }
            if(lock_free)
                break;
            lock_guard lock{mymutex};
            did_lock = true;
            block_atomic *now = mylist.load(memory_order_relaxed);
            if(list == now) {
                list = block = allocate_block_locked(list); // throws
                if(!block)
                    break;
            }
            else
                list = block = now;
        }
        statistics_allocate_failed(mystatistics, lock_free);
        return 0;
    }

    block_atomic* allocate_block_locked(block_atomic *list, size_t block_size = 0) {
        #ifdef WATER_USE_WATER_THREADS
        ___water_threads_statistics(
            if(!mylist.load(memory_order_relaxed))
                name_if(statistics_pointer(mymutex)) << "water::fixed::memory_atomic bytes=" << mybytes;
        )
        #endif
        if(!block_size)
            block_size = myblocksize;
        if(!myblocksize)
            return 0;
        if(!myallocator)
            myallocator = new(here(myallocator_memory)) allocator_type; // throws???
        block_atomic *b = block_atomic::create(myallocator, mybytes, block_size, list);
        if(!b)
            return 0;
        mylist.store(b, memory_order_release);
        return b;
    }

    size_t lookup(void const* find, memory_order order) const {
        block_atomic *list = mylist.load(order);
        while(list && !list->inside(find, mybytes))
            list = list->list();
        if(!list)
            return 0;
        size_t r = 1 + static_cast<size_t>(static_cast<char const*>(find) - static_cast<char const*>(list->memory())) / mybytes;
        while((list = list->list()) != 0)
            r += list->size();
        return r;
    }

    void* lookup(size_t find, memory_order order) const {
        if(!find)
            return 0;
        --find;
        block_atomic
            *list = mylist.load(order),
            *l = list;
        size_t size = 0;
        while(l) {
            size += l->size();
            l = l->list();
        }
        if(find >= size)
            return 0;
        while(find < (size -= list->size()))
            list = list->list();
        return static_cast<char*>(list->memory()) + (find - size) * mybytes;
    }

    static void statistics_allocate_quick(memory_atomic_statistics_if<true>& s) {
        s.allocate_quick.fetch_add(1, memory_order_relaxed);
    }

    static void statistics_allocate(memory_atomic_statistics_if<true>& s, bool did_lock) {
        did_lock ? s.allocate_locked.fetch_add(1, memory_order_relaxed) : s.allocate_lock_free.fetch_add(1, memory_order_relaxed);
    }

    static void statistics_allocate_failed(memory_atomic_statistics_if<true>& s, bool lock_free) {
        lock_free ? s.allocate_lock_free_failed.fetch_add(1, memory_order_relaxed) : s.allocate_locked_failed.fetch_add(1, memory_order_relaxed);
    }

    static void statistics_free(memory_atomic_statistics_if<true>& s) {
        s.free.fetch_add(1, memory_order_relaxed);
    }

    static void statistics_allocate_quick(memory_atomic_statistics_if<false> const&) {}
    static void statistics_allocate(memory_atomic_statistics_if<false> const&, bool) {}
    static void statistics_allocate_failed(memory_atomic_statistics_if<false> const&, bool) {}
    static void statistics_free(memory_atomic_statistics_if<false> const&) {}
};

template<typename allocator_, bool statistics_>
allocator_throw<memory_atomic<allocator_, statistics_>> allocator_for(memory_atomic<allocator_, statistics_>& a) noexcept {
    return a;
}

template<typename allocator_, bool statistics_>
allocator_nothrow<memory_atomic<allocator_, statistics_>> allocator_nothrow_for(memory_atomic<allocator_, statistics_>& a) noexcept {
    return a;
}

template<typename allocator_, bool statistics_>
allocator_throw_lock_free<memory_atomic<allocator_, statistics_>> allocator_lock_free_for(memory_atomic<allocator_, statistics_>& a) noexcept {
    return a;
}

template<typename allocator_, bool statistics_>
allocator_nothrow_lock_free<memory_atomic<allocator_, statistics_>> allocator_nothrow_lock_free_for(memory_atomic<allocator_, statistics_>& a) noexcept {
    return a;
}

}}
#endif
