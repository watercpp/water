// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_THREAD_RELATIVE_PRIORITY_HPP
#define WATER_THREADS_TESTS_THREAD_RELATIVE_PRIORITY_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/thread.hpp>
#include <water/threads/mutex.hpp>
namespace water { namespace threads { namespace tests {


struct thread_relative_priority
{
    thread_relative_priority() {
        if(relative_priority_exists) {
            unsigned constexpr count = 5;
            mutex<> lock;
            relative_priority_t relative[count] {
                priority_lower,
                priority_low,
                priority_normal,
                priority_high,
                priority_higher
            };
            join_t joins[count];
            unsigned
                at = 0,
                counter = 0;
            do {
                bool run_ok = run_copy(
                    [set = relative[(at + 1) % count], &lock, &counter] {
                        bool ok = relative_priority(set);
                        ___water_test(ok);
                        auto l = lock_move(lock);
                        ++counter;
                    },
                    joins[at],
                    run_options().relative_priority(relative[at])
                );
                ___water_test(run_ok);
                if(!run_ok)
                    break;
            } while(++at != count);
            while(at)
                join(joins[--at]);
            ___water_test(counter == count);
        }
    }
};

}}}
#endif
