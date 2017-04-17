// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#include <water/memory_track/example/example.hpp>
#include <water/str/out_trace.hpp>
#include <water/later.hpp>
#include <new>
namespace water { namespace memory_track { namespace example {

memory_type global_memory_object;

memory_type& memory() {
	return global_memory_object;
	}

void* operator_new_nothrow(size_t size, memory_tag_type tag) {
	return memory().allocate(size, tag, {});
	}

void* operator_new(size_t size, memory_tag_type tag) {
	void *r = operator_new_nothrow(size, tag);
	if(!r) throw std::bad_alloc{};
	return r;
	}

void operator_delete(void *pointer, memory_tag_type tag) {
	memory().free(pointer, 0, tag, {});
	}

auto dump_later = later([]{
	str::out_trace o;
	o << "water::memory_track::example\n";
	dump_to(o);
	});

}}}

void* operator new  (std::size_t s)                                 { return water::memory_track::example::operator_new(s, "new"); }
void* operator new[](std::size_t s)                                 { return water::memory_track::example::operator_new(s, "new[]"); }
void* operator new  (std::size_t s, std::nothrow_t const&) noexcept { return water::memory_track::example::operator_new_nothrow(s, "new"); }
void* operator new[](std::size_t s, std::nothrow_t const&) noexcept { return water::memory_track::example::operator_new_nothrow(s, "new[]"); }

void  operator delete  (void* v) noexcept                        { water::memory_track::example::operator_delete(v, "new"); }
void  operator delete[](void* v) noexcept                        { water::memory_track::example::operator_delete(v, "new[]"); }
void  operator delete  (void* v, std::nothrow_t const&) noexcept { water::memory_track::example::operator_delete(v, "new"); }
void  operator delete[](void* v, std::nothrow_t const&) noexcept { water::memory_track::example::operator_delete(v, "new[]"); }
