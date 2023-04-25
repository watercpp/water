// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MEMORY_TRACK_ALLOCATOR_BASE_HPP
#define WATER_MEMORY_TRACK_ALLOCATOR_BASE_HPP
#include <water/memory_track/allocator_tag.hpp>
namespace water { namespace memory_track {

/*

Allocates from memory_track::memory
Keeps track of the allocation function and optionally an "address", free must be made with the matching function + "address"

IMPORTANT:
The char const* name_string_literal pointer is stored as is in this, just the pointer.
So it must point to a literal or something that exists while the allocator does.
See "String literals" in the readme.md


This is a base class, the derived_ class should look like this:

    class derived : public allocator_base<derived, memory<>, exception_type>
    {
    public:
        memory<>* memory_pointer();
        memory<> const* memory_pointer() const;
        
        // unless something beyond the memory_pointer needs to be compared, reuse the == and != from allocator_base
        using allocator_base<derived, memory<>, exception_type>::operator==;
        using allocator_base<derived, memory<>, exception_type>::operator!=;
    };


When the memory_track::memory is a global, the derived class can return that global from the
memory_pointer functions, and the allocator will be a little bit smaller plus the constructors
can be constexpr.

Look at allocator.hpp

*/

template<typename derived_, typename memory_, typename exception_ = void>
class allocator_base
{
public:
    using memory_type = memory_;
    using tag_type = typename memory_type::tag_type;
    using exception = exception_;
    static bool constexpr is_noexcept = equal<void, exception>;

private:
    char const* myname = 0;
    tag_type mytag {};
    void const* myaddress = 0;

public:

    constexpr allocator_base() = default;

    constexpr allocator_base(char const* name_string_literal, tag_type const& tag = {}) :
        myname{name_string_literal},
        mytag{tag}
    {}
    
    bool operator==(allocator_base const& a) const noexcept {
        return static_cast<derived_ const*>(this)->memory_pointer() == static_cast<derived_ const*>(&a)->memory_pointer() && mytag == a.mytag;
    }
    
    bool operator!=(allocator_base const& a) const noexcept {
        return !(*this == a);
    }
    
    char const* name() const noexcept {
        return myname;
    }

    allocator_base& name(char const* name_string_literal) noexcept {
        myname = name_string_literal;
        return *this;
    }

    tag_type const& tag() const noexcept {
        return mytag;
    }

    allocator_base& tag(tag_type const& a) noexcept {
        mytag = a;
        return *this;
    }

    void const* allocator_tag_address() const noexcept {
        return myaddress;
    }

    allocator_base& allocator_tag_address(void const* a) noexcept {
        // when freeing, this address must match what it was when memory was allocated. default is 0.
        myaddress = a;
        return *this;
    }

    void* allocate(size_t bytes) noexcept(is_noexcept) {
        return allocate_do(bytes, 0, tag_function<void>);
    }

    void free(void *pointer, size_t bytes) noexcept {
        free_do(pointer, bytes, 0, tag_function<void>);
    }

    template<typename type_>
    type_* allocate() noexcept(is_noexcept) {
        return static_cast<type_*>(allocate_do(sizeof(type_), alignof(type_), tag_function<type_>));
    }

    template<typename type_>
    void free(void *pointer) noexcept {
        free_do(pointer, sizeof(type_), alignof(type_), tag_function<type_>);
    }

    template<typename type_>
    type_* allocate(size_t count) noexcept(is_noexcept) {
        return static_cast<type_*>(allocate_do(sizeof(type_) * count, alignof(type_), tag_function<type_[1]>));
    }

    template<typename type_>
    void free(void *pointer, size_t count) noexcept {
        free_do(pointer, sizeof(type_) * count, alignof(type_), tag_function<type_[1]>);
    }

private:

    void* allocate_do(size_t bytes, size_t align, void (*tag)()) noexcept(is_noexcept) {
        auto memory = static_cast<derived_*>(this)->memory_pointer();
        auto pointer = memory ? memory->allocate(bytes, align, myname, mytag, allocator_tag(myaddress ? myaddress : memory, tag)) : 0;
        if(!pointer) throw_if<exception>();
        return pointer;
    }

    void free_do(void *pointer, size_t bytes, size_t align, void (*tag)()) noexcept {
        auto memory = static_cast<derived_*>(this)->memory_pointer();
        ___water_assert(memory);
        memory->free(pointer, bytes, align, myname, mytag, allocator_tag(myaddress ? myaddress : memory, tag));
    }

    template<typename type_>
    static void tag_function()
    {}
};


template<typename derived_, typename memory_> using allocator_base_throw = allocator_base<derived_, memory_, exception>;
template<typename derived_, typename memory_> using allocator_base_nothrow = allocator_base<derived_, memory_>;


}}
#endif
