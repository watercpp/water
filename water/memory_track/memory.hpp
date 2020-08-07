// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MEMORY_TRACK_MEMORY_HPP
#define WATER_MEMORY_TRACK_MEMORY_HPP
#include <water/memory_track/allocator.hpp>
#include <water/memory_track/cookie.hpp>
#include <water/memory_track/statistics.hpp>
#include <water/memory_track/statistics_by_bytes.hpp>
#include <water/memory_track/statistics_by_name.hpp>
#include <water/memory_track/underlying_allocator.hpp>
#include <water/threads/mutex.hpp>
#include <water/align_max.hpp>
namespace water { namespace memory_track {

/*

Memory tracking allocator

[..cookie..][..pattern..][.......memory..........][..pattern..]
                          ^
                          |
                          aligined to water::align_max

Cookie cannot be after memory, because the memory size is not always known

*/

template<typename tag_ = void, typename callback_ = void>
class memory
{
public:
    using cookie_type = cookie<tag_>;
    using tag_type = typename cookie_type::tag_type;
    using cookie_iterator = memory_track::cookie_iterator<cookie_type>;
    using callback_type = typename types::if_not_void<callback_, no_callback>::result;
    static constexpr size_t
        align = align_max,
        overhead =
            sizeof(cookie_type) +
            pattern_size +
            pattern_size +
            ((sizeof(cookie_type) + pattern_size) % align ? align - ((sizeof(cookie_type) + pattern_size) % align) : 0);

private:
    mutable threads::mutex<threads::need_constexpr_constructor> mylock;
    cookie_type *mycookies = 0;
    large_t myallocations = 0;
    statistics_by_bytes mybytes;
    statistics_by_name mynames;
    statistics mystatistics_current; // can reset
    statistics mystatistics_total; // never reset
    size_t mygeneration = 0; // changes on each reset/resize
    callback_type mycallback{};

public:
    constexpr memory() = default;
    memory(memory const&) = delete;
    memory& operator=(memory const&) = delete;
    
    void callback(callback_type const& a) {
        auto lock = lock_move(mylock);
        mycallback = a;
    }

    callback_type callback() const {
        auto lock = lock_move(mylock);
        return mycallback;
    }
    
    template<typename function_>
    void cookies_to(function_&& function) const {
        auto lock = lock_move(mylock);
        function(cookie_iterator{mycookies});
    }

    cookie_type const* cookie_from_pointer(void const* pointer) const noexcept {
        // retrun cookie from any pointer, if it points to a location inside any allocation.
        // returns 0 if it was not found.
        //
        // this is a slow linear search. use cookie_from_pointer_from_allocate() if pointer is
        // what allocate() returned
        //
        auto lock = lock_move(mylock);
        byte const *p = static_cast<byte const*>(pointer);
        if(mystatistics_total.address_lowest <= p && p <= mystatistics_total.address_highest) {
            if(auto c = mycookies) {
                do {
                    if(c->pointer <= p && p < c->pointer + c->bytes)
                        return c;
                } while((c = c->next) != mycookies);
            }
        }
        return 0;
    }

    cookie_type const* cookie_from_pointer_from_allocate(void const* pointer) const noexcept {
        // return cookie from pointer, 0 if error
        // make sure the pointer is what allocate() returned. otherwise use cookie_from_pointer()
        //
        // this uses some error checking of the pointer, but should be very quick
        auto lock = lock_move(mylock);
        byte const *p = static_cast<byte const*>(pointer);
        if(
            p &&
            !(upointer(p) % align) &&
            mystatistics_total.count_now &&
            mystatistics_total.address_lowest <= p &&
            p <= mystatistics_total.address_highest
        ) {
            auto c = static_cast<cookie_type const*>(static_cast<void const*>(p - (overhead - pattern_size)));
            if(c->pointer == p)
                return c;
        }
        return 0;
    }
    
    bool statistics_reset() noexcept {
        // reset statisitcs
        auto lock = lock_move(mylock);
        mybytes.reset();
        mynames.reset();
        mystatistics_current = {};
        ++mygeneration;
        return true;
    }

    template<typename function_>
    size_t statistics_resize(function_&& sizer, size_t bytes_max = 0) noexcept {
        // resize statisitcs_by_bytes, resets statistics
        auto lock = lock_move(mylock);
        size_t r = mybytes.resize(sizer, bytes_max);
        if(r) {
            mynames.reset();
            mystatistics_current = {};
            ++mygeneration;
        }
        return r;
    }
    
    template<typename function_>
    void statistics_by_bytes_to(function_&& function) const {
        auto lock = lock_move(mylock);
        function(mybytes.begin(), mybytes.end());
    }
    
    template<typename function_>
    void statistics_by_name_to(function_&& function) const {
        auto lock = lock_move(mylock);
        function(mynames.begin(), mynames.end());
    }

    memory_track::statistics_for_bytes statistics_for_bytes(size_t a) const noexcept {
        auto lock = lock_move(mylock);
        return mybytes.find_and_copy(a);
    }

    memory_track::statistics_for_name statistics_for_name(char const* a) const noexcept {
        auto lock = lock_move(mylock);
        return mynames.find_and_copy(a);
    }

    statistics statistics_current() const noexcept {
        auto lock = lock_move(mylock);
        return mystatistics_current;
    }

    statistics statistics_total() const noexcept {
        auto lock = lock_move(mylock);
        return mystatistics_total;
    }
    
    struct cookie_error {
        cookie_type const *cookie = 0;
        void const *where = 0;
        char const *error = 0;
        explicit operator bool() const noexcept {
            return error != 0;
        }
    };

private:
    cookie_error cookie_error_from(cookie_type const* c) const noexcept {
        byte const
            *lowest = mystatistics_total.address_lowest - (overhead - pattern_size),
            *highest = mystatistics_total.address_highest - (overhead - pattern_size);
        cookie_error r;
        r.cookie = c;
        auto b = static_cast<byte const*>(static_cast<void const*>(c));
        if(upointer(c) % alignof(cookie_type)) {
            r.where = c;
            r.error = "cookie not aligned";
        }
        else if(c->pointer != b + (overhead - pattern_size)) {
            r.where = &c->pointer;
            r.error = "cookie pointer";
        }
        else if(
            (upointer(c->next) % alignof(cookie_type)) ||
            static_cast<void const*>(c->next) < lowest ||
            static_cast<void const*>(c->next) > highest
        ) {
            r.where = &c->next;
            r.error = "cookie next pointer";
        }
        else if(
            (upointer(c->prev) % alignof(cookie_type)) ||
            static_cast<void const*>(c->prev) < lowest ||
            static_cast<void const*>(c->prev) > highest
        ) {
            r.where = &c->next;
            r.error = "cookie prev pointer";
        }
        else if(c->pointer + c->bytes > mystatistics_total.address_highest) {
            r.where = &c->bytes;
            r.error = "cookie bytes";
        }
        else if(!c->id || c->id > myallocations) {
            r.where = &c->id;
            r.error = "cookie id";
        }
        else if(c->generation > mygeneration) {
            r.where = &c->generation;
            r.error = "cookie generation";
        }
        else if((r.where = c->overrun()) != 0)
            r.error = "overrun";
        else if((r.where = c->underrun()) != 0)
            r.error = "underrun";
        else if(c->next->prev != c) {
            r.where = &c->next;
            r.error = "cookie next->prev is not cookie";
        }
        else if(c->prev->next != c) {
            r.where = &c->prev;
            r.error = "cookie prev->next is not cookie";
        }
        else
            return {};
        return r;
    }

public:
    
    cookie_error look_for_memory_corruption() const noexcept {
        // returns cookie + where + error-message if memory corruption was found.
        // this could find overrun or underrun bugs
        auto lock = lock_move(mylock);
        if(auto c = mycookies) {
            do {
                if(cookie_error r = cookie_error_from(c))
                    return r;
            } while((c = c->next) != mycookies);
        }
        return {};
    }
    
    void* allocate(size_t bytes, tag_type const& tag, allocator_tag tag2) noexcept {
        // bytes can be 0
        ___water_assert(bytes < static_cast<size_t>(-1) / 2 && "size_t overflow"); // this has to be an error
        auto lock = lock_move(mylock);
        ___water_threads_statistics(
            if(!myallocations)
                name_if(statistics_pointer(mylock)) << "water::memory_track::memory";
        )
        if(!mybytes && !mybytes.resize())
            return 0;
        memory_track::statistics_for_bytes *b = mybytes.find(bytes);
        auto n = mynames.find_or_add(tag.name());
        if(!n)
            return 0;
        byte *a = 0;
        if(bytes < static_cast<size_t>(-1) / 2) // also means bytes + overhead wont overflow
            a = static_cast<byte*>(underlying_allocator_nothrow{}.allocate(bytes + overhead));
        byte *r = 0;
        cookie_type *c = 0;
        if(a) {
            r = a + (overhead - pattern_size);
            pattern_set(a + sizeof(cookie_type), bytes + overhead - sizeof(cookie_type));
            add(myallocations);
            c = new(here(a)) cookie_type{0, 0, myallocations, r, bytes, tag, tag2, mygeneration};
            if(!mycallback.allocate(static_cast<cookie_type const*>(c), mystatistics_total.bytes_now + bytes)) { // could overflow in theory
                sub(myallocations);
                c->~cookie_type();
                underlying_allocator_nothrow{}.free(a, bytes + overhead);
                a = 0;
            }
        }
        if(!a) {
            b->failure(bytes);
            n->failure(bytes);
            mystatistics_current.failure(bytes);
            mystatistics_total.failure(bytes);
            return 0;
        }
        b->success(r, bytes);
        n->success(r, bytes);
        mystatistics_current.success(r, bytes);
        mystatistics_total.success(r, bytes);
        if(mycookies) {
            // insert before mycookies, list from newest to oldest
            c->prev = mycookies->prev;
            c->next = mycookies;
            c->prev->next = c;
            c->next->prev = c;
            mycookies = c;
        }
        else
            mycookies = c->next = c->prev = c;
        lock = {}; // assert outside lock
        ___water_assert(!(upointer(c) % align) && !(upointer(r) % align) && "underlying_allocator allocation not aligend to water::align_max");
        return r;
    }

    void free(void *pointer, size_t bytes, tag_type const& tag, allocator_tag tag2, bool ___water_debug(assert_enabled) = true) noexcept {
        // bytes = 0 is ok because delete
        auto lock = lock_move(mylock);
        char const* error = 0;
        cookie_type *c = 0; // out here for assert below
        if(!pointer)
            error = "pointer is 0";
        else if(!mycookies)
            error = "no allocations exist";
        else if(upointer(pointer) % align)
            error = "not aligned to water::align";
        else if(static_cast<size_t>(-1) - bytes < overhead)
            error = "size_t overflow";
        else if(bytes >= static_cast<size_t>(-1) / 2)
            error = "size_t overflow";
        else if(static_cast<byte*>(pointer) < mystatistics_total.address_lowest)
            error = "address lower than address_lowest";
        else if(static_cast<byte*>(pointer) > mystatistics_total.address_highest) // for 0 size pointer can be equal to highest address
            error = "address higher than address_highest";
        else {
            void *p = static_cast<byte*>(pointer) - (overhead - pattern_size);
            c = static_cast<cookie_type*>(p);
            if(auto e = cookie_error_from(c))
                error = e.error;
            else if(bytes && c->bytes != bytes)
                error = "cookie size missmatch";
            else if(c->tag != tag)
                error = "tag missmatch";
            else if(c->allocator != tag2)
                error = "allocator tag missmatch";
            else {
                c->next->prev = c->prev;
                c->prev->next = c->next;
                if(mycookies == c && (mycookies = c->next) == c)
                    mycookies = 0;
                if(c->generation == mygeneration) {
                    if(auto b = mybytes.find(c->bytes))
                        b->free(c->bytes);
                    if(auto n = mynames.find(c->tag.name()))
                        n->free(c->bytes);
                    mystatistics_current.free(c->bytes);
                }
                mystatistics_total.free(c->bytes);
                mycallback.free(static_cast<cookie_type const*>(c));
                size_t s = c->bytes + overhead;
                c->~cookie_type();
                pattern_set(p, s); // if its used after free
                underlying_allocator_nothrow{}.free(p, s);
                return;
            }
        }
        if(!mycallback.free_error(static_cast<cookie_type const*>(c), pointer, bytes, tag, error))
            return;
        lock = {}; // unlock before trace/assert, dont deadlock if they use this
        ___water_debug(trace() << "water::memory_track free error pointer=" << pointer << " bytes=" << bytes << " tag=" << tag.name() << " error=" << error;)
        ___water_assert(!assert_enabled || !error);
    }
};

template<typename tag_, typename callback_>
allocator_throw<memory<tag_, callback_> > allocator_for(memory<tag_, callback_>& a, typename memory<tag_, callback_>::tag_type const& t) noexcept {
    return allocator_throw<memory<tag_, callback_> >{a, t};
}

template<typename tag_, typename callback_>
allocator_throw<memory<tag_, callback_> > allocator_for(memory<tag_, callback_>& a) noexcept {
    return allocator_throw<memory<tag_, callback_> >{a};
}

template<typename tag_, typename callback_>
allocator_nothrow<memory<tag_, callback_> > allocator_nothrow_for(memory<tag_, callback_>& a, typename memory<tag_, callback_>::tag_type const& t) noexcept {
    return allocator_nothrow<memory<tag_, callback_> >{a, t};
}

template<typename tag_, typename callback_>
allocator_nothrow<memory<tag_, callback_> > allocator_nothrow_for(memory<tag_, callback_>& a) noexcept {
    return allocator_nothrow<memory<tag_, callback_> >{a};
}

}}
#endif
