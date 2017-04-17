// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_WINDOWS_DEADLINE_HPP
#define WATER_THREADS_WINDOWS_DEADLINE_HPP
#include <water/threads/windows/bits.hpp>
namespace water { namespace threads {

class deadline {
	ulonglong_t my = 0;
	public:
		//deadline() noexcept = false;
		deadline(double seconds) noexcept {
			if(auto m = milli_from_seconds(seconds))
				my = GetTickCount64() + m;
			}
		bool passed() const noexcept {
			return my == 0;
			}
		dword_t milli_left() noexcept {
			if(my) {
				auto a = GetTickCount64();
				if(my > a) return static_cast<dword_t>(my - a);
				my = 0;
				}
			return 0;
			}
		double left() noexcept {
			return milli_left() / 1e3;
			}
	};

}}
#endif
