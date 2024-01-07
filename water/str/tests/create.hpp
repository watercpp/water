// Copyright 2024 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_TESTS_CREATE_HPP
#define WATER_STR_TESTS_CREATE_HPP
#include <water/str/tests/bits.hpp>
#include <water/later.hpp>
namespace water { namespace str { namespace tests {

/*

Test str::create

Not thread safe, uses a global pointer

*/

namespace create_tests {

    struct to {
        unsigned construct = 0;
        char array[1024];
        unsigned size = 0;
    };

    template<size_t size_>
    bool operator==(to const& a, char const (&b)[size_]) {
        return equal(a.array, a.size, b, size_ - 1);
    }

    namespace {
        to *pointer = 0; // <----------------------------------------------- the global pointer
    }

    class write
    {
        bool mydestroyed = false;
        to *myto;

    public:

        write() :
            myto{pointer}
        {
            ___water_test(pointer);
            ++myto->construct;
        }

        ~write() {
            mydestroyed = true;
        }

        write(write const&) = delete;
        write& operator=(write const&) = delete;
    
        template<typename iterator_>
        void operator()(iterator_ begin, iterator_ end) {
            ___water_test(!mydestroyed);
            while(begin != end && myto->size != array_size(myto->array)) {
                myto->array[myto->size++] = static_cast<char>(*begin);
                ++begin;
            }
        }
    };

    constexpr str::create<out<write>>
        create,
        create_copy1 = create,
        create_copy2{create};

    inline void tests() {
        create_tests::to to;
        ___water_test(pointer == 0);
        pointer = &to;
        auto pointer0 = later([]{ pointer = 0; });

        to = {};
        create << "hello" << " world";
        auto create2 = create;
        auto create3{create};
        create2 = create3;
        ___water_test(to.construct == 1);
        ___water_test(to == "hello world");

        // test first argument as something complex
        // visual c++ 2022 had problems with the return type of the << operator for water::xtr

        to = {};
        create << begin_end_from("hello", 5) << '!';
        ___water_test(to.construct == 1);
        ___water_test(to == "hello!");

        to = {};
        create << restore_settings << hex << 10;
        ___water_test(to.construct == 1);
        ___water_test(to == "0xA");

        to = {};
        create << true;
        ___water_test(to.construct == 1);
        ___water_test(to == "1");

        to = {};
        create << static_cast<int*>(0);
        ___water_test(to.construct == 1);
        ___water_test(to == "0x0");

        to = {};
        create << number(1, str::settings().base(2));
        ___water_test(to.construct == 1);
        ___water_test(to == "0b1");


    }

}


inline void create_all() {
    create_tests::tests();
}


}}}
#endif
