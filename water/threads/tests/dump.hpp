// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_DUMP_HPP
#define WATER_THREADS_TESTS_DUMP_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/thread.hpp>
#include <water/threads/barrier.hpp>
#include <water/threads/condition.hpp>
#include <water/threads/mutex.hpp>
#include <water/threads/read_write.hpp>
#include <water/threads/semaphore.hpp>
#include <water/str/str.hpp>
#include <water/str/type_name.hpp>
namespace water { namespace threads { namespace tests {

template<typename type_>
struct dump_type {
    template<typename o_>
    dump_type(str::out<o_> *o, char const *prefix) {
        *o << prefix << str::type_name<type_>() << ' ';
        if(is_system<type_>()) *o << "system ";
        if(is_water<type_>()) *o << "water ";
        if(has_timeout<type_>()) *o << "timeout ";
        if(is_recursive<type_>()) *o << "recursive ";
        if(has_constexpr_constructor<type_>()) *o << "constexpr_constructor ";
        if(has_trivial_destructor<type_>()) *o << "trivial_destructor ";
        if(is_spin<type_>()) *o << "spin ";
        if(has_statistics<type_>()) *o << "statistics ";
        *o << '\n';
    }
};

template<typename o_>
void dump(str::out<o_>& o) {
    o << "thread_t " << str::type_name<thread_t>() << '\n';
    o << "priority_exists " << stack_size_exists << '\n';
    priority p;
    o << "priority.get() " << p.get() << '\n';
    o << "priority.min() " << p.min() << '\n';
    o << "priority.max() " << p.max() << '\n';
    o << "stack_size_exists " << stack_size_exists << '\n';
    test_list<dump_type, barrier_list>(&o, "barrier ");
    test_list<dump_type, condition_list>(&o, "condition ");
    test_list<dump_type, mutex_list>(&o, "mutex ");
    test_list<dump_type, read_write_list>(&o, "read_write ");
    test_list<dump_type, semaphore_list>(&o, "semaphore ");
}

}}}
#endif
