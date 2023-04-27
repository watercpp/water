// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TESTS_VECTORS_CONSTRUCT_HPP
#define WATER_TESTS_VECTORS_CONSTRUCT_HPP
#include <water/tests/vectors/bits.hpp>
#include <water/begin_end.hpp>
#include <water/allocator_pointer.hpp>
namespace water { namespace tests { namespace vectors {

// construct operator= swap

template<typename vector_, bool if_, typename = void>
struct construct_one_if_default_constructor
{
    static void do_it(typename vector_::allocator_type const& a) {
        vector_ v{3, a};
        ___water_test(v.size() == 3);
    }
    static void do_it() {
        vector_ v{static_cast<typename vector_::size_type>(3)};
        ___water_test(v.size() == 3);
    }
};

template<typename vector_>
struct construct_one_if_default_constructor<vector_, true, to_void<decltype(typename vector_::value_type{3})>>
{
    static void do_it(typename vector_::allocator_type const& a) {
        vector_ v{3, a};
        ___water_test(v.size() == 3);
    }
    static void do_it() {
        vector_ v{3};
        #ifdef WATER_NO_STD
        ___water_test(v.size() == 3);
        #else
        ___water_test(v.size() == 1);
        #endif
    }
};

template<typename vector_>
struct construct_one_if_default_constructor<vector_, false, void>
{
    static void do_it(typename vector_::allocator_type const&) {}
    static void do_it() {}
};


template<typename vector_, bool default_constructor_ = true>
void construct_one(typename vector_::value_type const& value, typename vector_::allocator_type const& a) {
    // temporary::vector and regular vector can do this
    using value_type = typename vector_::value_type;
    value_type i[5] = {value, value, value, value, value};
    unsigned is = 5;
    
    vector_
        v0,
        v1{a},
        v2{v0},
        v3{static_cast<vector_&&>(v2)},
        v4{{value, value}, a},
        //v5{1, a},
        v6{1, value, a},
        v7{i, i + is, a},
        v8{i, is, a},
        v9{begin_end_from(i), a},
        v10{i, a},
        
        v11{downgrade_iterators::input_from(i), downgrade_iterators::input_from(i + is), a},
        v12{downgrade_iterators::input_from(i), is, a},
        v13{downgrade_iterators::input_proxied_from(i), downgrade_iterators::input_proxied_from(i + is), a},
        v14{downgrade_iterators::input_proxied_from(i), is, a},
        v15{begin_end_from(downgrade_iterators::input_proxied_from(i), downgrade_iterators::input_proxied_from(i + is)), a},
        
        v16{downgrade_iterators::forward_proxied_const_from(i), downgrade_iterators::forward_proxied_const_from(i + is), a},
        v17{downgrade_iterators::forward_proxied_const_from(i), is, a},
        v18{downgrade_iterators::forward_const_from(i), downgrade_iterators::forward_const_from(i + is), a},
        v19{downgrade_iterators::forward_const_from(i), is, a},
        v20{begin_end_from(downgrade_iterators::forward_const_from(i), downgrade_iterators::forward_const_from(i + is)), a},
        
        v21{downgrade_iterators::bidirectional_proxied_const_from(i), downgrade_iterators::bidirectional_proxied_const_from(i + is), a},
        v22{downgrade_iterators::bidirectional_proxied_const_from(i), is, a},
        v23{downgrade_iterators::bidirectional_const_from(i), downgrade_iterators::bidirectional_const_from(i + is), a},
        v24{downgrade_iterators::bidirectional_const_from(i), is, a},
        v25{begin_end_from(downgrade_iterators::forward_const_from(i), downgrade_iterators::forward_const_from(i + is)), a},
        
        v26{downgrade_iterators::random_access_proxied_const_from(i), downgrade_iterators::random_access_proxied_const_from(i + is), a},
        v27{downgrade_iterators::random_access_proxied_const_from(i), is, a},
        v28{downgrade_iterators::random_access_const_from(i), downgrade_iterators::random_access_const_from(i + is), a},
        v29{downgrade_iterators::random_access_const_from(i), is, a},
        v30{begin_end_from(downgrade_iterators::forward_const_from(i), downgrade_iterators::forward_const_from(i + is)), a},
        
        // empty:
        v31{downgrade_iterators::input_proxied_from(i), downgrade_iterators::input_proxied_from(i), a},
        v32{downgrade_iterators::input_proxied_from(i), 0, a},
        v33{static_cast<value_type*>(0), static_cast<value_type*>(0), a},
        v34{static_cast<value_type*>(0), 0, a},
        v35{begin_end_from(static_cast<value_type*>(0), 0), a};
    
    ___water_test(v0.size() == 0);
    ___water_test(v1.size() == 0);
    ___water_test(v2.size() == 0);
    ___water_test(v3.size() == 0);
    ___water_test(v4.size() == 2);
    //___water_test(v5.size() == 1);
    ___water_test(v6.size() == 1);
    ___water_test(v7.size() == is);
    ___water_test(v8.size() == is);
    ___water_test(v9.size() == is);
    ___water_test(v10.size() == is);
    
    ___water_test(v11.size() == is);
    ___water_test(v12.size() == is);
    ___water_test(v13.size() == is);
    ___water_test(v15.size() == is);
    
    ___water_test(v16.size() == is);
    ___water_test(v17.size() == is);
    ___water_test(v18.size() == is);
    ___water_test(v19.size() == is);
    ___water_test(v20.size() == is);
    
    ___water_test(v21.size() == is);
    ___water_test(v22.size() == is);
    ___water_test(v23.size() == is);
    ___water_test(v24.size() == is);
    ___water_test(v25.size() == is);
    
    ___water_test(v26.size() == is);
    ___water_test(v27.size() == is);
    ___water_test(v28.size() == is);
    ___water_test(v29.size() == is);
    ___water_test(v30.size() == is);
    
    ___water_test(v31.size() == 0);
    ___water_test(v32.size() == 0);
    ___water_test(v33.size() == 0);
    ___water_test(v34.size() == 0);
    ___water_test(v35.size() == 0);

    v0 = v1;
    v0 = v11;
    ___water_test(v0.size() == v11.size());
    
    auto moved = v11.begin();
    v1 = static_cast<vector_&&>(v11);
    ___water_test(v11.size() == 0 && moved == v1.begin());
    
    v4 = i;
    ___water_test(v4.size() == is);
    
    v4 = {value, value};
    ___water_test(v4.size() == 2);
    
    v4 = {};
    ___water_test(v4.empty());
    
    auto d21 = v21.data();
    auto d22 = v22.data();
    swap(v21, v22);
    ___water_test(v21.data() == d22 && v22.data() == d21);
    
    construct_one_if_default_constructor<vector_, default_constructor_>::do_it(a);
}


template<typename vector_, bool default_constructor_ = true>
void construct_one(typename vector_::value_type const& value) {
    // not for temporary::vector
    using value_type = typename vector_::value_type;
    value_type i[5] = {value, value, value, value, value};
    unsigned is = 5;
    
    #ifndef WATER_NO_STD
    
    // std::initializer_list
    vector_
        v0{value},
        v1{value, value},
        v2{value, value, value};
    ___water_test(v0.size() == 1);
    ___water_test(v1.size() == 2);
    ___water_test(v2.size() == 3);
    
    #endif
    
    vector_
        v7{i, i + is},
        v8{i, is},
        v9{begin_end_from(i)},
        v10{i},
        
        v11{downgrade_iterators::input_from(i), downgrade_iterators::input_from(i + is)},
        v12{downgrade_iterators::input_from(i), is},
        v13{downgrade_iterators::input_proxied_from(i), downgrade_iterators::input_proxied_from(i + is)},
        v14{downgrade_iterators::input_proxied_from(i), is},
        v15{begin_end_from(downgrade_iterators::input_proxied_from(i), downgrade_iterators::input_proxied_from(i + is))},
        
        v16{downgrade_iterators::forward_proxied_const_from(i), downgrade_iterators::forward_proxied_const_from(i + is)},
        v17{downgrade_iterators::forward_proxied_const_from(i), is},
        v18{downgrade_iterators::forward_const_from(i), downgrade_iterators::forward_const_from(i + is)},
        v19{downgrade_iterators::forward_const_from(i), is},
        v20{begin_end_from(downgrade_iterators::forward_const_from(i), downgrade_iterators::forward_const_from(i + is))},
        
        v21{downgrade_iterators::bidirectional_proxied_const_from(i), downgrade_iterators::bidirectional_proxied_const_from(i + is)},
        v22{downgrade_iterators::bidirectional_proxied_const_from(i), is},
        v23{downgrade_iterators::bidirectional_const_from(i), downgrade_iterators::bidirectional_const_from(i + is)},
        v24{downgrade_iterators::bidirectional_const_from(i), is},
        v25{begin_end_from(downgrade_iterators::forward_const_from(i), downgrade_iterators::forward_const_from(i + is))},
        
        v26{downgrade_iterators::random_access_proxied_const_from(i), downgrade_iterators::random_access_proxied_const_from(i + is)},
        v27{downgrade_iterators::random_access_proxied_const_from(i), is},
        v28{downgrade_iterators::random_access_const_from(i), downgrade_iterators::random_access_const_from(i + is)},
        v29{downgrade_iterators::random_access_const_from(i), is},
        v30{begin_end_from(downgrade_iterators::forward_const_from(i), downgrade_iterators::forward_const_from(i + is))},
        
        // empty:
        v31{downgrade_iterators::input_proxied_from(i), downgrade_iterators::input_proxied_from(i)},
        v32{downgrade_iterators::input_proxied_from(i), 0},
        v33{static_cast<value_type*>(0), static_cast<value_type*>(0)},
        v34{static_cast<value_type*>(0), 0},
        v35{begin_end_from(static_cast<value_type*>(0), 0)};
    
    ___water_test(v7.size() == is);
    ___water_test(v8.size() == is);
    ___water_test(v9.size() == is);
    ___water_test(v10.size() == is);
    
    ___water_test(v11.size() == is);
    ___water_test(v12.size() == is);
    ___water_test(v13.size() == is);
    ___water_test(v15.size() == is);
    
    ___water_test(v16.size() == is);
    ___water_test(v17.size() == is);
    ___water_test(v18.size() == is);
    ___water_test(v19.size() == is);
    ___water_test(v20.size() == is);
    
    ___water_test(v21.size() == is);
    ___water_test(v22.size() == is);
    ___water_test(v23.size() == is);
    ___water_test(v24.size() == is);
    ___water_test(v25.size() == is);
    
    ___water_test(v26.size() == is);
    ___water_test(v27.size() == is);
    ___water_test(v28.size() == is);
    ___water_test(v29.size() == is);
    ___water_test(v30.size() == is);
    
    ___water_test(v31.size() == 0);
    ___water_test(v32.size() == 0);
    ___water_test(v33.size() == 0);
    ___water_test(v34.size() == 0);
    ___water_test(v35.size() == 0);
        
    construct_one_if_default_constructor<vector_, default_constructor_>::do_it();
}


template<typename vector_, bool default_constructor_ = true>
void construct_one(typename vector_::value_type const& value, typename vector_::allocator_type const& a, typename vector_::sizer_type const& s) {
    // water::vector
    construct_one<vector_, default_constructor_>(value);
    construct_one<vector_, default_constructor_>(value, a);
    vector_
        v4(s),
        v5(a, s),
        v6(s, a);    
}


template<typename a_, typename b_>
struct result_if_equal {
};

template<typename a_>
struct result_if_equal<a_, a_> {
    static bool constexpr result = true;
};


template<typename value_>
void construct_with_template_deduction(value_ const& value) {
    test_unused(value);
    
    #ifdef __cpp_deduction_guides

    water::allocator allocator;
    water::allocator_pointer allocator_pointer{allocator};
    value_ i[5] = {value, value, value, value, value};
    size_t is = 5;

    #ifndef WATER_NO_STD

    vector v0{value};
    vector v1{value, value};
    vector v2{value, value, value};
    static_assert(result_if_equal<vector<value_>, decltype(v0)>::result, "");
    static_assert(result_if_equal<vector<value_>, decltype(v1)>::result, "");
    static_assert(result_if_equal<vector<value_>, decltype(v2)>::result, "");

    #endif

    vector v3(i);
    vector v4(i, allocator_pointer);
    static_assert(result_if_equal<vector<value_>, decltype(v3)>::result, "");
    static_assert(result_if_equal<vector<value_, decltype(allocator_pointer)>, decltype(v4)>::result, "");

    vector v6(i, i + is);
    vector v7(downgrade_iterators::forward_proxied_const_from(i), downgrade_iterators::forward_proxied_const_from(i + is));
    vector v8(i, i + is, allocator_pointer);
    vector v9(downgrade_iterators::forward_proxied_const_from(i), downgrade_iterators::forward_proxied_const_from(i + is), allocator_pointer);
    static_assert(result_if_equal<vector<value_>, decltype(v6)>::result, "");
    static_assert(result_if_equal<vector<value_>, decltype(v7)>::result, "");
    static_assert(result_if_equal<vector<value_, decltype(allocator_pointer)>, decltype(v8)>::result, "");
    static_assert(result_if_equal<vector<value_, decltype(allocator_pointer)>, decltype(v9)>::result, "");

    vector v10(i, is);
    vector v11(downgrade_iterators::forward_proxied_const_from(i), is);
    vector v12(i, is, allocator_pointer);
    vector v13(downgrade_iterators::forward_proxied_const_from(i), is, allocator_pointer);
    static_assert(result_if_equal<vector<value_>, decltype(v10)>::result, "");
    static_assert(result_if_equal<vector<value_>, decltype(v11)>::result, "");
    static_assert(result_if_equal<vector<value_, decltype(allocator_pointer)>, decltype(v12)>::result, "");
    static_assert(result_if_equal<vector<value_, decltype(allocator_pointer)>, decltype(v13)>::result, "");

    #endif
}


struct construct {
    construct() {
        construct_one<vector<int>>(0, {}, {});
        construct_one<vector<size_t>>(0, {}, {});
        construct_one<vector<double>>(0., {}, {});
        value_complex_count c;
        construct_one<vector<value_complex>, false>(value_complex(c), {}, {});
        ___water_test(c.count == 0);
        construct_one<vector<value_simple>>({}, {}, {});

        construct_with_template_deduction(value_simple{});
        construct_with_template_deduction(static_cast<int*>(0));
        construct_with_template_deduction(static_cast<size_t>(0));
    }
};

}}}
#endif
