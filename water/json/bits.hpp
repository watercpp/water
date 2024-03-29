// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_BITS_HPP
#define WATER_JSON_BITS_HPP
#include <water/water.hpp>
#include <water/types.hpp>
#include <water/is_no_to.hpp>
#include <water/int.hpp>
#include <water/unicode/utf_iterators.hpp>
#include <water/unicode/utf_length.hpp>
#include <water/unicode/utf.hpp>
#include <water/unicode/byterator.hpp>
#include <water/swap.hpp>
#include <water/char8.hpp>
#include <water/size_from.hpp>
namespace water { namespace json {

using uchar_t = ifel<is_unsigned<char>, char, unsigned char>;
using int64_t = int_bits_at_least<64>;
using int32_t = int_bits_at_least<32>;
using uint16_t = uint_bits_at_least<16>;
using uint32_t = uint_bits_at_least<32>;

enum class type : char {
    null = 0,
    array = '[',
    object = '{',
    string = 's',
    number = 'n',
    boolean = 'b'
};

// memory_node is the data structure
// this strange layout should save some space
// 32bit: 32 bytes
// 64bit: 40 bytes
struct memory_node {
    memory_node *in;
    uchar_t *name;
    union {
        memory_node **nodes; // array or object
        uchar_t *string;
        int64_t integer;
        bool boolean;
    };
    struct at_capacity { // do not declare struct inside anonymous union
        uint32_t at; // position in in->nodes
        uint32_t capacity; // of this->nodes, if this is not 0 the node is non-empty object/array
    };
    union {
        at_capacity me;
        memory_node *previous; // used only during read before the nodes array is allocated
    };
    union {
        uint32_t size; // string or array or object
        int32_t exponent;
    };
    uint16_t name_size;
    json::type type;
    uchar_t extra; // used for number, imprecisce
};



namespace _ {

    template<typename unsigned_, bool small_ = (static_cast<unsigned_>(-1) < static_cast<size_t>(-1))>
    struct string_size_clip_if {
        template<typename iterator_>
        static bool do_it(unsigned_& to, size_t size, iterator_ begin) {
            if(size <= static_cast<unsigned_>(-1)) {
                to = static_cast<unsigned_>(size);
                return false;
            }
            to = static_cast<unsigned_>(-1);
            auto e = begin + to;
            unsigned x = 0;
            while(++x != 8) {
                --to;
                auto n = unicode::utf8_first_of(*--e);
                if(n && n <= x) {
                    to += n;
                    break;
                }
            }
            ___water_assert(!x || unicode::utf8_first_of(*(begin + to)));
            return true;
        }
    };
    
    template<typename unsigned_>
    struct string_size_clip_if<unsigned_, false> {
        template<typename iterator_>
        static bool do_it(unsigned_& to, size_t size, iterator_) {
            to = size;
            return false;
        }
    };

}

template<typename unsigned_, typename iterator_>
bool string_size_clip(unsigned_& to, iterator_ begin, iterator_ end) {
    // clip string to whole utf8 sequences if its size does not fit unsigned_, return true if it did clip
    return _::string_size_clip_if<unsigned_>::do_it(to, static_cast<size_t>(end - begin), begin);
}

template<typename char_>
inline char32_t string_compare_cast(char_ a) { return static_cast<char32_t>(a); }
inline char16_t      string_compare_cast(char16_t a)      { return a; }
inline unsigned char string_compare_cast(unsigned char a) { return a; }
inline unsigned char string_compare_cast(signed char a)   { return static_cast<unsigned char>(a); }
inline unsigned char string_compare_cast(char a)          { return static_cast<unsigned char>(a); }
inline char8_or_not  string_compare_cast(char8_or_not a)  { return a; }

}}
#endif
