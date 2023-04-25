// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_ALIGN_WITH_UNALIGN_HPP
#define WATER_ALIGN_WITH_UNALIGN_HPP
#include <water/int.hpp>
#include <water/types.hpp>
namespace water {

/*

A way to make allocations aligned to any alignment:

void* allocate_aligned(size_t bytes, size_t align) {
    void *pointer = operator new(bytes + align);
    return align_with_unalign(pointer, align);
}

void free_aligned(void *pointer, size_t bytes, size_t align) {
    pointer = unalign(pointer);
    operator delete(pointer, bytes + align);
}

align must never be 0, and align extra bytes must be allocated from the underlying allocator.

*/



inline void* align_with_unalign(void* pointer, size_t align) noexcept {
    // return pointer aligned to align
    // poiner can be 0, but align must *not* be 0
    
    // this keeps how much pointer was moved in the bytes below the returned pointer. 
    // if m is number of bytes moved and r is the returned pointer:
    // - when m is 1
    //   - r[-m] = 0
    // - when m is less than byte-max
    //   - r[-m] = 0
    //   - r[-1] = m - 1
    // - when m is (255 + 255 + 8), and byte-max is 255
    //   - r[-m] = 0
    //   - r[-(255 + 8)] = 255
    //   - r[-8] = 255
    //   - r[-1] = 7
    using byte = unsigned char;
    byte constexpr byte_max = static_cast<byte>(-1);
    byte *a = static_cast<byte*>(pointer);
    if(a) {
        *a = 0;
        align -= reinterpret_cast<if_not_void<uint_size_at_least<sizeof(void*)>, size_t>>(a) % align;
        a += align;
        if(--align) {
            byte *i = a - 1;
            while(align > byte_max) {
                *i = byte_max;
                i -= byte_max;
                align -= byte_max;
            }
            *i = static_cast<byte>(align);
        }
    }
    return a;
}



namespace _ {

    template<typename byte_>
    byte_* unalign_do(byte_* p) noexcept {
        if(p) {
            --p;
            while(*p)
                p -= *p;
        }
        return p;
    }
    
    template<typename byte_>
    byte_* unalign_do(byte_* p, decltype(sizeof(0)) align) noexcept {
        if(p) {
            if(reinterpret_cast<if_not_void<uint_size_at_least<sizeof(void*)>, size_t>>(p) % align)
                return 0;
            size_t sum = 0;
            --p;
            ++sum;
            while(*p) {
                sum += *p;
                if(sum > align)
                    return 0;
                p -= *p;
            }
        }
        return p;
    }
    
}


inline void* unalign(void* pointer) noexcept {
    // return original pointer after align_with_unalign
    return _::unalign_do(static_cast<unsigned char*>(pointer));
}

inline void const* unalign(void const* pointer) noexcept {
    // return original pointer after align_with_unalign
    return _::unalign_do(static_cast<unsigned char const*>(pointer));
}


inline void* unalign(void* pointer, size_t align) {
    // return original pointer after align_with_unalign, or 0 if it detects an error.
    // this could be useful for debugging.
    // align must be the same as for with align_with_unalign, and is used to verify the result.
    return _::unalign_do(static_cast<unsigned char*>(pointer), align);
}

inline void const* unalign(void const* pointer, size_t align) {
    return _::unalign_do(static_cast<unsigned char const*>(pointer), align);
}



}
#endif
