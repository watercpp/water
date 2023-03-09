// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XTR_TESTS_TO_FUNCTION_HPP
#define WATER_XTR_TESTS_TO_FUNCTION_HPP
#include <water/xtr/tests/bits.hpp>
namespace water { namespace xtr { namespace tests {

/*

This is not thread safe! Do not run this test from multiple threads at once.

Tests to_buffered and to_unbuffered

*/

struct to_result
{
    char array[256];
    unsigned size = 0;
    bool called_begin_end = false;
    bool called_cstring = false;
    bool called_char = false;
    bool called_insert = false;
    
    void operator()(char const* b, char const* e) {
        called_begin_end = true;
        ___water_test(b && e && b <= e);
        while(b != e && size != sizeof(array))
            array[size++] = *b++;
        ___water_test(b == e);
    }
    
    void operator()(char const* b) {
        called_cstring = true;
        ___water_test(b);
        if(!b)
            return;
        while(*b)
            array[size++] = *b++;
        ___water_test(size != sizeof(array));
    }
    
    void operator()(char a) {
        called_char = true;
        ___water_test(size != sizeof(array));
        if(size != sizeof(array))
            array[size++] = a;
    }
    
    struct iterator {};
    
    iterator end() {
        return {};
    }
    
    void insert(iterator, char const* b, char const* e) {
        called_insert = true;
        while(b != e && size != sizeof(array))
            array[size++] = *b++;
    }
};

template<size_t size_>
bool equal(to_result& a, char const (&b)[size_]) {
    return equal(a.array, a.array + a.size, b, b + size_ - 1);
}

template<typename = void>
struct to_result_global {
    static to_result* pointer; // <----------------------- this global pointer is set and used without locks
};
template<typename a_>
to_result* to_result_global<a_>::pointer = 0;

inline void to_plain_function_begin_end(char const* b, char const* e) {
    (*to_result_global<>::pointer)(b, e);
}

inline void to_plain_function_cstring(char const* a) {
    (*to_result_global<>::pointer)(a);
}

inline void to_plain_function_char(char a) {
    (*to_result_global<>::pointer)(a);
}

struct to_struct_all {
    to_result *to;
    template<typename iterator_> void operator()(iterator_ b, iterator_ e) { (*to)(b, e); }
    template<typename iterator_> void operator()(iterator_ a) { (*to)(a); } // it should always pick the begin,end version, never this
    void operator()(char a) { (*to)(a); }
};

struct to_struct_all_const {
    to_result *to;
    template<typename iterator_> void operator()(iterator_ b, iterator_ e) const { (*to)(b, e); }
    template<typename iterator_> void operator()(iterator_ a) const { (*to)(a); }
    void operator()(char a) const { (*to)(a); }
};

struct to_struct_cstring {
    to_result *to;
    template<typename iterator_> void operator()(iterator_ a) { (*to)(a); }
};

struct to_struct_cstring_const {
    to_result *to;
    template<typename iterator_> void operator()(iterator_ a) const { (*to)(a); }
};



inline void to_function_test() {
        
    { // buffered
        to_result r;
        to_buffered<to_struct_all> t{&r};
        t << "hello" << 1;
        to_function(to_struct_all{&r}) << '!';
        auto x = to_struct_all{&r};
        to_function(x) << '!';
        to_function(static_cast<decltype(x) const&>(x)) << '!';
        ___water_test(equal(r, "hello1!!!"));
        ___water_test(r.called_begin_end);
    }
    
    { // rvalue reference operator
        to_result r;
        to_buffered<to_struct_all>{&r} << "world" << 7;
        ___water_test(equal(r, "world7"));
        ___water_test(r.called_begin_end);
    }
    
    { // const reference operator
        to_result r;
        to_buffered<to_struct_all_const> t{&r};
        static_cast<to_buffered<to_struct_all_const> const&>(t) << 123 << '!';
        ___water_test(equal(r, "123!"));
        ___water_test(r.called_begin_end);
    }
    
    { // cstring only, refernece
        to_result r;
        to_buffered<to_struct_cstring> b3{&r};
        b3 << "hello" << 123;
        to_function(to_struct_cstring{&r}) << '!';
        auto x = to_struct_cstring{&r};
        to_function(x) << '!';
        to_function(static_cast<decltype(x) const&>(x)) << '!';
        ___water_test(equal(r, "hello123!!!"));
        ___water_test(r.called_cstring);
    }
    
    { // cstring only, const reference operator
        to_result r;
        to_buffered<to_struct_cstring_const> b4{&r};
        static_cast<to_buffered<to_struct_cstring_const> const&>(b4) << 1.23 << 'x';
        ___water_test(equal(r, "1.23x"));
        ___water_test(r.called_cstring);
    }
    
    { // function pointer begin end
        to_result r;
        to_result_global<>::pointer = &r;
        using type_ = to_buffered<decltype(&to_plain_function_begin_end)>;
        type_ t{&to_plain_function_begin_end};
        t << "hell" << 0;
        static_cast<type_&&>(t) << ' ' << "wor";
        static_cast<type_ const&>(t) << 1 << 'd';
        to_function(to_plain_function_begin_end) << '!';
        to_function(&to_plain_function_begin_end) << '!';
        ___water_test(equal(r, "hell0 wor1d!!"));
        ___water_test(r.called_begin_end);
    }
    
    { // function pointer cstring
        to_result r;
        to_result_global<>::pointer = &r;
        using type_ = to_buffered<decltype(&to_plain_function_cstring)>;
        type_ t{&to_plain_function_cstring};
        t << "hell" << 0;
        static_cast<type_&&>(t) << ' ' << "wor";
        static_cast<type_ const&>(t) << 1 << 'd';
        to_function(to_plain_function_cstring) << '!';
        to_function(&to_plain_function_cstring) << '!';
        ___water_test(equal(r, "hell0 wor1d!!"));
        ___water_test(r.called_cstring);
    }
    
    { // unbuffered
        to_result r;
        to_unbuffered<to_struct_all> t{&r};
        t << "hello" << 1;
        to_function_unbuffered(to_struct_all{&r}) << '!';
        auto x = to_struct_all{&r};
        to_function_unbuffered(x) << '!';
        to_function_unbuffered(static_cast<decltype(x) const&>(x)) << '!';
        ___water_test(equal(r, "hello1!!!"));
        ___water_test(r.called_char);
    }
    
    { // unbuffered, rvalue reference operator
        to_result r;
        to_unbuffered<to_struct_all>{&r} << "world" << 7;
        ___water_test(equal(r, "world7"));
        ___water_test(r.called_char);
    }
    
    { // unbuffered, const reference operator
        to_result r;
        to_unbuffered<to_struct_all_const> t{&r};
        static_cast<to_unbuffered<to_struct_all_const> const&>(t) << 123 << '!';
        ___water_test(equal(r, "123!"));
        ___water_test(r.called_char);
    }
    
    { // function pointer unbuffered
        to_result r;
        to_result_global<>::pointer = &r;
        using type_ = to_unbuffered<decltype(&to_plain_function_char)>;
        type_ t{&to_plain_function_char};
        t << "hell" << 0;
        static_cast<type_&&>(t) << ' ' << "wor";
        static_cast<type_ const&>(t) << 1 << 'd';
        to_function_unbuffered(to_plain_function_char) << '!';
        to_function_unbuffered(&to_plain_function_char) << '!';
        ___water_test(equal(r, "hell0 wor1d!!"));
        ___water_test(r.called_char);
    }
    
    { // lambda begin end
        to_result r;
        to_function([&r](char const* b, char const* e) { r(b, e); }) << "hello " << 123;
        ___water_test(equal(r, "hello 123"));
        ___water_test(r.called_begin_end);
    }
    
    { // lambda cstring
        to_result r;
        to_function([&r](char const* b) { r(b); }) << "hello " << 123;
        ___water_test(equal(r, "hello 123"));
        ___water_test(r.called_cstring);
    }
    
    { // lambda unbuffered
        to_result r;
        to_function_unbuffered([&r](char a) { r(a); }) << "hello " << 123;
        ___water_test(equal(r, "hello 123"));
        ___water_test(r.called_char);
    }
    
    { // insert_at_end
        to_result r;
        insert_at_end(r) << "hello " << 123;
        ___water_test(equal(r, "hello 123"));
        ___water_test(r.called_insert);
    }
}

}}}
#endif
