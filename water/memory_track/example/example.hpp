// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MEMORY_TRACK_EXAMPLE_EXAMPLE_HPP
#define WATER_MEMORY_TRACK_EXAMPLE_EXAMPLE_HPP
#include <water/memory_track/memory_track.hpp>
#include <water/throw_if.hpp>

/*

Basic example that replaces the global operator new + delete.
When the program ends, statistics are written to water::trace (the debugger output)

To use it, compile example.cpp into your program. The easiest method is probably to include the cpp
file from one of the programs cpp files.

#define WATER_DEBUG so that the program breakpoints if memory_track detects an error.

If you get access violation errors for memory addresses like 0xdeaddead it means you are using
uninitialized memory or memory that has been freed. memory_track::memory will fill everything with
0xdead.

Look at example_use.hpp for example use of the things from this file and example.cpp.

*/

namespace water { namespace memory_track { namespace example {

    using memory_type = memory_track::memory<>;

    memory_type& memory();

    void trace_memory_statistics();


    
    // water::allocator type allocator, can be used with other things in water, and the std_allocator below.
    // Compared to memory_track::allocator this one is hard-coded to use memory() so it has a default constructor.
    
    class allocator : public memory_track::allocator_base<allocator, memory_type, memory_track::exception>
    {
        using base = memory_track::allocator_base<allocator, memory_type, memory_track::exception>;
    public:
        constexpr allocator() = default;
    
        constexpr allocator(char const* string_literal) :
            base{string_literal}
        {}

        memory_type* memory_pointer() const {
            return &memory();
        }
    
        using base::operator==;
        using base::operator!=;
    };

    
    
    // std::allocator type allocator, can be used with STL containers

    template<typename type_>
    using std_allocator = water::std_allocator<type_, allocator>;



    // Below is an example of placement new and delete with a tracking tag name.
    // auto x = new(track("name")) type;
    // delete x;

    struct track {
        char const *name;
        constexpr track(char const *name) : name{name} {}
    };

}}}


// Notice that the allocator_tag "new" and "new[]" is used, same as the example.cpp new/delete operators.
// This enables the regular delete to be used after these new operators.

inline void* operator new(std::size_t s, water::memory_track::example::track t) {
    void *r = water::memory_track::example::memory().allocate(s, 0, t.name, {}, "new");
    if(!r) water::throw_if<std::bad_alloc>();
    return r;
}

inline void operator delete(void *p, std::size_t s, water::memory_track::example::track t) noexcept {
    water::memory_track::example::memory().free(p, s, 0, t.name, {}, "new");
}

inline void operator delete(void *p, water::memory_track::example::track t) noexcept {
    water::memory_track::example::memory().free(p, 0, 0, t.name, {}, "new");
}

inline void* operator new[](std::size_t s, water::memory_track::example::track t) {
    void *r = water::memory_track::example::memory().allocate(s, 0, t.name, {}, "new[]");
    if(!r) water::throw_if<std::bad_alloc>();
    return r;
}

inline void operator delete[](void *p, std::size_t s, water::memory_track::example::track t) noexcept {
    water::memory_track::example::memory().free(p, s, 0, t.name, {}, "new[]");
}

inline void operator delete[](void *p, water::memory_track::example::track t) noexcept {
    water::memory_track::example::memory().free(p, 0, 0, t.name, {}, "new[]");
}


#endif
