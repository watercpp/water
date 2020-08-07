// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_CONDITION_PONG_HPP
#define WATER_THREADS_TESTS_CONDITION_PONG_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/condition.hpp>
#include <water/threads/mutex.hpp>
namespace water { namespace threads { namespace tests {

template<typename condition_>
class condition_pong
{
    condition_ myconditions[5];
    mutex_for_condition<condition_>
        mylock,
        mylocks[5];
    unsigned
        myat = 0,
        myvalues[5] = {0,0,0,0,1};

public:
    condition_pong() {
        unsigned i = 0;
        do {
            ___water_threads_statistics(name_if(statistics_pointer(myconditions[i])) << "water::threads::tests::condition_pong condition " << i);
            ___water_threads_statistics(name_if(statistics_pointer(mylocks[i])) << "water::threads::tests::condition_pong mutex " << i);
        } while(++i != 5);
        ___water_threads_statistics(name_if(statistics_pointer(mylock)) << "water::threads::tests::condition_pong lock");
        
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
        unsigned next = (at + 1) % 5;
        do {
            {
                auto l = lock_move(mylocks[at]);
                while(!myvalues[at]) {
                    ___water_test(myconditions[at].wait(mylocks[at]));
                }
                --myvalues[at];
            }
            {
                auto l = lock_move(mylocks[next]);
                ++myvalues[next];
                ___water_test(myconditions[next].wake());
            }
        } while(--times);
    }
};

}}}
#endif
