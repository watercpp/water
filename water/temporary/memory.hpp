// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TEMPORARY_MEMORY_HPP
#define WATER_TEMPORARY_MEMORY_HPP
#include <water/temporary/block.hpp>
#include <water/temporary/allocator.hpp>
#include <water/swap.hpp>
#include <water/allocator_nothrow.hpp>
#ifdef WATER_DEBUG
    #include <water/trace.hpp>
#endif
namespace water { namespace temporary {

template<typename allocator_ = void, typename sizer_ = void>
class memory
{
public:
    using allocator_type = typename types::if_not_void<allocator_, water::allocator_nothrow>::result;
    using sizer_type = typename types::if_not_void<sizer_, temporary::sizer>::result;

private:
    block *my = 0;
    unsigned mykeep = 1;
    allocator_type myallocator{};
    sizer_type mysizer{};

public:
    memory() = default;
    memory(memory const&) = delete;
    memory& operator=(memory const&) = delete;

    memory(memory&& a) noexcept :
        my(a.my),
        mykeep(a.mykeep),
        myallocator(static_cast<allocator_type&&>(a.myallocator)),
        mysizer(static_cast<allocator_type&&>(a.mysizer))
    {
        a.my = 0;
    }

    explicit memory(allocator_type const& a) :
        myallocator{a}
    {}

    ~memory() noexcept {
        #ifdef WATER_DEBUG
        trace << "water::temporary::memory destructor " << this << '\n';
        unsigned n = 0;
        if(block *b = my)
            do {
                ++n;
                trace << "  block bytes " << b->capacity() << '\n';
            } while((b = b->next) != my);
        trace << "  " << n << " blocks\n";
        #endif
        free_all_blocks();
    }

    memory& operator=(memory&& a) noexcept {
        swap(a);
        return *this;
    }

    void swap(memory& a) noexcept {
        swap_from_swap(my, a.my);
        swap_from_swap(mykeep, a.mykeep);
        swap_from_swap(myallocator, a.myallocator);
        swap_from_swap(mysizer, a.mysizer);
    }

    block*& list() noexcept {
        return my;
    }

    block* list() const noexcept {
        return my;
    }

    memory& allocate_and_keep_blocks(unsigned a) noexcept {
        // setting this to 0 will disable allocation of new blocks, and will keep all current blocks
        mykeep = a;
        return *this;
    }

    unsigned allocate_and_keep_blocks() const noexcept {
        return mykeep;
    }

    memory& sizer(sizer_type const& a) {
        mysizer = a;
        return *this;
    }

    sizer_type const& sizer() const noexcept {
        return mysizer;
    }

    allocator_type* allocator_pointer() noexcept {
        return &myallocator;
    }

    bool allocate_block(size_t bytes = 0) {
        // bytes 0 makes sense if sizer has a good minimum size
        // returns true if it was successfull
        block *a = temporary::allocate_block(&myallocator, my, bytes, mysizer) != 0;
        if(!a) return false;
        my = a;
        return true;
    }

    void free_all_blocks() {
        temporary::free_all_blocks(&myallocator, my);
        my = 0;
    }

    void free_unused_blocks(unsigned keep = 0) {
        my = temporary::free_unused_blocks(&myallocator, my, keep);
    }
};

template<typename allocator_, typename sizer_>
void swap(memory<allocator_, sizer_>& a, memory<allocator_, sizer_>& b) noexcept {
    a.swap(b);
}

template<typename allocator_, typename sizer_>
allocator_throw<memory<allocator_, sizer_> > allocator_for(memory<allocator_, sizer_>& a) noexcept {
    return allocator_throw<memory<allocator_, sizer_> >{a};
}

template<typename allocator_, typename sizer_>
allocator_nothrow<memory<allocator_, sizer_> > allocator_nothrow_for(memory<allocator_, sizer_>& a) noexcept {
    return allocator_nothrow<memory<allocator_, sizer_> >{a};
}

}}
#endif
