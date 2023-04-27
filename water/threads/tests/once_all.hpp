// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_ONCE_ALL_HPP
#define WATER_THREADS_TESTS_ONCE_ALL_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/once.hpp>
#include <water/threads/spin_once.hpp>
#include <water/threads/sleep.hpp>
namespace water { namespace threads { namespace tests {

template<typename once_, typename extra_>
struct once_hold {
    static once_ once;
    static unsigned count;
};
template<typename once_, typename extra_> once_ once_hold<once_, extra_>::once;
template<typename once_, typename extra_> unsigned once_hold<once_, extra_>::count;

template<typename once_, typename extra_>
void once_add() {
    sleep(0.5);
    ++once_hold<once_, extra_>::count;
}

template<typename once_>
struct once_once {
    void operator()() {
        once_hold<once_, void>::once(&once_add<once_, void>);
        ___water_test(once_hold<once_, void>::count == 1);
    }
};

template<typename once_>
struct once_once_lambda {
    void operator()() {
        once_hold<once_, short>::once([] { once_add<once_, short>(); });
        ___water_test(once_hold<once_, short>::count == 1);
    }
};


template<typename once_>
struct once_once_heavy {
    void operator()() {
        struct { char array[123]; } heavy{};
        once_hold<once_, long>::once([heavy] {
            test_unused(heavy);
            once_add<once_, long>();
        });
        ___water_test(once_hold<once_, long>::count == 1);
    }
};

template<typename once_>
struct once_once_reference
{
    class function
    {
        void *me;
    public:
        function() :
            me{this}
        {}
        
        function(function const&) = delete;
        function& operator=(function const&) = delete;
        
        void operator()() {
            once_add<once_, function&>();
            ___water_test(me == this);
        }
    };
    
    void operator()() {
        function f;
        once_hold<once_, function&>::once(f);
        ___water_test(once_hold<once_, function&>::count == 1);
    }
};

template<typename once_>
struct once_all_test {
    once_all_test() {
        once_once<once_> o1;
        run_many(o1, 10);
        
        once_once_lambda<once_> o2;
        run_many(o2, 10);
        
        once_once_heavy<once_> o3;
        run_many(o3, 10);
        
        once_once_reference<once_> o4;
        run_many(o4, 10);
    }
};

template<>
struct once_all_test<spin_once<false>>
{};

inline void once_all() {
    once_all_test<threads::once>();
    once_all_test<spin_once<>>();
}

}}}
#endif
