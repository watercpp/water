// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_STATISTICS_REFERENCE_HPP
#define WATER_THREADS_STATISTICS_REFERENCE_HPP
#include <water/threads/statistics/data.hpp>
namespace water { namespace threads { namespace statistics {

class reference {
	public:
		using atomic_tag = atomic::atomic_tag;
		using value_type = data*;
		using atomic_type = types::if_not_void<atomic::alias_type_from<value_type>, value_type>::result;
	private:
		atomic_type my = 0;
	public:
		constexpr reference() noexcept = default;
		reference(reference const&) = delete;
		reference& operator=(reference const&) = delete;
		atomic_type& atomic() noexcept {
			return my;
			}
	};

}}}
#endif
