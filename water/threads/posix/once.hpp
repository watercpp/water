// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_POSIX_ONCE_HPP
#define WATER_THREADS_POSIX_ONCE_HPP
#include <water/threads/posix/bits.hpp>
namespace water { namespace threads {


template<typename = void>
thread_local void* once_pointer = 0;

template<typename function_, typename = void>
constexpr bool use_once_pointer = true;

template<typename function_>
constexpr bool use_once_pointer<function_, to_void<decltype(static_cast<void(*)()>(make_type<function_&>()))>> = false;


class once
{
    pthread_once_t my = PTHREAD_ONCE_INIT;

public:
    constexpr once() noexcept = default;
    once(once const&) = delete;
    once& operator=(once const&) = delete;
    
    template<typename function_>
    auto operator()(function_&& a) noexcept -> ifel<!use_once_pointer<function_>, void> {
        int e = pthread_once(&my, static_cast<void(*)()>(a));
        ___water_assert(!e);
        panic_if(e);
    }
    
    template<typename function_>
    auto operator()(function_&& a) noexcept -> ifel<use_once_pointer<function_>, void> {
        once_pointer<> = &a;
        int e = pthread_once(&my, &once::pointer<function_>);
        ___water_assert(!e);
        panic_if(e);
    }

private:
    
    template<typename function_>
    static void pointer() noexcept {
        (*static_cast<no_reference<function_>*>(once_pointer<>))();
    }
};

}}
#endif
