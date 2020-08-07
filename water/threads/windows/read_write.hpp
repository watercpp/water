// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_WINDOWS_READ_WRITE_HPP
#define WATER_THREADS_WINDOWS_READ_WRITE_HPP
#include <water/threads/windows/srwlock.hpp>
#include <water/threads/windows/read_write_count.hpp>
#include <water/threads/spin_read_write.hpp>
namespace water { namespace threads {

using read_write_list = list<
    srwlock,
    read_write_count,
    spin_read_write<>
>;

}}
#endif
