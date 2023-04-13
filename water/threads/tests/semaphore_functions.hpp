// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_SEMAPHORE_FUNCTIONS_HPP
#define WATER_THREADS_TESTS_SEMAPHORE_FUNCTIONS_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/semaphore.hpp>
namespace water { namespace threads { namespace tests {

/*

test semaphore functions

*/

template<typename semaphore_>
class semaphore_functions
{
public:
    semaphore_functions() {
        semaphore_ a, b(2);
        ___water_threads_statistics(name_if(statistics_pointer(a)) << "water::threads::tests::semaphore_functions a");
        ___water_threads_statistics(name_if(statistics_pointer(b)) << "water::threads::tests::semaphore_functions b");
        ___water_test(a);
        ___water_test(b);
        ___water_test(down(b));
        spin_if(b);
        ___water_test(b.down());
        ___water_test(!a.try_down());
        ___water_test(!b.try_down());
        ___water_test(a.up());
        ___water_test(up(b));
        timeout_if(a);
    }

private:
    template<typename a_>
    void timeout_if(a_& a, ifel<has_timeout<m_>(), int> = 0) {
        ___water_test(down(a, 0.01));
        ___water_test(up(a));
        ___water_test(a.down(0.01));
        ___water_test(a.up());
        ___water_test(down(a, deadline(0.01)));
        ___water_test(up(a));
        ___water_test(a.down(deadline(0.01)));
        ___water_test(!a.down(0.001));
        ___water_test(!a.down(deadline(0.001)));
    }

    template<typename s_>
    void timeout_if(s_&, ifel<!has_timeout<m_>(), int> = 0) {
    }

    template<typename a_>
    void spin_if(a_& a, ifel<is_spin<m_>(), int> = 0) {
        a.spin_times(1000);
    }

    template<typename a_>
    void spin_if(a_& a, ifel<!is_spin<m_>(), int> = 0) {
    }
};


}}}
#endif
