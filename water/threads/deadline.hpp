// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_DEADLINE_HPP
#define WATER_THREADS_DEADLINE_HPP
#include <water/threads/configuration.hpp>
#include WATER_THREADS_INCLUDE(deadline.hpp)
namespace water { namespace threads {

#if 0
class deadline {
	public:
		deadline(double seconds) noexcept;
			// deadline this many seconds from now
		double left() noexcept;
			// returns how many seconds left
	};

#endif

}}
#endif
