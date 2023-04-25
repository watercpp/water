// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MUTEX_HPP
#define WATER_MUTEX_HPP

/*

std::mutex and std::lock_guard are used normally.

#define WATER_USE_WATER_THREADS to use water::threads

#define WATER_MUTEX to replace it with your own mutex:

    namespace space {
        struct special_mutex {
            constexpr special_mutex();
            void lock();
            bool try_lock();
            void unlock();
        };
    };

    #define WATER_MUTEX space::special_mutex

*/

#if !defined(WATER_MUTEX) && !defined(WATER_NO_STD) && !defined(WATER_USE_WATER_THREADS)

#include <mutex>
namespace water {

    using std::mutex;
    using lock_guard = std::lock_guard<mutex>;
    using std::adopt_lock;

}

#else

#ifndef WATER_MUTEX
    #include <water/threads/mutex.hpp>
#endif
namespace water {
    
    #ifdef WATER_MUTEX
    using mutex = WATER_MUTEX;
    #else
    using mutex = threads::mutex<threads::need_constexpr_constructor>;
    #endif

    struct adopt_lock_type {};

    constexpr adopt_lock_type adopt_lock;

    class lock_guard
    {
        mutex *my;
    
    public:
    
        explicit lock_guard(mutex& m) :
            my{&m}
        {
            my->lock();
        }
    
        lock_guard(mutex& m, adopt_lock_type) :
            my{&m}
        {}
    
        ~lock_guard() {
            my->unlock();
        }
    
        lock_guard(lock_guard const&) = delete;
        lock_guard& operator=(lock_guard const&) = delete;
    };

}

#endif
#endif
