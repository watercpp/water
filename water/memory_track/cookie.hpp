// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MEMORY_TRACK_COOKIE_HPP
#define WATER_MEMORY_TRACK_COOKIE_HPP
#include <water/memory_track/bits.hpp>
namespace water { namespace memory_track {

/*

linked list, added to all allocations

*/

template<typename tag_ = void> struct
 cookie {
	using tag_type = typename types::if_not_void<tag_, memory_track::tag>::result;
	cookie
		*next,
		*prev;
	large_t id; // unique and ordered, incremented for each allocation, except if it reaches max
	byte *pointer;
	size_t bytes;
	tag_type tag;
	allocator_tag allocator;
	size_t generation;
	void const* overrun() const noexcept {
		return pointer ? pattern_check(pointer + bytes, pattern_size) : 0;
		}
	void const* underrun() const noexcept {
		byte const *b = static_cast<byte const*>(static_cast<void const*>(this + 1));
		return pointer ? pattern_check(b, static_cast<size_t>(pointer - b)) : 0;
		}
	};

template<typename cookie_> class
 cookie_iterator {
	public:
		using value_type = cookie_;
		using pointer = value_type const*;
		using reference = value_type const&;
	private:
		cookie_ const *my = 0;
	public:
		cookie_iterator() noexcept = default;
		cookie_iterator(cookie_ const *a) noexcept :
			my(a)
			{}
		explicit operator bool() const noexcept {
			return my != 0;
			}
		pointer operator->() const noexcept {
			return my;
			}
		reference operator*() const noexcept {
			return *my;
			}
		cookie_iterator& operator++() noexcept {
			my = my->next;
			return *this;
			}
		cookie_iterator& operator--() noexcept {
			my = my->prev;
			return *this;
			}
		cookie_iterator operator++(int) noexcept {
			cookie_iterator r{*this};
			my = my->next;
			return r;
			}
		cookie_iterator operator--(int) noexcept {
			cookie_iterator r{*this};
			my = my->prev;
			return r;
			}
		bool operator==(cookie_iterator a) const noexcept {
			return my == a.my;
			}
		bool operator!=(cookie_iterator a) const noexcept {
			return my != a.my;
			}
	};


}}
#endif
