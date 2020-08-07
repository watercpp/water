// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MEMORY_TRACK_TESTS_BASIC_HPP
#define WATER_MEMORY_TRACK_TESTS_BASIC_HPP
#include <water/memory_track/tests/bits.hpp>
#include <water/swap.hpp>
namespace water { namespace memory_track { namespace tests {

/*

test most of memory_track::memory

does not test all error detection

uses a fake random number generator. it used to use a real one that is not in water

*/

char16_t constexpr not_random_list[256] = {
    0x66C6, 0x360D, 0xA1B6, 0x8F1B, 0xA854, 0x9CE7, 0x08AE, 0x3323, 0x33F0, 0xE0C1, 0xF66D, 0xC431, 0xD395, 0x0B51, 0x36C1, 0xA804,
    0xE0A9, 0x9EB0, 0xADA8, 0x4ED9, 0xC99E, 0xD49F, 0x06C1, 0x5A40, 0x4D72, 0xD6C9, 0x8820, 0x5D94, 0x7888, 0xA394, 0xD28F, 0xF313,
    0xC10C, 0xBC98, 0xC7AB, 0xB1FA, 0x59F8, 0x0634, 0xD54B, 0x2761, 0x7EE3, 0x1AF3, 0xBCA5, 0xDFE6, 0xB93B, 0xFA50, 0x4CE3, 0xBBB6,
    0x9E91, 0xA353, 0xE45D, 0xAEEF, 0xA4A2, 0x3A42, 0x3D28, 0x9F91, 0xCCF1, 0x5FC4, 0x1392, 0xD60C, 0x5923, 0x12CA, 0xA304, 0x574E,
    0x2D86, 0xE025, 0xD7F7, 0x66F1, 0x8FF5, 0x2A98, 0xC471, 0x74DB, 0xB138, 0x7D90, 0x2808, 0xC285, 0x24CB, 0xE122, 0xEAB7, 0xA97D,
    0x8BD1, 0x4350, 0xA29A, 0x0475, 0x7191, 0x544B, 0xB61B, 0x3D1D, 0x53E5, 0x4C45, 0xED17, 0xE070, 0x7736, 0x6718, 0x8B02, 0x0971,
    0xB302, 0x6A42, 0x5839, 0xDB2C, 0x5898, 0xF9F0, 0x9C9E, 0x6F70, 0xC5F1, 0xEAAD, 0x8BC8, 0xBB4F, 0x663D, 0x21AA, 0xD785, 0xEE18,
    0x87C3, 0xAB80, 0x601C, 0x2FB0, 0x3659, 0x8701, 0x13E9, 0xDB2B, 0xF567, 0xB135, 0x6678, 0x940D, 0xC973, 0x502B, 0xD710, 0x2103,
    0xFB3B, 0x0855, 0xF9F6, 0xF281, 0xFCD0, 0x2AB8, 0xEA2B, 0x7A60, 0xBE2C, 0x31C2, 0x97D1, 0x9CD4, 0xB893, 0xE8BE, 0xCFF6, 0x60F3,
    0x34A0, 0x995B, 0xC7A7, 0xFDA6, 0x2F00, 0x05CE, 0x28B4, 0xC70D, 0x25DD, 0x7A66, 0xACB2, 0x6D81, 0x9444, 0x7C3E, 0xDF86, 0x156C,
    0x823A, 0x35C3, 0xCCBC, 0xCCB7, 0xA060, 0x8C81, 0x07EC, 0x6C9A, 0xBF69, 0x3292, 0xAD27, 0xE011, 0xC9D4, 0x4E13, 0xDF8F, 0xA25E,
    0xEC39, 0xC7D7, 0x5846, 0xADF3, 0x07B9, 0xBCFA, 0x5BB7, 0xEF6A, 0xC29D, 0x6D7F, 0x4D60, 0x7FAD, 0x816B, 0x92CB, 0xD67C, 0xF8C9,
    0xC237, 0x7038, 0xE541, 0xC0B3, 0x400D, 0x01CE, 0xBF5C, 0x8378, 0xBF4C, 0x8F80, 0xAE0A, 0x4B28, 0x898A, 0x9D95, 0x6183, 0x6A12,
    0x8712, 0xDAB4, 0x9B04, 0x59D8, 0xEB7E, 0x7F4C, 0xE84B, 0x0CC6, 0x79F0, 0xF362, 0xEFF6, 0xDA55, 0xFC82, 0x381B, 0x6ACC, 0x1323,
    0x9DFE, 0xDECC, 0xE6E3, 0x0501, 0xA159, 0x5B80, 0x9BB1, 0xAE9A, 0x4950, 0x7AB7, 0xC0CC, 0xF03E, 0xD8DC, 0x5E7E, 0xB05F, 0x12B9,
    0xFAB6, 0xDDF7, 0x19D1, 0x075C, 0xE91B, 0xEE21, 0x370F, 0x67EC, 0xD2D2, 0xA1DE, 0xC94C, 0xF1EE, 0x608F, 0xCC37, 0x0CD8, 0x562E
};

class not_random {
    unsigned mya = 0, myb = 0;
public:
    unsigned operator()() {
        unsigned
            a = not_random_list[mya],
            b = not_random_list[myb];
        mya = (mya + 1) & 0xff;
        if(!mya)
            myb = (myb + 1) & 0xff;
        return a ^ (((b >> 8) & 0xff) | ((b & 0xff) << 8));
    }
};

inline void basic() {
    test_memory memory;
    auto allocator = allocator_for(memory, "tests::basic allocator");
    auto allocator_nothrow = allocator_nothrow_for(memory, "tests::basic allocator_nothrow");
    unsigned constexpr list_size = 256;
    struct {
        void *pointer = 0;
        size_t size = 0;
        bool nothrow = false;
        bool ints = false;
    } list[list_size];
    not_random random;
    
    unsigned at = 0;
    do {
        unsigned masks[] { 0xf, 0xff, 0xfff, 0xffff };
        auto& a = list[at];
        a.size = at == list_size / 2 ?
            0 :
            (random() & masks[random() % 4]);
        a.nothrow = (random() & 1) != 0;
        a.ints = (random() & 1) != 0;
        if(a.nothrow && a.ints)
            a.pointer = allocator_nothrow.allocate<int>(a.size);
        else if(a.nothrow)
            a.pointer = allocator_nothrow.allocate(a.size);
        else if(a.ints)
            a.pointer = allocator.allocate<int>(a.size);
        else
            a.pointer = allocator.allocate(a.size);
        ___water_test(list[at].pointer);
        if(at && !(random() % 4)) {
            // free random
            unsigned free = random() % at;
            auto f = list[free];
            if(f.nothrow && f.ints)
                allocator_nothrow.free<int>(f.pointer, f.size);
            else if(f.nothrow)
                allocator_nothrow.free(f.pointer, f.size);
            else if(f.ints)
                allocator.free<int>(f.pointer, f.size);
            else
                allocator.free(f.pointer, f.size);
            ___water_test(!memory.callback().error());
            swap(list[at], list[free]);
        }
        else
            ++at;
    } while(at != list_size);
    
    size_t cookie_count = 0;
    memory.cookies_to([&cookie_count](test_memory::cookie_iterator b) {
        auto i = b;
        if(i) do ++cookie_count; while(++i != b);
    });
    ___water_test(cookie_count == list_size);
    
    auto corruption = memory.look_for_memory_corruption();
    ___water_test(!corruption.where);
    
    auto corruption_where = static_cast<byte*>(list[list_size / 2].pointer) - 1;
    auto corruption_repair = *corruption_where;
    *corruption_where = 1;
    corruption = memory.look_for_memory_corruption();
    ___water_test(corruption.where == corruption_where);
    *corruption_where = corruption_repair;
    
    auto c = memory.cookie_from_pointer_from_allocate(list[list_size / 3].pointer);
    ___water_test(c);
    
    for(auto &l : list)
        if(l.size > 1) {
            c = memory.cookie_from_pointer(static_cast<byte const*>(l.pointer) + l.size / 2);
            break;
        }
    ___water_test(c);
    
    c = memory.cookie_from_pointer(&list);
    ___water_test(!c);
    
    memory.callback({});
    
    auto statistics_total = memory.statistics_total();
    auto statistics_current = memory.statistics_current();
    ___water_test(statistics_current.success_count == statistics_total.success_count);
    
    size_t count = 0;
    size_t bytes = 0;
    memory.statistics_by_bytes_to([&count, &bytes](statistics_for_bytes const* b, statistics_for_bytes const* e) {
        size_t last = 0;
        while(b != e) {
            ___water_test(!last || last < b->min);
            last = b->max;
            count += b->count_now;
            bytes += b->bytes_now;
            ++b;
        }
    });
    ___water_test(count == statistics_total.count_now);
    ___water_test(bytes == statistics_total.bytes_now);
    
    count = 0;
    bytes = 0;
    size_t names = 0;
    memory.statistics_by_name_to([&count, &bytes, &names](statistics_for_name const* b, statistics_for_name const* e) {
        while(b != e) {
            count += b->count_now;
            bytes += b->bytes_now;
            ++names;
            ++b;
        }
    });
    ___water_test(names == 2);
    ___water_test(count == statistics_total.count_now);
    ___water_test(bytes == statistics_total.bytes_now);
    
    statistics_for_name statistics_name = memory.statistics_for_name(allocator.tag().name());
    ___water_test(statistics_name.count_now);
    
    statistics_for_bytes statistics_bytes = memory.statistics_for_bytes(0);
    ___water_test(statistics_bytes.max && !statistics_bytes.min);
    
    
    // free in random order
    at = list_size;
    while(at) {
        unsigned free = random() % at;
        auto f = list[free];
        if(f.nothrow && f.ints)
            allocator_nothrow.free<int>(f.pointer, f.size);
        else if(f.nothrow)
            allocator_nothrow.free(f.pointer, f.size);
        else if(f.ints)
            allocator.free<int>(f.pointer, f.size);
        else
            allocator.free(f.pointer, f.size);
        ___water_test(!memory.callback().error());
        --at;
        if(free != at)
            swap(list[at], list[free]);
    }
    ___water_test(!memory.statistics_current().count_now);
    
    memory.statistics_reset();
    ___water_test(!memory.statistics_current().success_count);
    count = 0;
    memory.statistics_by_name_to([&count](statistics_for_name const* b, statistics_for_name const* e) {
        count = static_cast<size_t>(e - b);
    });
    ___water_test(!count);
    
    memory.statistics_resize([](size_t size){
        return size ? size * 3 : 33;
    });
    count = 0;
    memory.statistics_by_bytes_to([&count](statistics_for_bytes const* b, statistics_for_bytes const* e) {
        count = static_cast<size_t>(e - b);
    });
    ___water_test(count);
    
    memory.statistics_resize([](size_t size){
        return size ? size * 4 : 44;
    },
        12345
    );
    count = 0;
    memory.statistics_by_bytes_to([&count](statistics_for_bytes const* b, statistics_for_bytes const* e) {
        count = static_cast<size_t>(e - b);
    });
    ___water_test(count);
    
}

}}}
#endif
