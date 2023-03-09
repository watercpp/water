// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_TESTS_CREATE_HPP
#define WATER_JSON_TESTS_CREATE_HPP
#include <water/json/tests/bits.hpp>
#include <water/cstring.hpp>
namespace water { namespace json { namespace tests {

/*

test creating nodes

*/

inline void create() {
    memory<> m;
    
    auto root = m.create();
    
    root.push_front().null();
    root.push_back().boolean(true);
    root.push_front().boolean(false);
    root.insert(123123).number(123);
    root.insert(root.size()).number(-1.23e-45);
    root.push_back(root.pop_back());
    root.push_front(root.pop_front());
    root.insert(3, root.remove(3));
    
    char const *s = "string";
    root.push_back().string("hello");
    root.push_back().string(cstring("world"));
    root.push_back().string(s, s + 6);
    root.push_back().string(s, 6);
    root.push_back().string(string<char const*>(s, 6));
    
    ___water_test(root[1]);
    
    // add an object
    auto object = root.push_back().object(10);
    ___water_test(object.capacity() == 10);
    object.push_back().name("hello").string("world");
    object.push_back().name(cstring("world")).number(1);
    object.push_back().name(s, s + 6).boolean(true);
    object.push_back().name(s, 6).object(0);
    object.push_back().name(string<char const*>(s, 6)).array(0);
    
    ___water_test(object.nodes().find("hello"));
    ___water_test(object.nodes().find(cstring("world")));
    ___water_test(object.nodes().find(s, s + 6));
    ___water_test(object.nodes().find(s, 6));
    ___water_test(object.nodes().find(string<char const*>(s, 6)));
    
    ___water_test(object["hello"]);
    ___water_test(object[cstring(s)]);
    ___water_test(object[string<char const*>(s, 6)]);
    
    // add an array
    auto array = root.push_back().array(20);
    ___water_test(array.capacity() == 20);
    for(int i = 0; i != static_cast<int>(array.capacity()); ++i)
        array.push_back().number(i - 1);
    
    for(auto a : array)
        a.number(static_cast<int64_t>(a.number()) + 1);
    
    // add depths
    auto depth = root;
    for(unsigned i = 0; i != 64; ++i)
        depth = depth.push_back().array(0);
    
    // add with read
    char const
        data1[] = "{\"name1\":\"value1\", \"name2\":\"value2\"}",
        data2[] = "[1,2,3]";
    root.push_back(read_to_memory(m)(data1, sizeof(data1) - 1).nodes());
    root.push_back(read_to_memory(m)(data2, sizeof(data2) - 1).nodes());

    root.object(0);
    root.array(32); // will reallocate
    
    ___water_test(root.capacity() >= 32);
    
    size_t ws = write_size(root);
    char *w = static_cast<char*>(m.allocate(ws)), *wi = w;
    size_t ws2 = write([&wi](char const* b, char const *e){ while(b != e) *wi++ = *b++; }, root);
    ___water_test(ws == ws2);
    ___water_test(wi == w + ws);
    
    char const expect[] = "[false,null,true,123,-1.23e-45,\"hello\",\"world\",\"string\",\"string\",\"string\",{\"hello\":\"world\",\"world\":1,\"string\":true,\"string\":{},\"string\":[]},[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19],[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]],{\"name1\":\"value1\",\"name2\":\"value2\"},[1,2,3]]";
    ___water_test(string_from(w, w + ws) == string_from(expect, sizeof(expect) - 1));
    
    /*trace() << ministr::size<1024> << string_from(w, ws);
    
    vector<char> x;
    indent([&x](char a){ x.push_back(a); })(w, w + ws);
    trace() << ministr::size<10000> << x;*/
}

}}}
#endif
