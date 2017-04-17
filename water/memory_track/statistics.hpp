// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_MEMORY_TRACK_STATISTICS_HPP
#define WATER_MEMORY_TRACK_STATISTICS_HPP
#include <water/memory_track/bits.hpp>
namespace water { namespace memory_track {

struct statistics {
	large_t
		success_count = 0,
		success_bytes = 0,
		failed_count = 0,
		failed_bytes = 0;
	size_t
		count_now = 0,
		count_peak = 0,
		bytes_now = 0,
		bytes_peak = 0,
		smallest_bytes = static_cast<size_t>(-1),
		largest_bytes = 0;
	byte const
		*address_lowest = 0,
		*address_highest = 0;
	void success(void const *pointer, size_t bytes) noexcept {
		add(success_count);
		add(success_bytes, bytes);
		add(count_now);
		add(bytes_now, bytes);
		if(count_peak < count_now) count_peak = count_now;
		if(bytes_peak < bytes_now) bytes_peak = bytes_now;
		if(smallest_bytes > bytes) smallest_bytes = bytes;
		if(largest_bytes < bytes) largest_bytes = bytes;
		auto a = static_cast<byte const*>(pointer);
		if(!address_lowest || address_lowest > a) address_lowest = a;
		a += bytes;
		if(address_highest < a) address_highest = a;
		}
	void failure(size_t bytes) noexcept {
		add(failed_count);
		add(failed_bytes, bytes);
		}
	void free(size_t bytes) noexcept {
		sub(count_now);
		sub(bytes_now, bytes);
		}
	bool empty() const noexcept {
		return !success_count && !failed_count;
		}
	};

}}
#endif
