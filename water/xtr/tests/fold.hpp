// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XTR_TESTS_FOLD_HPP
#define WATER_XTR_TESTS_FOLD_HPP
#include <water/xtr/tests/bits.hpp>
#include <water/xtr/fold.hpp>
namespace water { namespace xtr { namespace tests {



inline void fold_test() {
    {
        auto s = fold("hello world", ' ' , 123, "! ", base<16>, 1.234e5);
        auto e = cstring("hello world 123! 0x1.E208p16");
        ___water_test(equal(s.begin(), s.end(), e.begin(), e.end()));
    }
    {
        auto s = fold_2014("hello world", ' ' , 123, "! ", base<16>, 1.234e5);
        auto e = cstring("hello world 123! 0x1.E208p16");
        ___water_test(equal(s.begin(), s.end(), e.begin(), e.end()));
    }
    {
        auto s = fold();
        ___water_test(!s.size());
    }
    
}

}}}
#endif
