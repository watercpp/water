// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TEMPORARY_MEMORY_TRACK_HPP
#define WATER_TEMPORARY_MEMORY_TRACK_HPP
#include <water/temporary/memory.hpp>
#include <water/temporary/statistics.hpp>
namespace water { namespace temporary {
namespace _ {

    template<typename out_>
    inline void out_statistics_if(statistics& a) {
        out_ o;
        out(o, a);
    }
    
    template<>
    inline void out_statistics_if<void>(statistics&)
    {}
    
}

/*

Like temporary::memory but collects temporary::statistics

temporary::allocator is specialized for this. It is enough to replace temporary::memory with temporary::memory_track

template
- out_ should be void, a str::out or xtr::to_something. Unless it is void the destructor will default
construct out_ and write statistics to it

Use name() to give this a name

*/

template<typename allocator_ = void, typename sizer_ = void, typename out_ = void>
class memory_track
{
public:
    using real_allocator_type = if_not_void<allocator_, water::allocator_nothrow>;
    using allocator_type = memory_track;
    using sizer_type = if_not_void<sizer_, temporary::sizer>;

private:
    block *my = 0;
    unsigned mykeep = 1;
    real_allocator_type myallocator{};
    sizer_type mysizer{};
    temporary::statistics mystatistics{};

public:
    memory_track() = default;
    memory_track(memory_track const&) = delete;
    memory_track& operator=(memory_track const&) = delete;

    memory_track(memory_track&& a) noexcept :
        my(a.my),
        mykeep(a.mykeep),
        myallocator(static_cast<real_allocator_type&&>(a.myallocator)),
        mysizer(static_cast<real_allocator_type&&>(a.mysizer)),
        mystatistics(static_cast<temporary::statistics&&>(a.mystatistics))
    {
        a.my = 0;
    }

    explicit memory_track(real_allocator_type const& a) :
        myallocator{a}
    {}

    ~memory_track() noexcept {
        _::out_statistics_if<out_>(mystatistics);
        free_all_blocks();
    }

    memory_track& operator=(memory_track&& a) noexcept {
        swap(a);
        return *this;
    }

    void swap(memory_track& a) noexcept {
        swap_from_swap(my, a.my);
        swap_from_swap(mykeep, a.mykeep);
        swap_from_swap(myallocator, a.myallocator);
        swap_from_swap(mysizer, a.mysizer);
        swap_from_swap(mystatistics, a.mystatistics);
    }

    block*& list() noexcept {
        return my;
    }

    block* list() const noexcept {
        return my;
    }

    memory_track& allocate_and_keep_blocks(unsigned a) noexcept {
        // setting this to 0 will disable allocation of new blocks, and will keep all current blocks
        mykeep = a;
        return *this;
    }

    unsigned allocate_and_keep_blocks() const noexcept {
        return mykeep;
    }

    memory_track& sizer(sizer_type const& a) {
        mysizer = a;
        return *this;
    }

    sizer_type const& sizer() const noexcept {
        return mysizer;
    }

    memory_track* allocator_pointer() noexcept {
        return this;
    }

    bool allocate_block(size_t bytes = 0) {
        // bytes 0 makes sense if sizer has a good minimum size
        // returns true if it was successfull
        block *a = temporary::allocate_block(this, my, bytes, mysizer) != 0;
        if(!a) return false;
        my = a;
        return true;
    }

    void free_all_blocks() {
        temporary::free_all_blocks(this, my);
        my = 0;
    }

    void free_unused_blocks(unsigned keep = 0) {
        my = temporary::free_unused_blocks(this, my, keep);
    }

public:
    
    // track specific
    
    void name(char const* a) noexcept {
        mystatistics.rename(a);
    }

    temporary::statistics& statistics() noexcept {
        return mystatistics;
    }

    temporary::statistics const& statistics() const noexcept {
        return mystatistics;
    }

    void* allocate(size_t bytes) noexcept(allocator_noexcept<real_allocator_type>()) {
        auto undo = mystatistics.block_allocation.failure;
        mystatistics.block_allocation.failure(bytes);
        void *r = myallocator.allocate(bytes);
        if(r) {
            mystatistics.block_allocation.failure = undo;
            mystatistics.block_allocation.success(bytes);
        }
        return r;
    }

    void free(void* pointer, size_t bytes) noexcept {
        myallocator.free(pointer, bytes);
    }
};

template<typename allocator_, typename sizer_, typename out_>
void swap(memory_track<allocator_, sizer_, out_>& a, memory_track<allocator_, sizer_, out_>& b) noexcept {
    a.swap(b);
}

}} // namespace

#include <water/temporary/allocator_track.hpp>

namespace water { namespace temporary {
    
template<typename allocator_, typename sizer_, typename out_>
allocator_throw<memory_track<allocator_, sizer_, out_> > allocator_for(memory_track<allocator_, sizer_, out_>& a) noexcept {
    return allocator_throw<memory_track<allocator_, sizer_, out_> >{a};
}

template<typename allocator_, typename sizer_, typename out_>
allocator_nothrow<memory_track<allocator_, sizer_, out_> > allocator_nothrow_for(memory_track<allocator_, sizer_, out_>& a) noexcept {
    return allocator_nothrow<memory_track<allocator_, sizer_, out_> >{a};
}

}}
#endif
