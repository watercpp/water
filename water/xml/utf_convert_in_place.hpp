// Copyright 2018-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_UTF_CONVERT_IN_PLACE_HPP
#define WATER_XML_UTF_CONVERT_IN_PLACE_HPP
#include <water/xml/encoding.hpp>
#include <water/unicode/utf_length.hpp>
#include <water/unicode/byterator.hpp>
namespace water { namespace xml {

/*

Used by read::parse_in_place to convert raw memory to the char type and UTF encoding it needs

Converts memory in place if possible, otherwise uses allocate_with_undo

This expects each byte of the input to contain 8 bits.

*/

template<typename char_, typename memory_>
class utf_convert_in_place
{
    static_assert(unicode::utf_from_char<char_>, "use a built in type that works with water::unicode");
    static_assert(unicode::utf_from_char<char_> != 8 || sizeof(char_) == 1, "");
    
    static unsigned constexpr utf = unicode::utf_from_char<char_>;
    
    template<unsigned>
    struct select {};

private:
    memory_ *mymemory;
    void *mypointer;
    encoding myencoding;
    char_
        *mybegin = 0,
        *myend = 0;

public:
    utf_convert_in_place(memory_ *memory, void *pointer, size_t bytes) :
        mymemory{memory},
        myencoding{pointer, bytes}
    {
        mypointer = static_cast<char*>(pointer) + myencoding.byte_order_mark();
        bytes -= myencoding.byte_order_mark();
        if(myencoding.utf8()) {
            unicode::utf_length<8> length{static_cast<unsigned char*>(mypointer), bytes, [](char32_t a) { return a != 0; }};
            if(length)
                convert(length, static_cast<unsigned char*>(mypointer), select<utf == 8 ? 1 : 0>{});
        }
        else if(myencoding.utf16() && !(bytes % 2)) {
            auto from = unicode::byterator<16>(mypointer, myencoding.big_endian());
            unicode::utf_length<16> length{from, bytes / 2, [](char32_t a) { return a != 0; }};
            if(length)
                convert(
                    length,
                    from,
                    select<utf == 16 && sizeof(char_) == 2 && static_cast<char_>(-1) == 0xffffu ? 1 : 0>{}
                );
        }
        else if(myencoding.utf32() && !(bytes % 4)) {
            auto from = unicode::byterator<32>(mypointer, myencoding.big_endian());
            unicode::utf_length<32> length{from, bytes / 4, [](char32_t a) { return a != 0; }};
            if(length)
                convert(
                    length,
                    from,
                    select<
                        utf == 32 && sizeof(char_) == 4 && static_cast<char_>(-1) == 0xfffffffful ? 1 :
                        sizeof(char_) <= utf / 8 ? 2 : // can convert in place unless char_ is odd
                        0
                    >{}
                );
        }
    }

    utf_convert_in_place(utf_convert_in_place const&) = delete;
    utf_convert_in_place& operator=(utf_convert_in_place const&) = delete;

    char_* begin() {
        return mybegin;
    }

    char_* end() {
        return myend;
    }

private:
    template<unsigned from_utf_, typename iterator_>
    void convert(unicode::utf_length<from_utf_> length, iterator_ from, select<0>) {
        // copy
        size_t
            to_size = utf == 8 ? length.utf8() : utf == 16 ? length.utf16() : length.utf32(),
            from_size = from_utf_ == 8 ? length.utf8() : from_utf_ == 16 ? length.utf16() : length.utf32();
        mybegin = static_cast<char_*>(mymemory->allocate_with_undo(to_size * sizeof(char_), alignof(char_)));
        if(mybegin)
            myend = unicode::utf_from_utf<utf, from_utf_>(mybegin, from, from_size).end();
    }

    void convert(unicode::utf_length<8> length, unsigned char*, select<1>) {
        // from and to utf8
        mybegin = static_cast<char_*>(mypointer); // char_ has size 1
        myend = mybegin + length.utf8();
    }

    void convert(unicode::utf_length<16> length, unicode::byterator<16> from, select<1>) {
        // from utf16, in place
        mybegin = static_cast<char_*>(mypointer);
        myend = mybegin + length.utf16();
        endian_change_maybe(from);
    }

    void convert(unicode::utf_length<32> length, unicode::byterator<32> from, select<1>) {
        // from ut32, in place
        mybegin = static_cast<char_*>(mypointer);
        myend = mybegin + length.utf32();
        endian_change_maybe(from);
    }

    void convert(unicode::utf_length<32> length, unicode::byterator<32> from, select<2>) {
        // from utf32, convert in place, the output is always <= 4 bytes of each input character
        mybegin = static_cast<char_*>(mypointer);
        myend = unicode::utf_from_utf<utf, 32>(mybegin, from, length.utf32()).end();
    }

    template<typename iterator_>
    void endian_change_maybe(iterator_ from) {
        char_ const one = 1;
        bool big_endian = *static_cast<unsigned char const*>(static_cast<void const*>(&one)) == 0;
        if(big_endian == myencoding.big_endian())
            return;
        auto *to = mybegin;
        while(to != myend) {
            *to = static_cast<char_>(*from);
            ++to;
            ++from;
        };
    }
};

}}
#endif
