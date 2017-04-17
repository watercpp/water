// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MEMORY_TRACK_ALLOCATOR_HPP
#define WATER_MEMORY_TRACK_ALLOCATOR_HPP
#include <water/memory_track/bits.hpp>
namespace water { namespace memory_track {

/*

Allocates from memory_track::memory
Keeps track of the allocation function and optionally an "address", free must be made with the matching function + "address"

The address can be useful for testing when something should use the same allocator-object for allocate + free

*/

template<typename memory_, typename exception_ = void> class
 allocator {
	public:
		using memory_type = memory_;
		using tag_type = typename memory_type::tag_type;
		using exception = exception_;
		static bool constexpr is_noexcept = types::equal_plain<void, exception>::result;
	private:
		memory_type *mymemory = 0;
		tag_type mytag {};
		void const* myaddress = 0; // keep this 0
	public:
		allocator() = default;
		allocator(memory_type& memory, tag_type const& tag = {}) :
			mymemory(&memory),
			mytag(tag)
			{}
		tag_type const& tag() const noexcept {
			return mytag;
			}
		allocator& tag(tag_type const& a) noexcept {
			mytag = a;
			return *this;
			}
		void const* address() const noexcept {
			return myaddress;
			}
		allocator& address(void const* a) noexcept {
			// when freeing, this address must match what it was when memory was allocated. default is 0.
			myaddress = a;
			return *this;
			}
		void* allocate(size_t bytes) noexcept(is_noexcept) {
 			return allocate_do(bytes, tag_function<void>);
 			}
 		void free(void *pointer, size_t bytes) noexcept {
 			free_do(pointer, bytes, tag_function<void>);
 			}
 		template<typename type_>
		 type_* allocate() noexcept(is_noexcept) {
			return static_cast<type_*>(allocate_do(sizeof(type_), tag_function<type_>));
			}
		template<typename type_>
		 void free(void *pointer) noexcept {
			free_do(pointer, sizeof(type_), tag_function<type_>);
			}
 		template<typename type_>
		 type_* allocate(size_t count) noexcept(is_noexcept) {
			return static_cast<type_*>(allocate_do(sizeof(type_) * count, tag_function<type_[1]>));
			}
		template<typename type_>
		 void free(void *pointer, size_t count) noexcept {
			free_do(pointer, sizeof(type_) * count, tag_function<type_[1]>);
			}
	private:
		void* allocate_do(size_t bytes, void (*tag)()) noexcept(is_noexcept) {
			auto pointer = mymemory->allocate(bytes, mytag, allocator_tag(myaddress, tag));
			if(!pointer) throw_if<exception>();
			return pointer;
			}
		void free_do(void *pointer, size_t bytes, void (*tag)()) noexcept {
			mymemory->free(pointer, bytes, mytag, allocator_tag(myaddress, tag));
			}
		template<typename type_>
		 static void tag_function()
		 	{}
	};

template<typename memory_> using allocator_throw = allocator<memory_, exception>;
template<typename memory_> using allocator_nothrow = allocator<memory_>;

}}
#endif
