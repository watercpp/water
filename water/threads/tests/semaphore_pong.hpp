// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_SEMAPHORE_PONG_HPP
#define WATER_THREADS_TESTS_SEMAPHORE_PONG_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/semaphore.hpp>
#include <water/threads/mutex.hpp>
namespace water { namespace threads { namespace tests {

// gcc does not like mysemaphores{{0},{},{0},{},{1}} when the semaphore constructor is not constexpr

template<typename semaphore_>
class semaphore_pong
{
    semaphore_ my0, my1, my2, my3, my4;
    typename types::ifel<is_spin<semaphore_>(), mutex<need_spin>, mutex<>>::result mylock;
    unsigned myat = 0;

public:
    semaphore_pong() :
        my0(0),
        my2(0),
        my4(1)
    {
        for(unsigned i = 0; i != 5; ++i) {
            ___water_test(*(&my0 + i));
            ___water_threads_statistics(name_if(statistics_pointer(*(&my0 + i))) << "water::threads::tests::semaphore_pong " << i);
        }
        ___water_threads_statistics(name_if(statistics_pointer(mylock)) << "water::threads::tests::semaphore_pong lock");
        run_many_reference(*this, 5);
    }

    void operator()() {
        unsigned times = 100;
        unsigned at = 0;
        {
            auto l = lock_move(mylock);
            at = myat;
            ++myat;
        }
        do {
            ___water_test((&my0 + at)->down());
            ___water_test((&my0 + (at + 1) % 5)->up());
        } while(--times);
    }
};

}}}
#endif
