// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_MUTEX_FUNCTIONS_HPP
#define WATER_THREADS_TESTS_MUTEX_FUNCTIONS_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/mutex.hpp>
namespace water { namespace threads { namespace tests {

/*

test mutex functions

*/

template<typename mutex_>
class mutex_functions
{
    mutex_ my;

public:
    mutex_functions() {
        ___water_threads_statistics(name_if(statistics_pointer(my)) << "water::threads::tests::mutex_functions");
        bool locked = false;
        {
            auto l = lock_move(my);
            if(is_recursive<mutex_>()) {
                unsigned n = 0;
                do lock(my); while(++n != 10);
                do unlock(my); while(--n);
            }
        }
        spin_if(my, static_cast<ifel<is_spin<mutex_>(), bool, void>*>(0));
        my.lock();
        my.unlock();
        lock(my);
        unlock(my);
        ___water_test(locked = try_lock(my));
        if(locked)
            auto l = unlock_move(my);
        ___water_test(locked = my.try_lock());
        if(locked)
            my.unlock();
        timeout_if(my, static_cast<ifel<has_timeout<mutex_>(), bool, void>*>(0));
    }

private:
    template<typename m_>
    void timeout_if(m_& m, bool*) {
        bool locked;
        ___water_test(locked = lock(m, 0.01));
        if(locked)
            unlock(m);
        ___water_test(locked = lock(m, deadline(0.01)));
        if(locked)
            m.unlock();
    }

    template<typename m_>
    void timeout_if(m_&, void*) {
    }

    template<typename m_>
    void spin_if(m_& m, bool*) {
        m.spin_times(1000);
    }

    template<typename m_>
    void spin_if(m_&, void*) {
    }
};


}}}
#endif
