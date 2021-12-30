// Copyright 2017-2021 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_FUNCTIONS_HPP
#define WATER_THREADS_FUNCTIONS_HPP
#include <water/threads/deadline.hpp>
namespace water { namespace threads {

template<typename mutex_>
auto lock(mutex_& a) -> decltype(a.lock()) {
    return a.lock();
}

template<typename thing_>
auto lock(thing_& a, double seconds) -> decltype(a.lock(seconds)) {
    return a.lock(seconds);
}

template<typename thing_>
auto lock(thing_& a, deadline d) -> decltype(a.lock(d)) {
    return a.lock(d);
}

template<typename mutex_>
auto try_lock(mutex_& a) -> decltype(a.try_lock()) {
    return a.try_lock();
}

template<typename mutex_>
auto unlock(mutex_& a) -> decltype(a.unlock()) {
    return a.unlock();
}

template<typename mutex_>
auto read_lock(mutex_& a) -> decltype(a.read_lock()) {
    return a.read_lock();
}

template<typename mutex_>
auto read_lock(mutex_& a, double seconds) -> decltype(a.read_lock(seconds)) {
    return a.read_lock(seconds);
}

template<typename mutex_>
auto read_lock(mutex_& a, deadline d) -> decltype(a.read_lock(d)) {
    return a.read_lock(d);
}

template<typename mutex_>
auto read_try_lock(mutex_& a) -> decltype(a.read_try_lock()) {
    return a.read_try_lock();
}

template<typename mutex_>
auto read_unlock(mutex_& a) -> decltype(a.read_unlock()) {
    return a.read_unlock();
}


template<typename mutex_>
class move_unlock
{
    mutex_ *my = 0;

public:
    move_unlock() noexcept = default;

    explicit move_unlock(mutex_* a) noexcept :
        my(a)
    {}

    explicit move_unlock(mutex_& a) noexcept :
        my(&a)
    {}

    ~move_unlock() noexcept {
        if(my) unlock(*my); // could panic() and throw
    }

    move_unlock(move_unlock&& a) noexcept :
        my(a.my)
    {
        a.my = 0;
    }

    move_unlock& operator=(move_unlock&& a) noexcept {
        swap(a);
        return *this;
    }

    move_unlock(move_unlock const&) = delete;
    move_unlock& operator=(move_unlock const&) = delete;

    void swap(move_unlock& a) noexcept {
        auto s = my;
        my = a.my;
        a.my = s;
    }

    void dont() noexcept {
        my = 0;
    }
};


template<typename mutex_>
void swap(move_unlock<mutex_>& a, move_unlock<mutex_>& b) noexcept {
    a.swap(b);
}

template<typename mutex_>
move_unlock<mutex_> lock_move(mutex_& a) {
    lock(a);
    return move_unlock<mutex_>{a};
}

template<typename mutex_>
move_unlock<mutex_> unlock_move(mutex_& a) {
    return move_unlock<mutex_>{a};
}


template<typename mutex_>
class move_read_unlock
{
    mutex_ *my = 0;

public:
    move_read_unlock() noexcept = default;

    explicit move_read_unlock(mutex_* a) noexcept :
        my(a)
    {}

    explicit move_read_unlock(mutex_& a) noexcept :
        my(&a)
    {}

    ~move_read_unlock() noexcept {
        if(my) read_unlock(*my); // could panic() and throw
    }

    move_read_unlock(move_read_unlock&& a) noexcept :
        my(a.my)
    {
        a.my = 0;
    }

    move_read_unlock& operator=(move_read_unlock&& a) noexcept {
        swap(a);
        return *this;
    }

    move_read_unlock(move_read_unlock const&) = delete;
    move_read_unlock& operator=(move_read_unlock const&) = delete;

    void swap(move_read_unlock& a) noexcept {
        auto s = my;
        my = a.my;
        a.my = s;
    }

    void dont() noexcept {
        my = 0;
    }
};


template<typename mutex_>
void swap(move_read_unlock<mutex_>& a, move_read_unlock<mutex_>& b) noexcept {
    a.swap(b);
}

template<typename mutex_>
move_read_unlock<mutex_> read_lock_move(mutex_& a) {
    read_lock(a);
    return move_read_unlock<mutex_>{a};
}

template<typename mutex_>
move_read_unlock<mutex_> read_unlock_move(mutex_& a) {
    return move_read_unlock<mutex_>{a};
}

template<typename semaphore_>
auto down(semaphore_& a) -> decltype(a.down()) {
    return a.down();
}

template<typename semaphore_>
auto down(semaphore_& a, double seconds) -> decltype(a.down(seconds)) {
    return a.down(seconds);
}

template<typename semaphore_>
auto down(semaphore_& a, deadline d) -> decltype(a.down(d)) {
    return a.down(d);
}

template<typename semaphore_>
auto try_down(semaphore_& a) -> decltype(a.try_down()) {
    return a.try_down();
}

template<typename semaphore_>
auto up(semaphore_& a) -> decltype(a.up()) {
    return a.up();
}

template<typename condition_, typename mutex_>
auto wait(condition_& c, mutex_& m) -> decltype(c.wait(m)) {
    return c.wait(m);
}

template<typename condition_, typename mutex_>
auto wait(condition_& c, mutex_& m, double seconds) -> decltype(c.wait(m, seconds)) {
    return c.wait(m, seconds);
}

template<typename condition_, typename mutex_>
auto wait(condition_& c, mutex_& m, deadline d) -> decltype(c.wait(m, d)) {
    return c.wait(m, d);
}

template<typename condition_>
auto wake(condition_& a) -> decltype(a.wake()) {
    return a.wake();
}

template<typename condition_>
auto wake_all(condition_& a) -> decltype(a.wake_all()) {
    return a.wake_all();
}

namespace statistics { class data; }

namespace _ {

    template<typename type_, typename = statistics::data*>
    struct statistics_pointer_if {
        static bool constexpr has = false;
        static statistics::data* do_it(type_&) noexcept { return 0; }
    };
    
    template<typename type_>
    struct statistics_pointer_if<type_, decltype(types::make<type_&>().statistics())> {
        static bool constexpr has = true;
        static statistics::data* do_it(type_& a) noexcept { return a.statistics(); }
    };
}

template<typename type_>
statistics::data* statistics_pointer(type_& a) noexcept {
    // returns 0 if the type has no statistics
    return _::statistics_pointer_if<type_>::do_it(a);
}

template<typename type_>
constexpr bool has_statistics() noexcept {
    return _::statistics_pointer_if<type_>::has;
}

}}
#endif
