// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MEMORY_TRACK_TESTS_ALIGN_HPP
#define WATER_MEMORY_TRACK_TESTS_ALIGN_HPP
#include <water/memory_track/tests/bits.hpp>
namespace water { namespace memory_track { namespace tests {

/*

test memory_track::memory with different alignment

*/

inline void align() {
    test_memory memory;
    
    size_t const align_list[] {
        1,
        2,
        3,
        4,
        7,
        16,
        32,
        63,
        64,
        123,
        256,
        257,
        11111
    };
    
    size_t const size_list[] {
        1,
        16,
        32,
        259,
        1024,
        99999
    };
    
    for(auto align : align_list) {
        for(auto bytes : size_list) {
            void *p = memory.allocate(bytes, align, "hello");
            ___water_test(p != 0);
            ___water_test(!(upointer(p) % align));
            auto *cookie = memory.cookie_from_pointer_from_allocate(p, align);
            ___water_test(cookie);
            ___water_test(cookie->pointer == p);
            ___water_test(cookie->bytes == bytes);
            ___water_test(cookie->align == align);
            auto error = memory.look_for_memory_corruption();
            ___water_test(!error);
            memory.free(p, bytes, align, "hello");
        }
    }
}

}}}
#endif
