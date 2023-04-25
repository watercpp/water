# water::memory_track

`water::memory_track` is used to track memory allocations, to figure out how a program uses dynamic
memory. It collects statistics and it is possible to iterate over all current allocations.

It also detects errors: Memory leaks, under and overruns, double frees, use of uninitialized or
freed memory. Or when non-array delete is used to delete an array. And allocations can be made to
fail, to simulate out-of-memory conditions.

Using `water::memory_track` will require changes to the programs code. And it will make the program
run slower and use more dynamic memory. Use it only for testing and debugging.

`#include <water/memory_track/memory_track.hpp>`


## How it works

`memory_track::memory` is the root allocator that tracks all allocations and collects statistics. It
allocates its memory from `memory_track::underlying_allocator` that defaults to use `malloc` so the
global new and delete can be tracked if you want to.

    template<typename tag_ = void, typename callback_ = void>
    class memory;

`memory` is a big object that cannot be moved or copied, and it must exist as long as any
allocations made from it exists.

You should not use `memory` directly except when writing new and delete operators or your own
allocators. Instead, use the light-weight `memory_track::std_allocator` for STL containers or
`memory_track::allocator` for a `water::allocator` type allocator.

    memory_track::memory<> m;
    std::vector<int, memory_track::std_allocator<int>> v{memory_track::std_allocator<int>(m, "tag")};

When memory is allocated, extra space for a `memory_track::cookie` and small areas before and after
the allocation to check for overruns and underruns are added to the allocation. The whole allocation
is filled with `0xDEAD` to catch use of uninitialized memory.

The extra space is usually 88 bytes on 64-bit, and 52 bytes on 32-bit.

All allocations are tagged with a name. Use it as a category to group related allocations together.
You can provide your own tag via the `tag_` template argument to `memory` to tag allocations with
more metadata.

The `memory_track::cookie` contains all information about the allocation. The pointer, size,
alignment and tags etc. It is possible to iterate through every `cookie` via `memory`, or get a
`cookie` from a pointer.

When memory is deallocated `memory` uses this information to check for errors. The whole allocation
is filled with `0xDEAD` again, to catch use-after-free errors.

Statistics are collected as `memory_track::statistics` objects (see below for details).

Statistics are collected for
- Total statistics for all allocations.
- By tag name, for example allocations tagged with "texture buffer".
- By size, for example allocations between 65-128 bytes.

It's up to you to do something useful with these statistics. There are functions to list all
statistics and cookies as text to a `water::str::out`, but nothing more.

You can provide a callback via the `callback_` template argument to `memory` that will be called for
all allocations and frees. One use of this is to simulate out of memory conditions, the callback can
deny allocations.

Memory leaks can be detected by checking if any allocations exist in `memory` when all memory should
be deallocated.



## Example

The example in `water/memory_track/example/` replaces the global new and delete operators, uses
placement new to tag memory allocations, and uses `std::vector` with a special allocator that tags
and tracks memory plus more.

Notice that replacing the global new and delete is not required. For example to just track
allocations of a few STL containers somewhere in a program, just replace the `std::allocator` of
those objects.

The example also uses a single global `memory` object, but multiple `memory` objects can be used and
it could be a useful way to separate statistics for different parts of a program instead of tagging
allocations with different names.



## String literals

The `allocator_base`, `allocator`, `std_allocator`, `allocator_tag` classes have constructors with a
`char const* name_string_literal` for the tag name. They store the char pointer as is, so it must
point to something permanent. Use hard-coded string literals:

    memory_track::memory<> memory;
    
    auto good = allocator_for(memory, "hello"); // good!
    
    char name[] = "hello";
    auto danger = allocator_for(memory, name); // danger!
    // dangling pointer to name if the danger-allocator is used after name goes out of scope
    

This is odd, but it means constructors can be constexpr, the objects are small, and they do not need
to allocate memory to copy the name.

`memory` does not require tag names to be string literals, you can write an allocator that use any
kind of name. 



## memory_track::statistics

The `memory_track::statistics` type has these unsigned integers:

- *count_now:* current number of allocations
- *bytes_now:* number of bytes allocated now (current memory use)
- *count_peak:* highest recorded number of allocations
- *bytes_peak:* highest recorded number of bytes allocated (peak memory use)
- *success_count:* total number of allocations that succeeded
- *success_bytes:* total sum of all bytes of all allocations that succeeded
- *failed_count:* total number of allocations that failed (returned 0 or threw an exception)
- *failed_bytes:* total sum of all bytes of all allocations that failed
- *smallest_bytes:* smallest allocation ever made
- *largest_bytes:* largest allocation ever made

These numbers are collected by `memory_track::memory` for all allocations (total statistics),
segmented into size ranges (for example allocations of 65-128 bytes), and segmented by name (for
example all allocations tagged with "example").

The functions in `water/memory_track/out.hpp` is probably a good place to see how to get the
statistics from the `memory` object.



## Replacing the global operator new and delete

Doing this creates problems:

The `memory` object must exist as long as the global new and delete are used. It has a constexpr
constructor, but it does have a destructor. One way to solve this is in `water/memory_track/example/`.

The `memory` member functions `cookies_to`, `statistics_by_bytes_to` and `statistics_by_name_to`
take a function object as argument, and this function object is called while the internal mutex of
`memory` is locked. Normally the function object would copy statistic data to use later. But if the
function object does something that allocates memory with new, it will deadlock. So for example it
cannot use a `std::vector`. The `memory_track::container` class can be used to avoid deadlocks.

The callback is also used while the mutex is locked, so it also cannot allocates memory with new.