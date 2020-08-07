// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_RUN_MANY_HPP
#define WATER_THREADS_TESTS_RUN_MANY_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/thread.hpp>
namespace water { namespace threads { namespace tests {

// run and join many

constexpr unsigned run_many_max = 512;

template<typename function_>
unsigned run_many(function_ function, unsigned count) {
    if(!count || count > run_many_max)
        return 0;
    unsigned at = 0;
    join_t list[run_many_max];
    do {
        bool run_ok = run(function, list[at]);
        ___water_test(run_ok);
        if(!run_ok) --count;
        else ++at;
    } while(at != count);
    at = 0;
    while(at != count) {
        bool join_ok = join(list[at]);
        ___water_test(join_ok);
        ++at;
    }
    return at;
}

template<typename function_>
unsigned run_many_reference(function_& function, unsigned count) {
    return run_many<function_&>(function, count);
}

}}}
#endif
