// Copyright 2017-2024 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_STR_TESTS_ALL_HPP
#define WATER_STR_TESTS_ALL_HPP
#include <water/str/tests/construct.hpp>
#include <water/str/tests/create.hpp>
#include <water/str/tests/default_settings_locale_flush.hpp>
#include <water/str/tests/everything.hpp>
#include <water/str/tests/buffer_and_buffer_lines.hpp>
#include <water/str/tests/function.hpp>
#include <water/str/tests/function_type_detect.hpp>
#include <water/str/tests/lifetime.hpp>
#include <water/str/tests/list.hpp>
#include <water/str/tests/rvalue.hpp>
namespace water { namespace str { namespace tests {

inline void all() {
    construct_all();
    everything_all();
    buffer_and_buffer_lines_all();
    function_all();
    function_type_detect_all();
    lifetime_all();
    rvalue_all();
    default_settings_locale_flush_all();
    create_all();
    list_all();
}

}}}
#endif
