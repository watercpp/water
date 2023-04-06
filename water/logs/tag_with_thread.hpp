// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_LOGS_TAG_WITH_THREAD_HPP
#define WATER_LOGS_TAG_WITH_THREAD_HPP
#include <water/logs/bits.hpp>
#include <water/str/out.hpp>
#include <water/threads/thread.hpp>
namespace water { namespace logs {


class tag_with_thread
{
    threads::thread_t my;

public:
    
    tag_with_thread() :
        my{threads::thread()}
    {}
    
    threads::thread_t thread() const {
        return my;
    }
};


template<typename o_>
str::out<o_>& operator<<(str::out<o_>& o, tag_with_thread const& t) {
    return o << t.thread();
}


}}
#endif
