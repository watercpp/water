// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_SPIN_ONCE_HPP
#define WATER_THREADS_SPIN_ONCE_HPP
#include <water/threads/spin.hpp>
namespace water { namespace threads {
namespace _ {

    template<typename function_, typename = bool>
    struct spin_once_bool {
        static bool function(function_& f) { f(); return true; }
    };
    
    template<typename function_>
    struct spin_once_bool<function_, decltype(static_cast<bool>(types::make<function_&>()()))> {
        static bool function(function_& f) { return static_cast<bool>(f()); }
    };
    
}

// unlike threads::once this works for any class type function
// the function can throw
// the function can return false if it fails
// (if the function returns anything that converts to bool, it must convert to true on success)

template<bool exists_ = spin_exists>
class spin_once
{
    atomic_uint my{0}; // 1 is done, 2 is locked

public:
    constexpr spin_once() noexcept = default;
    spin_once(spin_once const&) = delete;
    spin_once& operator=(spin_once const&) = delete;

    explicit operator bool() const noexcept {
        return my == 1;
    }

    template<typename function_>
    bool operator()(function_ function) {
        auto now = my.load(memory_order_relaxed);
        if(now == 1)
            return true;
        spin s;
        now = 0;
        while(!my.compare_exchange_strong(now, 2, memory_order_acquire) && now == 2) {
            now = 0;
            if(!s.once()) s.times(spin_times());
            s();
        }
        if(now == 1)
            return true;
        auto_set a{&my, 0};
        bool r = _::spin_once_bool<function_>::function(function); // can throw
        if(r) a.set = 1;
        return r;
    }

private:
    struct auto_set {
        atomic_uint *to;
        unsigned set;
        ~auto_set() noexcept {
            to->store(set, memory_order_release);
        }
    };
    
};

template<>
class spin_once<false>
{};

}}
#endif
