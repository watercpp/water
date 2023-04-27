// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_ONCE_HPP
#define WATER_THREADS_ONCE_HPP
#include <water/threads/configuration.hpp>
#include WATER_THREADS_INCLUDE(once.hpp)
namespace water { namespace threads {

#if 0

class once
{
public:
    constexpr once() noexcept = default;
    once(once const&) = delete;
    once& operator=(once const&) = delete;
    
    template<typename function_>
    void operator()(function_&& function) noexcept;
    // calls function(), do not throw from it
    // threads::panic() if it fails
};

#endif

}}
#endif
