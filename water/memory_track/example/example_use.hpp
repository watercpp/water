// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MEMORY_TRACK_EXAMPLE_EXAMPLE_USE_HPP
#define WATER_MEMORY_TRACK_EXAMPLE_EXAMPLE_USE_HPP
#include <water/memory_track/example/example.hpp>
#include <vector>
#include <memory>
#include <water/move_pointer.hpp>
namespace water { namespace memory_track { namespace example {

/*

A few different ways to use the things from example.hpp and example.cpp 

IMPORTANT:
Notice that the tags are string literals ("tag" below).
See "String literals" in the readme.md

*/


class type
{
    int my = 0;
public:
    constexpr type() = default;
    constexpr explicit type(int a) : my{a} {}
    void set(int a) { my = a; }
};


inline void example_use() {

    // example.cpp will replace operator new, so a plain vector will use water::memory_track
    std::vector<type> v1;
    v1.resize(123);
    
    // with std_allocator from example.hpp, the allocations can be tagged with a name
    std::vector<type, std_allocator<type>> v2{std_allocator<type>{"tag"}};
    v2.resize(123);
    
    // using memory_track::std_allocator also works, but it needs more typing and the constructor
    // is not constexpr because it needs the object from the memory() function
    using allocator3 = memory_track::std_allocator<type, memory_type>;
    std::vector<type, allocator3> v3{allocator3{memory(), "tag"}};
    v3.resize(123);
    
    // using placement new and delete to tag operator new
    type *i1 = new(track("tag")) type;
    i1->set(123);
    delete i1;
    
    // placement new again
    std::unique_ptr<type> u1{new(track("tag")) type};
    u1->set(123);
    
    // water::move_pointer using the example::allocator
    // (this type of allocator is used everywhere in water)
    move_pointer<type, allocator> m1{"tag"};
    new(m1) type;
    m1->set(123);
    
    // using memory_track::allocator also works, but it needs more typing and the constructor
    // is not constexpr because it needs the object from the memory() function
    using allocator4 = memory_track::allocator<memory_type>;
    move_pointer<type, allocator4> m2{allocator4{memory(), "tag"}};
    new(m2) type;
    m2->set(123);
    
    // alternate way to use memory_track::allocator
    auto allocator5 = allocator_for(memory(), "tag");
    move_pointer<type, decltype(allocator5)> m3{allocator5};
    new(m3) type;
    m3->set(123);
}

}}}
#endif
