// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STD_ALLOCATOR_HPP
#define WATER_STD_ALLOCATOR_HPP
#include <water/water.hpp>
#include <water/new_here.hpp>
#include <water/swap.hpp>
#ifndef WATER_NO_STD
    #include <type_traits>
#endif
namespace water {

/*

Make a std::allocator from a water::allocator.

    class my_allocator
    {
    public:
        template<typename type_>
        type_* allocate(size_t count) noexcept(false); // cannot return 0, must throw

        template<typename type_>
        void free(void *pointer, size_t count) noexcept:
    
        bool operator==(my_allocator const& a) const noexcept;
        bool operator!=(my_allocator const& a) const noexcept;
    };

    std::vector<int, water::std_allocator<int, my_allocator>> v;

*/

namespace _ { namespace std_allocators {

    // the standard says propagate_on_container_copy_assignment etc should be std::true_type and
    // not just any type containing type=bool and value=true/false

    #ifndef WATER_NO_STD
 
    using std::true_type;
    using std::false_type;

    #else

    template<bool value_>
    struct bool_constant {
        using type = bool;
        static bool constexpr value = value_;
    };

    using true_type = bool_constant<true>;
    using false_type = bool_constant<false>;

    #endif
}}


template<typename type_, typename allocator_>
class std_allocator
{   
public:
    using value_type = type_;
    using pointer = type_*;
    using const_pointer = type_ const*;
    using reference = value_type&;
    using const_reference = value_type const&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    
    template<typename other_>
    struct rebind {
        using other = std_allocator<other_, allocator_>;
    };
    
    using propagate_on_container_copy_assignment = _::std_allocators::true_type;
    using propagate_on_container_move_assignment = _::std_allocators::true_type;
    using propagate_on_container_swap = _::std_allocators::true_type;
    using is_always_equal = _::std_allocators::false_type;

public:
    // not std
    using underlying_allocator = allocator_;
    
private:
    underlying_allocator my{};

public:

    constexpr std_allocator() = default;
    constexpr std_allocator(std_allocator const&) = default;
    constexpr std_allocator(std_allocator&&) = default;
    
    constexpr std_allocator(underlying_allocator const& a)  :
        my{a}
    {}
    
    template<typename other_>
    constexpr std_allocator(std_allocator<other_, allocator_> const& a)  :
        my{a.underlying()}
    {}
    
    std_allocator& operator=(std_allocator const&) = default;
    std_allocator& operator=(std_allocator&&) = default;
    
    pointer address(value_type& a) noexcept {
        return &a;
    }
    
    const_pointer address(value_type const& a) noexcept {
        return &a;
    }
    
    pointer allocate(size_t count) noexcept(noexcept(this->underlying().template allocate<value_type>(count))) {
        return my.template allocate<value_type>(count);  
    }
    
    pointer allocate(size_t count, const void* /*hint*/) noexcept(noexcept(this->allocate(count))) {
        return allocate(count);
    }
    
    void deallocate(value_type *pointer, size_t count) noexcept {
        my.template free<value_type>(pointer, count);
    }
    
    constexpr size_type max_size() const noexcept {
        return static_cast<size_type>(-1);
    }
    
    template<typename to_, typename ...arguments_>
    void construct(to_ *to, arguments_&&... arguments) {
        new (here(to)) to_{static_cast<arguments_&&>(arguments)...};
    }
    
    template<typename to_>
    void destroy(to_ *to) noexcept {
        to->~to_();
    }

public:
    // not std
    
    constexpr underlying_allocator& underlying() noexcept {
        return my;
    }
    
    constexpr underlying_allocator const& underlying() const noexcept {
        return my;
    }
};


template<typename type1_, typename type2_, typename allocator_>
bool operator==(std_allocator<type1_, allocator_> const& a1, std_allocator<type2_, allocator_> const& a2) {
    return a1.underlying() == a2.underlying();
}

template<typename type1_, typename type2_, typename allocator_>
bool operator!=(std_allocator<type1_, allocator_> const& a1, std_allocator<type2_, allocator_> const& a2) {
    return a1.underlying() != a2.underlying();
}


template<typename type_, typename allocator_>
void swap(std_allocator<type_, allocator_>& a1, std_allocator<type_, allocator_>& a2) {
    swap_from_swap(a1.underlying(), a2.underlying());
}


}
#endif
