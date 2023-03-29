// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ALLOCATOR_POINTER_HPP
#define WATER_ALLOCATOR_POINTER_HPP
namespace water {

/*

Hold a pointer to an underlying allocator. Useful if you dont want to copy an allocator.

*/

template<typename allocator_>
class allocator_pointer
{
    using size_t = decltype(sizeof(0));

public:
    using allocator_type = allocator_;

private:
    allocator_type *my;

public:
    explicit constexpr allocator_pointer(allocator_type *a = 0) :
        my{a}
    {}

    explicit allocator_pointer(allocator_type& a) :
        my{&a}
    {}

    allocator_type* allocator() const noexcept {
        return my;
    }

    void allocator(allocator_type *a) noexcept {
        my = a;
    }
    
    bool operator==(allocator_pointer const& a) const noexcept {
        return my == a.my;
    }
    
    bool operator!=(allocator_pointer const& a) const noexcept {
        return my != a.my;
    }

public:
    // allocator
    void* allocate(size_t bytes) noexcept(noexcept(allocator_pointer{}.allocator()->allocate(bytes))) {
        return my->allocate(bytes);
    }

    void free(void *pointer, size_t size) noexcept {
        my->free(pointer, size);
    }

    template<typename type_>
    type_* allocate() noexcept(noexcept(allocator_pointer{}.allocator()->template allocate<type_>())) {
        return my->template allocate<type_>();
    }

    template<typename type_>
    void free(void *pointer) noexcept {
        my->template free<type_>(pointer);
    }

    template<typename type_>
    type_* allocate(size_t count) noexcept(noexcept(allocator_pointer{}.allocator()->template allocate<type_>(count))) {
        return my->template allocate<type_>(count);
    }

    template<typename type_>
    void free(void *pointer, size_t count) noexcept {
        my->template free<type_>(pointer, count);
    }
};

template<typename allocator_>
allocator_pointer<allocator_> allocator_pointer_from(allocator_ *a) {
    return allocator_pointer<allocator_>(a);
}

template<typename allocator_>
allocator_pointer<allocator_> allocator_pointer_from(allocator_& a) {
    return allocator_pointer<allocator_>(a);
}

}
#endif
