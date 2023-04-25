// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MEMORY_TRACK_MEMORY_HPP
#define WATER_MEMORY_TRACK_MEMORY_HPP
#include <water/memory_track/allocator.hpp>
#include <water/memory_track/cookie.hpp>
#include <water/memory_track/names.hpp>
#include <water/memory_track/statistics.hpp>
#include <water/memory_track/statistics_by_bytes.hpp>
#include <water/memory_track/statistics_by_name.hpp>
#include <water/memory_track/underlying_allocator.hpp>
#include <water/mutex.hpp>
#include <water/std_allocator.hpp>
namespace water { namespace memory_track {


/*

Memory tracking allocator

[..cookie..][..pattern..][.......memory..........][..pattern..]
                          ^
                          |
                          aligined to water::align_allocations

Cookie cannot be after memory, because the memory size is not always known

*/


template<typename tag_ = void, typename callback_ = void>
class memory
{
public:
    using cookie_type = cookie<tag_>;
    using tag_type = if_not_void<tag_, no_tag>;
    using cookie_iterator = memory_track::cookie_iterator<cookie_type>;
    using callback_type = if_not_void<callback_, no_callback>;
    static constexpr size_t overhead =
        sizeof(cookie_type) +
        pattern_size +
        pattern_size +
        ((sizeof(cookie_type) + pattern_size) % align_allocations ? align_allocations - ((sizeof(cookie_type) + pattern_size) % align_allocations) : 0);

private:
    mutable mutex mylock;
    cookie_type *mycookies = 0;
    large_t myallocations = 0;
    names mynames;
    statistics_by_bytes mystatistics_by_bytes;
    statistics_by_name mystatistics_by_name;
    statistics mystatistics_current; // can reset
    statistics mystatistics_total; // never reset
    large_t mystatistics_allocations = 0; // set to myallocations on each statisic reset, so previous allocations can be ignored
    cookie_type const* mycookie_address_min = 0;
    cookie_type const* mycookie_address_max = 0;
    callback_type mycallback{};

public:

    constexpr memory() = default;
    memory(memory const&) = delete;
    memory& operator=(memory const&) = delete;
    
    void callback(callback_type const& a) {
        lock_guard lock{mylock};
        mycallback = a;
    }

    callback_type callback() const {
        lock_guard lock{mylock};
        return mycallback;
    }
    
    template<typename function_>
    void cookies_to(function_&& function) const {
        // function must not use this to allocate memory, because it will deadlock
        lock_guard lock{mylock};
        function(cookie_iterator{mycookies});
    }

    cookie_type const* cookie_from_pointer(void const* pointer) const noexcept {
        // retrun cookie from any pointer, if it points to a location inside any allocation.
        // returns 0 if it was not found.
        //
        // this is a slow linear search. use cookie_from_pointer_from_allocate() if pointer is
        // what allocate() returned
        //
        lock_guard lock{mylock};
        byte const *p = static_cast<byte const*>(pointer);
        if(mystatistics_total.address_lowest <= p && p <= mystatistics_total.address_highest) {
            if(auto c = mycookies) {
                do {
                    if(c->pointer <= p && p < c->pointer + c->bytes)
                        return c;
                    c = c->next;
                } while(c != mycookies);
            }
        }
        return 0;
    }

    cookie_type const* cookie_from_pointer_from_allocate(void const* pointer, size_t align = 0) const noexcept {
        // return cookie from pointer, 0 if error
        // make sure the pointer is what allocate() returned. otherwise use cookie_from_pointer()
        // and align must be the same as used for allocate()
        //
        // this uses some error checking of the pointer, but should be very quick
        auto unaligned = static_cast<byte const*>(align_differently(align) ? unalign(pointer, align) : pointer);
        lock_guard lock{mylock};
        if(
            unaligned &&
            !(upointer(unaligned) % align_allocations) &&
            mystatistics_total.count_now &&
            mystatistics_total.address_lowest <= pointer &&
            pointer <= mystatistics_total.address_highest
        ) {
            auto c = static_cast<cookie_type const*>(static_cast<void const*>(unaligned - (overhead - pattern_size)));
            if(c->pointer == pointer)
                return c;
        }
        return 0;
    }
    
    bool statistics_reset() noexcept {
        // reset statisitcs
        lock_guard lock{mylock};
        mystatistics_by_bytes.reset();
        mystatistics_by_name.reset();
        mystatistics_current = {};
        mystatistics_allocations = myallocations;
        return true;
    }

    template<typename function_>
    size_t statistics_resize(function_&& sizer, size_t bytes_max = 0) noexcept {
        // resize statisitcs_by_bytes, resets statistics
        // sizer must not use this to allocate memory, because it will deadlock
        lock_guard lock{mylock};
        size_t r = mystatistics_by_bytes.resize(sizer, bytes_max);
        if(r) {
            mystatistics_by_name.reset();
            mystatistics_current = {};
            mystatistics_allocations = myallocations;
        }
        return r;
    }
    
    template<typename function_>
    void statistics_by_bytes_to(function_&& function) const {
        // function must not use this to allocate memory, because it will deadlock
        lock_guard lock{mylock};
        function(mystatistics_by_bytes.begin(), mystatistics_by_bytes.end());
    }
    
    template<typename function_>
    void statistics_by_name_to(function_&& function) const {
        // function must not use this to allocate memory, because it will deadlock
        lock_guard lock{mylock};
        function(mystatistics_by_name.begin(), mystatistics_by_name.end());
    }

    memory_track::statistics_for_bytes statistics_for_bytes(size_t a) const noexcept {
        lock_guard lock{mylock};
        return mystatistics_by_bytes.find_and_copy(a);
    }

    template<typename name_>
    memory_track::statistics_for_name statistics_for_name(name_ const& a) const noexcept {
        lock_guard lock{mylock};
        return mystatistics_by_name.find_and_copy(mynames.find(a));
    }

    statistics statistics_current() const noexcept {
        lock_guard lock{mylock};
        return mystatistics_current;
    }

    statistics statistics_total() const noexcept {
        lock_guard lock{mylock};
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
        cookie_error r;
        r.cookie = c;
        auto b = static_cast<byte const*>(static_cast<void const*>(c));
        auto unaligned_pointer = b + (overhead - pattern_size);
        size_t align_or_0 = align_differently(c->align) ? c->align : 0;
        if(upointer(c) % alignof(cookie_type)) {
            r.where = c;
            r.error = "cookie not aligned";
        }
        else if(!align_or_0 && c->pointer != unaligned_pointer) {
            r.where = &c->pointer;
            r.error = "cookie pointer";
        }
        else if(align_or_0 && !(unaligned_pointer < c->pointer && c->pointer <= unaligned_pointer + align_or_0)) {
            r.where = &c->pointer;
            r.error = "cookie pointer";
        }
        else if(
            (upointer(c->next) % alignof(cookie_type)) ||
            c->next < mycookie_address_min ||
            c->next > mycookie_address_max
        ) {
            r.where = &c->next;
            r.error = "cookie next pointer";
        }
        else if(
            (upointer(c->prev) % alignof(cookie_type)) ||
            c->prev < mycookie_address_min ||
            c->prev > mycookie_address_max
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
        else if((r.where = overrun(c)) != 0)
            r.error = "overrun";
        else if((r.where = underrun(c)) != 0)
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
        lock_guard lock{mylock};
        if(auto c = mycookies) {
            do {
                if(cookie_error r = cookie_error_from(c))
                    return r;
                c = c->next;
            } while(c != mycookies);
        }
        return {};
    }
    
    template<typename name_>
    void* allocate(size_t bytes, size_t align, name_ const& name, tag_type const& tag = {}, allocator_tag allocator = {}) noexcept {
        // bytes can be 0
        ___water_assert(bytes < static_cast<size_t>(-1) / 2 && "size_t overflow"); // this has to be an error
        byte *r = 0;
        cookie_type *c = 0;
        {
            lock_guard lock{mylock};
            #ifdef WATER_USE_WATER_THREADS
            ___water_threads_statistics(
                if(!myallocations)
                    name_if(statistics_pointer(mylock)) << "water::memory_track::memory";
            )
            #endif
            if(!mystatistics_by_bytes && !mystatistics_by_bytes.resize())
                return 0;
            memory_track::statistics_for_bytes *b = mystatistics_by_bytes.find(bytes);
            auto found_name = mynames.find_or_add(name);
            if(!found_name)
                return 0;
            auto n = mystatistics_by_name.find_or_add(found_name);
            if(!n)
                return 0;
            byte *a = 0;
            size_t align_or_0 = 0;
            if(align_differently(align))
                align_or_0 = align;
            if(
                bytes <= static_cast<size_t>(-1) - align_or_0 &&
                bytes + align_or_0 < static_cast<size_t>(-1) / 2 // also means bytes + overhead wont overflow
            )
                a = static_cast<byte*>(underlying_allocator_nothrow{}.allocate(bytes + align_or_0 + overhead));
            if(a) {
                pattern_set(a + sizeof(cookie_type), bytes + align_or_0 + overhead - sizeof(cookie_type));
                add(myallocations);
                r = a + (overhead - pattern_size);
                if(align_or_0)
                    r = static_cast<byte*>(align_with_unalign(r, align_or_0));
                c = new(here(a)) cookie_type;
                c->id = myallocations;
                c->pointer = r;
                c->bytes = bytes;
                c->align = align;
                c->name = found_name;
                c->allocator_tag = allocator;
                c->tag_or_not(tag);
                if(!mycallback.allocate(static_cast<cookie_type const*>(c), mystatistics_total.bytes_now + bytes)) { // could overflow in theory
                    sub(myallocations);
                    c->~cookie_type();
                    underlying_allocator_nothrow{}.free(a, bytes + align_or_0 + overhead);
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
                if(mycookie_address_min > c)
                    mycookie_address_min = c;
                if(mycookie_address_max < c)
                    mycookie_address_max = c;
            }
            else {
                mycookies = c->next = c->prev = c;
                mycookie_address_max = mycookie_address_min = c;
            }
        }
        // assert outside lock
        ___water_assert(!(upointer(c) % align_allocations) && "underlying_allocator allocation not aligend to water::align_allocations");
        return r;
    }

    template<typename name_>
    void free(void *pointer, size_t bytes, size_t align, name_ const& name, tag_type const& tag = {}, allocator_tag allocator = {}) noexcept {
        // bytes = 0 is ok because delete
        // if name is empty, it does not need to match the name from allocate
        char const* error = 0;
        {
            cookie_type *c = 0;
            size_t align_or_0 = 0;
            if(align_differently(align))
                align_or_0 = align;
            lock_guard lock{mylock};
            if(!pointer)
                error = "pointer is 0";
            else if(upointer(pointer) % (align ? align : align_allocations))
                error = "not aligned to align";
            else if(!mycookies)
                error = "no allocations exist";
            else if(
                bytes > static_cast<size_t>(-1) - align_or_0 ||
                bytes + align_or_0 >= static_cast<size_t>(-1) / 2
            )
                error = "size_t overflow";
            else if(static_cast<byte*>(pointer) < mystatistics_total.address_lowest)
                error = "address lower than address_lowest";
            else if(static_cast<byte*>(pointer) > mystatistics_total.address_highest) // for 0 size pointer can be equal to highest address
                error = "address higher than address_highest";
            else {
                bool ignore_name = name_is_empty(name);
                auto found_name = mynames.find(name);
                if(!ignore_name && !found_name)
                    error = "name does not exist";
                else {
                    auto unaligned = align_or_0 ? unalign(pointer, align) : pointer;
                    if(!unaligned)
                        error = "buffer underrun or different align used for allocate and free";
                    else {
                        void *p = static_cast<byte*>(unaligned) - (overhead - pattern_size);
                        c = static_cast<cookie_type*>(p);
                        if(auto e = cookie_error_from(c))
                            error = e.error;
                        else if(bytes && c->bytes != bytes)
                            error = "cookie size missmatch";
                        else if(c->align != align)
                            error = "cookie align missmatch";
                        else if(c->pointer != pointer)
                            error = "cookie pointer missmatch"; // could happen for some combination of pointer address and align?
                        else if(!ignore_name && c->name != found_name)
                            error = "name missmatch";
                        else if(c->tag_or_not() != tag)
                            error = "tag missmatch";
                        else if(c->allocator_tag != allocator)
                            error = "allocator tag missmatch";
                        else {
                            c->next->prev = c->prev;
                            c->prev->next = c->next;
                            if(mycookies == c && (mycookies = c->next) == c) {
                                mycookies = 0;
                                mycookie_address_max = mycookie_address_min = 0;
                            }
                            if(c->id > mystatistics_allocations) {
                                if(auto b = mystatistics_by_bytes.find(c->bytes))
                                    b->free(c->bytes);
                                if(auto n = mystatistics_by_name.find(found_name))
                                    n->free(c->bytes);
                                mystatistics_current.free(c->bytes);
                            }
                            mystatistics_total.free(c->bytes);
                            mycallback.free(static_cast<cookie_type const*>(c));
                            size_t s = c->bytes + align_or_0 + overhead;
                            c->~cookie_type();
                            pattern_set(p, s); // if its used after free
                            underlying_allocator_nothrow{}.free(p, s);
                            return;
                        }
                    }
                }
            }
            if(!mycallback.free_error(static_cast<cookie_type const*>(c), pointer, bytes, align, name, tag, error))
                return;
        }
        // unlock before trace/assert, dont deadlock if they use this
        ___water_debug(trace << "water::memory_track free error pointer=" << pointer << " bytes=" << bytes << " align=" << align << " name=" << name << " error=" << error;)
        ___water_assert(!error);
    }
};



template<typename tag_, typename callback_>
allocator_throw<memory<tag_, callback_> > allocator_for(memory<tag_, callback_>& m, char const *name = 0, typename memory<tag_, callback_>::tag_type const& tag = {}) noexcept {
    return allocator_throw<memory<tag_, callback_> >{m, name, tag};
}

template<typename tag_, typename callback_>
allocator_nothrow<memory<tag_, callback_> > allocator_nothrow_for(memory<tag_, callback_>& m, char const *name = 0, typename memory<tag_, callback_>::tag_type const& tag = {}) noexcept {
    return allocator_nothrow<memory<tag_, callback_> >{m, name, tag};
}



template<typename type_, typename memory_ = memory<>>
using std_allocator = water::std_allocator<type_, allocator_throw<memory_>>;


}}
#endif
