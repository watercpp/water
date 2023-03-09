// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_TESTS_UNICODE_HPP
#define WATER_XML_TESTS_UNICODE_HPP
#include <water/xml/tests/bits.hpp>
#include <water/str/str.hpp>
#include <water/xtr/base.hpp>
#include <water/vector.hpp>
namespace water { namespace xml { namespace tests {

/*

Generate XML with all unicode codepoints like this:

<unicode>
    <plane number="0">
        <codepoints first="0x20" last="0x7E">....</codepoints>
        <codepoints first="0xA0" last="0xFF">....</codepoints>
        <codepoints first="0x100" last="0x1FF">....</codepoints>
        ...
    </plane>
    <plane number="1">....</plane>
        <codepoints first="0x10000" last="0x100FF">...
    ....
</unicode>

Then write and parse it as UTF-8/16/32 and compare the results.

*/

template<typename memory_>
node<char16_t, memory_> unicode_generate(memory_& m) {
    auto root = create<char16_t>(m).name("unicode");
    
    auto plane = root.push_back().name("plane");
    plane.attributes(root.create().name("number").value("0"));

    unsigned char const ascii[] {
        'h', 'e', 'l', 'l', 'o', // will keep whitespace below
        0x9, 0xa, 0xd, // tab lf cr
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e
    };
    auto n = plane.push_back().name("codepoints").first_value(ascii);
    n.attributes_or_add("first").value("0x9");
    n.attributes_or_add("last").value("0x7E");
    
    unsigned char byte1[0x60]; // 0xa0 - 0xff (0x80-0x9f is control characters)
    unsigned char b = 0;
    do byte1[b] = 0xa0 + b; while(++b != 0x60);
    n = plane.create().name("codepoints").first_value(byte1).insert_after(n);
    n.attributes(root.create().name("first").value("0xA0").insert_before(root.create().name("last").value("0xFF")));
    
    char32_t hi = 0;
    do {
        char16_t
            chars[0x200],
            *c = chars,
            u = hi ? 0 : 0x100;
        if(hi) {
            plane = root.push_back().name("plane");
            plane.attributes(plane.create().name("number").value(xtr::no << static_cast<unsigned long>(hi >> 16) << xtr::string));
        }
        do {
            if(!hi) {
                if(u == 0xd800)
                    u = 0xe000;
                *c++ = u;
            }
            else {
                char32_t x = (hi | u) - 0x10000;
                *c++ = static_cast<char16_t>(0xd800 | ((x >> 10) & 0x3ff));
                *c++ = static_cast<char16_t>(0xdc00 | (x & 0x3ff));
            }
            if((u & 0xff) == 0xff || u == 0xfffd || (!hi && u == 0xfdcf)) { // fdd0-fdef are non-characters
                n = plane.push_back().name("codepoints").first_value(chars, c);
                n.attributes_or_add("first").value(xtr::no << xtr::base<16> << static_cast<unsigned long>((hi | u) & 0xffff00) << xtr::string);
                n.attributes_or_add("last").value(xtr::no << xtr::base<16> << static_cast<unsigned long>(hi | u) << xtr::string);
                c = chars;
                if(!hi && u == 0xfdcf)
                    u = 0xfdef;
            }
        } while(++u != 0xfffe);
        u = 0;
        hi += 0x10000;
    } while(hi != 0x110000);
    
    return root;
}



class unicode_test
{
    memory<> mymemory;

public:
    void operator()() {
        memory<> original_memory;
        auto original = unicode_generate(original_memory); // utf16
        
        vector<unsigned char> vectors[12];
        vector<char> vectors8[2];
        vector<char16_t> vectors16[2];
        vector<char32_t> vectors32[2];
        
        write(vectors[0], original, true);
        write(vectors[1], original, false);
        unsigned v = 0;
        do {
            mymemory.clear();
            
            auto a8 = read_to_memory<char>(mymemory).parse_in_place(copy(vectors[v]), vectors[v].size()).nodes();
            auto b8 = read_to_memory<char>(mymemory).parse_in_place(copy(vectors[v + 1]), vectors[v + 1].size()).nodes();
            ___water_test(a8);
            ___water_test(b8);
            ___water_test(equal(a8, original));
            ___water_test(equal(b8, original));
            ___water_test(equal(a8, b8));
            if(v == 0) {
                write(vectors[2], b8, true);
                write(vectors[3], a8, false);
                write(vectors8[0], a8, true);
                write(vectors8[1], b8, true);
            }
            
            auto a16 = read_to_memory<char16_t>(mymemory).parse_in_place(copy(vectors[v]), vectors[v].size()).nodes();
            auto b16 = read_to_memory<char16_t>(mymemory).parse_in_place(copy(vectors[v + 1]), vectors[v + 1].size()).nodes();
            ___water_test(a16);
            ___water_test(b16);
            ___water_test(equal(a16, original));
            ___water_test(equal(b16, original));
            ___water_test(equal(a16, b16));
            if(v == 2) {
                write(vectors[4], b16, true);
                write(vectors[5], a16, false);
                vectors[6] = vectors[4];
                vectors[7] = vectors[5];
                swap_endian_16(vectors[6]);
                swap_endian_16(vectors[7]);
                write(vectors16[0], a16, true);
                write(vectors16[1], b16, true);
            }
            
            auto a32 = read_to_memory<char32_t>(mymemory).parse_in_place(copy(vectors[v]), vectors[v].size()).nodes();
            auto b32 = read_to_memory<char32_t>(mymemory).parse_in_place(copy(vectors[v + 1]), vectors[v + 1].size()).nodes();
            ___water_test(a32);
            ___water_test(b32);
            ___water_test(equal(a32, original));
            ___water_test(equal(b32, original));
            ___water_test(equal(a32, b32));
            if(v == 6) {
                write(vectors[8], b32, true);
                write(vectors[9], a32, false);
                vectors[10] = vectors[8];
                vectors[11] = vectors[9];
                swap_endian_32(vectors[10]);
                swap_endian_32(vectors[11]);
                write(vectors32[0], a32, true);
                write(vectors32[1], b32, true);
            }
            
            ___water_test(equal(a8, a16));
            ___water_test(equal(a8, a32));
            ___water_test(equal(a16, a32));
            
            ___water_test(equal(b8, b16));
            ___water_test(equal(b8, b32));
            ___water_test(equal(b16, b32));
            
        } while((v += 2) != 12);
            
        parse2<char>(vectors8, original);
        parse2<char16_t>(vectors8, original);
        parse2<char32_t>(vectors8, original);
        
        parse2<char>(vectors16, original);
        parse2<char16_t>(vectors16, original);
        parse2<char32_t>(vectors16, original);
        
        parse2<char>(vectors16, original);
        parse2<char16_t>(vectors16, original);
        parse2<char32_t>(vectors16, original);
        
    }

private:
    template<typename to_, typename from_>
    void parse2(vector<from_>* from, node<char16_t> original) {
        mymemory.clear();
        auto f0 = unicode::utf_range_from<unicode::utf_from_char<to_>::result>(from[0].begin(), from[0].end());
        auto r0 = read_to_memory<to_>(mymemory)(f0.begin(), f0.end()).nodes();
        auto f1 = unicode::utf_range_from<unicode::utf_from_char<to_>::result>(from[1].begin(), from[1].end());
        auto r1 = read_to_memory<to_>(mymemory)(f1.begin(), f1.end()).nodes();
        ___water_test(r0);
        ___water_test(r1);
        ___water_test(equal(r0, original));
        ___water_test(equal(r1, original));
        ___water_test(equal(r0, r1));
    }

    void* copy(vector<unsigned char> const& a) {
        if(a.size()) {
            unsigned char *r = static_cast<unsigned char*>(mymemory.allocate(a.size()));
            if(r) {
                auto *t = r;
                auto *f = a.begin();
                do *t++ = *f++; while(f != a.end());
                return r;
            }
        }
        return 0;
    }

    void swap_endian_16(vector<unsigned char>& a) {
        if(a.empty() || a.size() % 2)
            return;
        auto
            i = a.begin(),
            e = a.end();
        while(i != e) {
            auto s = i[0];
            i[0] = i[1];
            i[1] = s;
            i += 2;
        }
    }

    void swap_endian_32(vector<unsigned char>& a) {
        if(a.empty() || a.size() % 4)
            return;
        auto
            i = a.begin(),
            e = a.end();
        while(i != e) {
            auto s = i[0];
            i[0] = i[3];
            i[3] = s;
            s = i[1];
            i[1] = i[2];
            i[2] = s;
            i += 4;
        }
    }

    template<typename char_>
    void write(vector<unsigned char>& to, node<char_, memory<>> from, bool indent) {
        size_t size = write_size(from, indent ? true : false, indent ? '\t' : 0);
        ___water_test(size);
        to.clear();
        xml::write(
            [&to](char_ const* b, char_ const* e) {
                auto x = static_cast<unsigned char const*>(static_cast<void const*>(b));
                to.insert(to.end(), x, x + static_cast<size_t>(e - b) * sizeof(*b));
            },
            from,
            indent ? true : false,
            indent ? '\t' : 0
        );
        ___water_test(size * sizeof(char_) == to.size());
    }

    template<typename char_>
    void write(vector<char_>& to, node<char_, memory<>> from, bool indent) {
        size_t size = write_size(from, indent ? true : false, indent ? '\t' : 0);
        ___water_test(size);
        to.clear();
        to.reserve(size);
        struct to_ {
            vector<char_> *to;
            void operator()(char_ a) {
                to->push_back(a);
            }
            void operator()(char_ const* b, char_ const* e) {
                to->insert(to->end(), b, e);
            }
        };
        write_unbuffered(to_{&to}, from, indent ? true : false, indent ? '\t' : 0);
        ___water_test(size == to.size());
    }
};

}}}
#endif
