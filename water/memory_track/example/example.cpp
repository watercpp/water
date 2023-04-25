// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#include <water/memory_track/example/example.hpp>
#include <water/later.hpp>
#include <water/str/out_trace.hpp>
#include <mutex>
#include <new>
namespace water { namespace memory_track { namespace example {

    // The memory_track::memory object will never be destroyed, to avoid crashes when memory is
    // deallocated during global object destruction.
    
    std::once_flag memory_once;
    char memory_buffer[sizeof(memory_type)];

    memory_type& memory() {
        call_once(memory_once, []{ new(memory_buffer) memory_type; });
        return *static_cast<memory_type*>(static_cast<void*>(memory_buffer));
    }
    
    
    // Write statisics to water::trace when the program ends
    
    auto trace_memory_statistics_later = later([]{
        str::out_trace to;
        to << "water::memory_track::example\n";
        out(to, memory());
        to << "allocations now\n";
        out_allocations(to, memory(), 2);
    });
    
    

    // Replace the global operator new and delete

    void* operator_new_nothrow(size_t size, char const* name, char const* allocator_tag, size_t align = 0) {
        return memory().allocate(size, align, name, {}, allocator_tag);
    }

    void* operator_new(size_t size, char const* name, char const* allocator_tag, size_t align = 0) {
        void *r = operator_new_nothrow(size, name, allocator_tag, align);
        if(!r) throw std::bad_alloc{};
        return r;
    }

    void operator_delete(void *pointer, size_t size, char const* name, char const* allocator_tag, size_t align = 0) {
        memory().free(pointer, size, align, name, {}, allocator_tag);
    }

}}}


// Notice how the delete operators do not use a name, only allocator_tag.
// The placement new operators in example.hpp will use another name, but the same allocator_tag.
// This makes the regular delete work for the special placement new operators, because the allocator_tag from new will match that from delete.

void* operator new  (std::size_t s)                                 { return water::memory_track::example::operator_new(s, "new", "new"); }
void* operator new[](std::size_t s)                                 { return water::memory_track::example::operator_new(s, "new[]", "new[]"); }
void* operator new  (std::size_t s, std::nothrow_t const&) noexcept { return water::memory_track::example::operator_new_nothrow(s, "new", "new"); }
void* operator new[](std::size_t s, std::nothrow_t const&) noexcept { return water::memory_track::example::operator_new_nothrow(s, "new[]", "new[]"); }

void  operator delete  (void* v) noexcept                        { water::memory_track::example::operator_delete(v, 0, 0, "new"); }
void  operator delete[](void* v) noexcept                        { water::memory_track::example::operator_delete(v, 0, 0, "new[]"); }
void  operator delete  (void* v, std::size_t s) noexcept         { water::memory_track::example::operator_delete(v, s, 0, "new"); }
void  operator delete[](void* v, std::size_t s) noexcept         { water::memory_track::example::operator_delete(v, s, 0, "new[]"); }
void  operator delete  (void* v, std::nothrow_t const&) noexcept { water::memory_track::example::operator_delete(v, 0, 0, "new"); }
void  operator delete[](void* v, std::nothrow_t const&) noexcept { water::memory_track::example::operator_delete(v, 0, 0, "new[]"); }

#ifdef __cpp_aligned_new

void* operator new  (std::size_t s, std::align_val_t a)                                 { return water::memory_track::example::operator_new(s, "new", "new", static_cast<std::size_t>(a)); }
void* operator new[](std::size_t s, std::align_val_t a)                                 { return water::memory_track::example::operator_new(s, "new[]", "new[]", static_cast<std::size_t>(a)); }
void* operator new  (std::size_t s, std::align_val_t a, std::nothrow_t const&) noexcept { return water::memory_track::example::operator_new_nothrow(s, "new", "new", static_cast<std::size_t>(a)); }
void* operator new[](std::size_t s, std::align_val_t a, std::nothrow_t const&) noexcept { return water::memory_track::example::operator_new_nothrow(s, "new[]", "new[]", static_cast<std::size_t>(a)); }

void  operator delete  (void* v, std::align_val_t a) noexcept                        { water::memory_track::example::operator_delete(v, 0, 0, "new", static_cast<std::size_t>(a)); }
void  operator delete[](void* v, std::align_val_t a) noexcept                        { water::memory_track::example::operator_delete(v, 0, 0, "new[]", static_cast<std::size_t>(a)); }
void  operator delete  (void* v, std::size_t s, std::align_val_t a) noexcept         { water::memory_track::example::operator_delete(v, s, 0, "new", static_cast<std::size_t>(a)); }
void  operator delete[](void* v, std::size_t s, std::align_val_t a) noexcept         { water::memory_track::example::operator_delete(v, s, 0, "new[]", static_cast<std::size_t>(a)); }
void  operator delete  (void* v, std::align_val_t a, std::nothrow_t const&) noexcept { water::memory_track::example::operator_delete(v, 0, 0, "new", static_cast<std::size_t>(a)); }
void  operator delete[](void* v, std::align_val_t a, std::nothrow_t const&) noexcept { water::memory_track::example::operator_delete(v, 0, 0, "new[]", static_cast<std::size_t>(a)); }

#endif
