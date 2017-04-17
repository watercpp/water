// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_APPLE_DEADLINE_MACH_HPP
#define WATER_THREADS_APPLE_DEADLINE_MACH_HPP
#include <water/threads/apple/mach.hpp>
namespace water { namespace threads {

class deadline_mach {
	mach_time_t my = 0;
	public:
		deadline_mach(double seconds) noexcept :
			my(mach_time_from_seconds(seconds))
			{
			if(my) my += mach_time();
			}
		deadline_mach(deadline_mach const&) = default;
		template<typename deadline_>
		 deadline_mach(deadline_ d, decltype(d.left()) = {}) :
		 	deadline_mach(d.left())
		 	{}
		bool passed() const noexcept {
			return my == 0;
			}
		double left() noexcept {
			if(my) {
				auto a = mach_time();
				if(my > a) return mach_seconds_from_time(my - a);
				my = 0;
				}
			return 0;
			}
		mach_time_t time() const noexcept {
			return my;
			}
	};

}}
#endif
