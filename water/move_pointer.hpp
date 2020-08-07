// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MOVE_POINTER_HPP
#define WATER_MOVE_POINTER_HPP
#include <water/swap.hpp>
namespace water {

/*

move_pointer<some_type, allocator_type> p(allocator);
new(p) some_type("hello");
p->function();

allocator_ must have
    template<typename type_> type_* allocate()
    template<typename type_> void free(void*)

*/

template<typename type_, typename allocator_>
class move_pointer
{
public:
    using value_type = type_;
    using element_type = value_type;
    using pointer = value_type*;
    using allocator_type = allocator_;

private:
    value_type *my = 0;
    allocator_type myallocator {};

public:
    constexpr move_pointer() = default;
    move_pointer(move_pointer const&) = delete;
    move_pointer& operator=(move_pointer const&) = delete;

    explicit move_pointer(allocator_type const& a) :
        myallocator{a}
    {}

    explicit move_pointer(pointer p, allocator_type const& a = {}) :
        my{p},
        myallocator{a}
    {}

    move_pointer(move_pointer&& a) :
        my{a.my},
        myallocator{static_cast<allocator_type&&>(a.myallocator)}
    {
        a.my = 0;
    }

    ~move_pointer() {
        destroy();
    }

    move_pointer& operator=(move_pointer&& a) {
        swap(a);
        return *this;
    }

    move_pointer& operator=(decltype(nullptr) a) {
        reset();
        return *this;
    }

    explicit operator bool() const {
        return my != 0;
    }

    pointer operator->() const {
        return my;
    }

    value_type& operator*() const {
        return *my;
    }

    pointer get() const {
        return my;
    }

    pointer release() {
        pointer r = my;
        my = 0;
        return r;
    }

    void reset(pointer a = 0) {
        destroy();
        my = a;
    }

    void swap(move_pointer& a) {
        swap_from_swap(my, a.my);
        swap_from_swap(myallocator, a.myallocator);
    }

    void* operator_new() {
        destroy();
        return my = myallocator.template allocate<value_type>();
    }

    void operator_delete() {
        myallocator.template free<value_type>(my);
        my = 0;
    }

private:
    void destroy() {
        if(my) {
            my->~value_type();
            myallocator.template free<value_type>(my);
        }
    }
    
};

template<typename type_, typename allocator_>
void swap(move_pointer<type_, allocator_>& a, move_pointer<type_, allocator_>& b) {
    a.swap(b);
}

template<typename type_, typename allocator_>
move_pointer<type_, allocator_> move_pointer_from(type_ *pointer, allocator_ const& allocator) {
    return move_pointer<type_, allocator_>{pointer, allocator};
}

} // namespace water

template<typename type_, typename allocator_>
void* operator new(decltype(sizeof(0)), water::move_pointer<type_, allocator_>& a) {
    return a.operator_new();
}

template<typename type_, typename allocator_>
void operator delete(void*, water::move_pointer<type_, allocator_>& a) {
    a.operator_delete();
}

#endif
