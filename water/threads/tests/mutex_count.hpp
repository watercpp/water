// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_MUTEX_COUNT_HPP
#define WATER_THREADS_TESTS_MUTEX_COUNT_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/mutex.hpp>
#include <water/threads/sleep.hpp>
#include <water/threads/yield.hpp>
namespace water { namespace threads { namespace tests {

template<typename mutex_> class
 mutex_count {
	mutex_ my;
	unsigned
		myat = 0,
		myend = 1000;
	public:
		mutex_count() {
			___water_threads_statistics(name_if(statistics_pointer(my)) << "water::threads::tests::mutex_count");
			run_many_reference(*this, 10);
			}
		void operator()() {
			unsigned x = static_cast<unsigned>(-1);
			auto x_from = thread();
			unsigned char
				*xt = static_cast<unsigned char*>(static_cast<void*>(&x)),
				*xf = static_cast<unsigned char*>(static_cast<void*>(&x_from));
			unsigned xi = 0;
			do xt[xi % sizeof(x)] ^= xf[xi]; while(++xi != sizeof(x_from));
			x |= 1u << (numeric_limits<unsigned>::digits - 1); // means hibit will be set by xor below
			unsigned at = 0;
			bool flip = false;
			while(at != myend) {
				auto l = lock_move(my);
				___water_test(myat <= myend);
				___water_test(at <= myat);
				if(myat != myend) ++myat;
				at = myat;
				myat ^= x;
				if(flip = !flip)
					sleep(0.001);
				else
					yield();
				myat ^= x;
				___water_test(myat == at);
				}
			}
	};


}}}
#endif
