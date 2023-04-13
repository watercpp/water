// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_UNICODE_BYTERATOR_HPP
#define WATER_UNICODE_BYTERATOR_HPP
#include <water/unicode/bits.hpp>
namespace water { namespace unicode {

// byte unpacking iterator, to read 16-bit or 24-bit or 32-bit from raw bytes. it expects 1 byte to have 8 bits

template<unsigned bits_>
class byterator
{
    static_assert(8 < bits_ && bits_ <= 32, "bits_ has to be between 9 and 32 bits");

public:
    using iterator_category = random_access_iterator_tag;
    using value_type = ifel<bits_ <= 16, char16_t, char32_t>;
    using reference = value_type;
    using pointer = value_type const*;
    using difference_type = decltype(static_cast<uchar_t*>(0) - static_cast<uchar_t*>(0));

private:
    uchar_t const *my = 0;
    uchar_t myorder[4] = {0,1,2,3}; // myorder[0] is most significant byte
    static constexpr difference_type bytes_ = static_cast<difference_type>(bits_ / 8 + (bits_ % 8 ? 1 : 0));
    template<difference_type> struct unpack_select;

public:
    byterator() = default;

    byterator(void const* pointer, bool big_endian) :
        my(static_cast<uchar_t const*>(pointer))
    {
        if(!big_endian) {
            myorder[0] = bytes_ - 1;
            myorder[1] = bytes_ >= 2 ? bytes_ - 2 : 0;
            myorder[2] = bytes_ >= 3 ? bytes_ - 3 : 0;
            myorder[3] = bytes_ >= 4 ? bytes_ - 4 : 0;
        }
    }

    byterator(
        void const* pointer,
        unsigned byte0,
        unsigned byte1,
        unsigned byte2 = 0,
        unsigned byte3 = 0
    ) :
        my(static_cast<uchar_t const*>(pointer)),
        myorder{
            static_cast<uchar_t>(byte0),
            static_cast<uchar_t>(byte1),
            static_cast<uchar_t>(byte2),
            static_cast<uchar_t>(byte3)
        }
    {
        // byte0 is the most significant byte, so big_endian 32-bit would have byte0 = 0, byte1 = 1, byte2 = 2, byte3 = 3
    }

    reference operator*() const {
        return unpack(my, static_cast<unpack_select<bytes_>*>(0));
    }

    reference operator[](difference_type d) const {
        return unpack(my + d / bytes_, static_cast<unpack_select<bytes_>*>(0));
    }

    byterator& operator++() {
        my += bytes_;
        return *this;
    }

    byterator operator++(int) {
        byterator r(*this);
        my += bytes_;
        return r;
    }

    byterator& operator--() {
        my -= bytes_;
        return *this;
    }

    byterator operator--(int) {
        byterator r(*this);
        my -= bytes_;
        return r;
    }

    byterator& operator+=(difference_type d) {
        my += d * bytes_;
        return *this;
    }

    byterator& operator-=(difference_type d) {
        my -= d * bytes_;
        return *this;
    }

    byterator operator+(difference_type d) const {
        return byterator(*this) += d;
    }

    friend byterator operator+(difference_type d, byterator b) {
        return b += d;
    }

    byterator operator-(difference_type d) const {
        return byterator(*this) -= d;
    }

    difference_type operator-(byterator const& b) const {
        return (my - b.my) / bytes_;
    }

    bool operator==(byterator const& b) const {
        return my == b.my;
    }

    bool operator!=(byterator const& b) const {
        return my != b.my;
    }

    bool operator<(byterator const& b) const {
        return my < b.my;
    }

    bool operator>(byterator const& b) const {
        return my > b.my;
    }

    bool operator<=(byterator const& b) const {
        return my <= b.my;
    }

    bool operator>=(byterator const& b) const {
        return my >= b.my;
    }

private:
    value_type unpack(uchar_t const* a, unpack_select<2>*) const {
        return
            (static_cast<value_type>(a[myorder[0]]) << 8) |
            a[myorder[1]];
    }

    value_type unpack(uchar_t const* a, unpack_select<3>*) const {
        return
            (static_cast<value_type>(a[myorder[0]]) << 8) |
            (static_cast<value_type>(a[myorder[1]]) << 8) |
            a[myorder[2]];
    }

    value_type unpack(uchar_t const* a, unpack_select<4>*) const {
        return
            (static_cast<value_type>(a[myorder[0]]) << 24) |
            (static_cast<value_type>(a[myorder[1]]) << 16) |
            (static_cast<value_type>(a[myorder[2]]) << 8) |
            a[myorder[3]];
    }
};

}}
#endif
