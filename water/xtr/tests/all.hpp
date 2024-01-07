// Copyright 2017-2024 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XTR_TESTS_ALL_HPP
#define WATER_XTR_TESTS_ALL_HPP
#include <water/xtr/tests/char_and_strings.hpp>
#include <water/xtr/tests/char_pointer.hpp>
#include <water/xtr/tests/create.hpp>
#include <water/xtr/tests/floatingpoint.hpp>
#include <water/xtr/tests/integers.hpp>
#include <water/xtr/tests/template_deduction.hpp>
#include <water/xtr/tests/to_function.hpp>
#include <water/xtr/tests/truncate.hpp>
#include <water/xtr/tests/utf.hpp>
#include <water/xtr/tests/xtr_operator_using_str.hpp>
#include <water/xtr/tests/zeros.hpp>
#include <water/xtr/tests/fold.hpp>
namespace water { namespace xtr { namespace tests {

inline void all() {
    char_and_strings();
    char_pointer();
    create_test();
    floatingpoint();
    integers();
    template_deduction();
    to_function_test();
    truncate();
    utf();
    xtr_operator_using_str_test();
    zeros();
    fold_test();
}

}}}
#endif
