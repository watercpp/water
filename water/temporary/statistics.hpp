// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TEMPORARY_STATISTICS_HPP
#define WATER_TEMPORARY_STATISTICS_HPP
#include <water/temporary/bits.hpp>
namespace water { namespace temporary {
namespace _ { namespace statistic {

    using large_t = uint_largest_t;
    
    template<typename unsigned_>
    void add(unsigned_& to, size_t add = 1) {
        if(to < static_cast<unsigned_>(-1) - add)
            to += add;
        else
            to = static_cast<unsigned_>(-1);
    }

    // keep the most recent + frequent allocated sizes
    class size_history
    {
    public:
        struct value_type {
            large_t update;
            size_t size;
            size_t count;
        };
        static unsigned constexpr capacity = 32;

    private:
        value_type my[capacity] {};
        large_t myupdate = 0;
        unsigned mysize = 0;

    public:
        void operator()(size_t a) noexcept {
            value_type *i = my, *u = my, *e = my + mysize;
            while(i != e) {
                if(i->size == a)
                    break;
                if(u->update > i->update)
                    u = i;
                ++i;
            }
            if(i == e) {
                if(mysize == capacity)
                    i = u;
                else
                    i = my + mysize++;
                i->size = a;
                i->count = 0;
            }
            add(myupdate);
            i->update = myupdate;
            add(i->count);
        }

        value_type const* begin() const noexcept {
            return my;
        }

        value_type const* end() const noexcept {
            return my + mysize;
        }
    };
    
    struct allocation {
        large_t count = 0;
        large_t size_total = 0;
        size_t size_min = 0;
        size_t size_max = 0;
        statistic::size_history size_history;
        
        void operator()(size_t size) noexcept {
            add(count);
            add(size_total, size);
            if(!size_min || size < size_min) size_min = size;
            if(size > size_max) size_max = size;
            size_history(size);
        }
    };
    
    struct allocation_success_failure {
        allocation success;
        allocation failure;
    };
    
    struct success_failure {
        large_t success;
        large_t failure;
        
        template<typename bool_>
        bool_ operator()(bool_ a) noexcept {
            add(a ? success : failure);
            return a;
        }
    };
    
    struct pop_success_failure {
        large_t popped = 0; // when free pointer did move
        large_t success = 0;
        large_t failure = 0; // when memory not found
    };

}}

// use temporary::memory_track to collect these statistics
struct statistics {
    char name[64] = {};
    _::statistic::allocation_success_failure
        block_allocation,
        push,
        push_at_least;
    size_t
        block_count_max = 0,
        block_memory_use_peak = 0,
        retain_max = 0;
    _::statistic::pop_success_failure
        pop;
    _::statistic::success_failure
        find,
        release,
        retain,
        resize;
    
    void rename(char const* a) noexcept {
        char *n = name, *ne = n + sizeof(name) - 1;
        if(a)
            while(n != ne && *a)
                *n++ = *a++;
        *n = 0;
    }
};

namespace _ { namespace statistic {

    template<typename out_, size_t size_>
    void out(out_& o, char const (&n)[size_], allocation_success_failure const& a) {
        o << "  " << n << '\n'
            << "    success count ...... " << a.success.count << '\n'
            << "    success size_total . " << a.success.size_total << '\n'
            << "    success size_min ... " << a.success.size_min << '\n'
            << "    success size_max ... " << a.success.size_max << '\n'
            << "    success size_history (bytes*count)";
        for(auto const& h : a.success.size_history)
            o << ' ' << h.size << '*' << h.count;
        o << '\n';
        o << "    failure count ...... " << a.failure.count << '\n'
            << "    failure size_total . " << a.failure.size_total << '\n'
            << "    failure size_min ... " << a.failure.size_min << '\n'
            << "    failure size_max ... " << a.failure.size_max << '\n'
            << "    failure size_history (bytes*count)";
        for(auto const& h : a.failure.size_history)
            o << ' ' << h.size << '*' << h.count;
        o << '\n';
    }

    template<typename out_, size_t size_>
    void out(out_& o, char const (&n)[size_], success_failure const& a) {
        o << "  " << n << '\n'
            << "    success ............ " << a.success << '\n'
            << "    failure ............ " << a.failure << '\n';
    }

}}

template<typename out_>
void out(out_& o, statistics const& a) {
    // str::out or xtr::to_something
    if(a.name[0])
        o << "water::temporary::statistics " << a.name << '\n';
    else
        o << "water::temporary::statistics " << &a << '\n';
    
    _::statistic::out(o, "block_allocation", a.block_allocation);
    o << "  block_count_max ...... " << a.block_count_max << '\n'
        << "  block_memory_use_peak  " << a.block_memory_use_peak << '\n';
    
    _::statistic::out(o, "push", a.push);
    _::statistic::out(o, "push_at_least", a.push_at_least);
    o << "  pop\n"
        << "    popped ............. " << a.pop.popped << '\n'
        << "    success ............ " << a.pop.success << '\n'
        << "    failure ............ " << a.pop.failure << '\n';
    _::statistic::out(o, "resize", a.resize);
    _::statistic::out(o, "retain", a.retain);
    o << "    retain_max ......... " << a.retain_max << '\n';
    _::statistic::out(o, "release", a.release);
    _::statistic::out(o, "find", a.find);
}

}}
#endif
