// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TESTS_STD_ALLOCATOR_HPP
#define WATER_TESTS_STD_ALLOCATOR_HPP
#ifdef WATER_NO_STD
namespace water { namespace tests {
    void std_allocator_all() {}
}}
#else
#include <water/test.hpp>
#include <water/std_allocator.hpp>
#include <water/types.hpp>
//#include <water/str/out_trace.hpp>
//#include <water/str/type_name.hpp>
#include <vector>
#include <list>
#include <memory>
namespace water { namespace tests {


struct std_allocator_test_allocator
{
    template<typename type_>
    type_* allocate(size_t count) noexcept(false) {
        //str::out_trace{} << "allocate " << str::type_name<type_>() << " count=" << count;
        return static_cast<type_*>(::operator new(sizeof(type_) * count));
    }

    template<typename type_>
    void free(void *pointer, size_t /*count*/) noexcept {
        //str::out_trace{} << "free " << str::type_name<type_>() << " pointer=" << pointer << " count=" << count;
        ::operator delete(pointer);
    }
    
    bool operator==(std_allocator_test_allocator const&) const {
        return true;
    }
    
    bool operator!=(std_allocator_test_allocator const& a) const {
        return !operator==(a);
    }
};


template<typename container_>
inline void std_allocator_test_container() {
    container_
        c1,
        c2{1, 2, 3},
        c3{typename container_::allocator_type{}},
        c4{c1},
        c5{container_{}},
        c6{c2.get_allocator()};
        
    swap(c3, c4);
    c1 = c2;
    c5 = container_{};
    for(int i = 0; i != 123; ++i)
        c1.push_back(typename container_::value_type{});
    c2.assign(c1.begin(), c1.end());
    c3.resize(12);
}


inline void std_allocator_all() {
    std_allocator_test_container<std::vector<int, std_allocator<int, std_allocator_test_allocator>>>();
    std_allocator_test_container<std::list<int, std_allocator<int, std_allocator_test_allocator>>>();
    
    using vector = std::vector<int>;
    std_allocator<vector, std_allocator_test_allocator> a;
    vector *v = a.allocate(1);
    a.construct(v, 1, 2, 3);
    a.destroy(a.address(*v));
    a.deallocate(v, 1);
    
    using allocator = std_allocator<vector, std_allocator_test_allocator>;
    using traits = std::allocator_traits<allocator>;
    static_assert(equal<traits::value_type, allocator::value_type>, "");
    static_assert(equal<std_allocator<int, std_allocator_test_allocator>, traits::rebind_alloc<int>>, "");
    static_assert(traits::propagate_on_container_copy_assignment::value, "");
    static_assert(traits::propagate_on_container_move_assignment::value, "");
    static_assert(traits::propagate_on_container_swap::value, "");
    static_assert(!traits::is_always_equal::value, "");
    
    v = traits::allocate(a, 1);
    traits::construct(a, v, 1, 2, 3);
    traits::destroy(a, v);
    traits::deallocate(a, v, 1);
}


}}
#endif
#endif
