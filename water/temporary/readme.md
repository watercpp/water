# water::temporary

`water::temporary` can help in a specific situation:

- Dynamic memory is needed temporarily, but repeatedly over and over with similar needs every time.
- The normal memory allocators are too slow or cause unwanted synchronization between threads.
- The code can be modified to use a special memory allocator.

It works like this:

A thread has a `water::temporary::memory` memory buffer. It is used from a single thread, each
thread using temporary memory needs its own memory buffer. That memory buffer is allocated from an
underlying allocator and is kept and reused for a long time.

When the thread needs temporary memory for some work, it uses `water::temporary::allocator` to
allocate memory from the `water::temporary::memory` buffer. It is very important that all temporary
memory is freed when the work is done, it should only be used temporarily.

    void thread() {
        temporary::memory<> memory;
        
        while(wait_for_work()) {
        
            auto allocator = allocator_for(memory);
            
            void *raw_memory = allocator.allocate(123);
            do_something(raw_memory, 123);
            allocator.free(raw_memory, 123);
            
            temporary::vector<int, decltype(allocator)> v{allocator};
            for(int i = 0; i != 1000; ++i)
                v.push_back(i);
        }
    }

Using `water::temporary::allocator` to allocate and free raw memory is not fun. It is difficult to
predict what the temporary memory will be used for and prepare easy to use tools for it. But the
`temporary::vector` exists.



## temporary::vector

    template<typename value_, typename allocator_, typename sizer_ = void>
    class vector

This class looks almost exactly like `std::vector` but it uses a `temporary::allocator` and is
optimized for temporary memory. A `temporary::allocator` must be used to construct a
`temporary::vector`, unlike `std::vector` where the `std::allocator` is optional. Except for that,
the interface should be exactly like `std::vector`.

When using two or more `temporary::vector` at the same time, try to fully grow each
`temporary::vector` before adding elements to another. This is the most effective way to use the
`temporary::memory`.

    temporary::memory<> memory;
    using allocator = temporary::allocator_throw<temporary::memory<>>;
    
    temporary::vector<int, allocator>
        v1{allocator{memory}},
        v2{allocator{memory}};
    
Avoid this:

    for(int i = 0; i != 1000; ++i) {
        v1.push_back(i);
        v2.push_back(i);
    }
    
This is effective:

    v1.reserve(1000);
    v2.reserve(1000);
    for(int i = 0; i != 1000; ++i) {
        v1.push_back(i);
        v2.push_back(i);
    }

This is also fine:

    for(int i = 0; i != 1000; ++i)
        v1.push_back(i);
    for(int i = 0; i != 1000; ++i)
        v2.push_back(i);



## water::move_pointer and water::later

`water::move_pointer` from `water/move_pointer.hpp` and `water::later` from `water/later.hpp` can
help when working with temporary memory.

    water::temporary::memory<> memory;
    
    auto allocator = allocator_for(memory);

    water::move_pointer<thing, decltype(allocator)> pointer{allocator};
    new(pointer) thing;
    pointer->function();
    
    void *raw_memory = allocator.allocate(123);
    auto raw_memory_free = water::later([raw_memory, allocator] { allocator.free(raw_memory, 123); });
    memcpy(raw_memory, something_else, 123);

`water::move_pointer` is similar to `std::unique_ptr` except it has `operator new` and uses
`water::allocator` type allocators for memory allocation.



## allocator and allocator_nothrow

Allocators can throw or return 0 when they fail to allocate memory:

    water::temporary::memory<> memory;
    
    auto a1 = allocator_for(memory); // throws exception when out of memory
    auto a2 = allocator_nothrow_for(memory); // returns 0
    
    using allocator = water::temporary::allocator_throw<water::temporary::memory<>>;
    using allocator_nothrow = water::temporary::allocator_nothrow<water::temporary::memory<>>;

The `temporary::memory` itself can use a `water::allocator` that throws or returns 0 (it uses one
that returns 0 by default):

    water::temporary::memory<water::allocator> memory_that_throws_exceptions;
    water::temporary::memory<water::allocator_nothrow> memory_that_returns_0;



## water::allocator functions and the lower level push/pop/resize functions

When writing code specifically for `water::temporary` there is the `allocator.allocate` and
`allocator.free` functions that are common to other allocators in `water`. Beyond that there is:

- `allocator.resize(pointer, old_size, new_size)` to grow or shrink an existing allocation
- `push` and `pop` that are similar to `allocate` and `free`
- `push_at_least(size_t& pushed, size_t at_least)` that will allocate as much as possible but at
  least a specific size (shrink it later with `resize`)

Look at `water/temporary/allocator.hpp` to see all functions.

Look at the documentation in `water/temprary/block.hpp` for the lowest level interface.



## Tuning and statistics

To get an idea of how the temporary memory is used, `temporary::memory_track` is a drop-in
replacement for `temporary::memory` that collects statistics. This is slower so do not use it all
the time.

Look at `water/temporary/memory_track.hpp` and `water/temporary/statistics.hpp`.

`temporary::memory` holds a few large memory blocks. It will allocate a new block when it's needed,
and at the same time free the smallest unused.

Depending on what the program does, it might be good to have a few large memory blocks in the
`temporary::memory<>` or more but smaller blocks.

    water::temporary::memory<> memory;
    memory.sizer(water::temporary::sizer{1024 * 1024}); // smallest block size is 1 MB
    memory.allocate_and_keep_blocks(3); // and try to keep the 3 largest blocks

It is also possible to pre-allocate memory and prevent any further memory allocation:

    water::temporary::memory<> memory;
    memory.allocate_block(1024 * 1024);
    memory.allocate_block(1024 * 1024);
    memory.allocate_and_keep_blocks(0); // zero means it will never allocate more memory
    
