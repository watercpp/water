# water::memory_track

This is used to track memory allocations, to figure out how an application uses dynamic memory. It
collects statistics and it is possible to iterate over all current allocations.

It can also be used to detect some errors: Memory leaks, under and overruns, double frees, use of 
uninitialized or freed memory.

To get started, look at the `water/memory_track/example/example.hpp` and `water/memory_track/example/example.cpp` files.

All memory allocations are tagged. The tag is a class that can be replaced. The default tag class
has a name, and the replacement must also have a name. Statistics are collected for each name.
It might be useful to have a timestamp for the tag, to track how long allocations exist.


## Lifetime

`memory` has a constexpr constructor, but it does have a destructor. It could be safe to use after it
has been destroyed, if the mutex can be used after it has been destroyed. But this is better:

    threads::spin_once<> memory_track_once;
    char memory_track_buffer[sizeof(memory_track::memory<>)];

    memory_track::memory<>& memory_track() {
        memory_track_once([](){new(memory_track_buffer) memory_track::memory<>;});
        return *static_cast<memory_track::memory<>*>(static_cast<void*>(memory_track_buffer));
    }
