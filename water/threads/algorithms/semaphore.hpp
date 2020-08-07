// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_ALGORITHMS_SEMAPHORE_HPP
#define WATER_THREADS_ALGORITHMS_SEMAPHORE_HPP
#include <water/threads/algorithms/bits.hpp>
namespace water { namespace threads { namespace algorithms {

/*

atomic semaphore to use on top of a regular semaphore

atomic:
- low half: semaphore value
- high half: number of waiting threads

*/

template<typename atomic_unsigned_>
class semaphore
{
    using unsigned_ = decltype(atomic_unsigned_{}.load());
    static constexpr unsigned_
        shift = numeric_limits<unsigned_>::digits / 2,
        max = (static_cast<unsigned_>(1) << shift) - 1;
    
    atomic_unsigned_ *my;
    bool mywaiting = false;

public:
    semaphore(atomic_unsigned_& a) noexcept :
        my{&a}
    {}

    bool down() noexcept {
        // returns true if success
        bool did;
        auto a = my->load(memory_order_relaxed);
        do {
            did = false;
            auto b = a;
            if(b & max) {
                did = true;
                --b;
                if(mywaiting) b -= max + 1;
            }
            else if(!mywaiting)
                b += max + 1;
            if(!my->compare_exchange_weak(a, b))
                continue;
            mywaiting = !did;
        } while(false);
        return did;
    }

    bool down_timeout() noexcept {
        // returns true if it was able to down
        if(!mywaiting)
            return false;
        bool did;
        auto a = my->load(memory_order_relaxed);
        do {
            did = false;
            auto b = a;
            if(b & max) {
                did = true;
                --b;
            }
            b -= max + 1;
            if(!my->compare_exchange_weak(a, b))
                continue;
        } while(false);
        return did;
    }

    bool try_down() noexcept {
        bool r;
        auto a = my->load(memory_order_relaxed);
        while(!my->compare_exchange_weak(a, (r = (a & max) != 0) == true ? a - 1 : a));
        return r;
    }

    unsigned up(unsigned n = 1) noexcept {
        // return number of threads to wake
        if(!n) return 0;
        auto wake = my->fetch_add(n) >> shift;
        return wake > n ? n : static_cast<unsigned>(wake);
    }
};

}}}
#endif
