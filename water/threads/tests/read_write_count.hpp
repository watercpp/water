// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_READ_WRITE_COUNT_HPP
#define WATER_THREADS_TESTS_READ_WRITE_COUNT_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/read_write.hpp>
#include <water/threads/sleep.hpp>
#include <water/threads/yield.hpp>
namespace water { namespace threads { namespace tests {

template<typename read_write_> class
 read_write_count {
	read_write_ my;
	unsigned
		myid = 0,
		myat = 0,
		myend = 1000;
	public:
		read_write_count() {
			___water_threads_statistics(name_if(statistics_pointer(my)) << "water::threads::tests::read_write_count");
			run_many_reference(*this, 16);
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
			bool write = false;
				{
				auto l = lock_move(my);
				++myid;
				if(!(myid % 8))
					write = true;
				}
			unsigned at = 0;
			bool flip = false;
			while(at != myend) {
				if(write) {
					auto l = lock_move(my);
					___water_test(myat <= myend);
					___water_test(at <= myat);
					if(myat != myend) ++myat;
					at = myat;
					myat ^= x;
					flip = !flip;
					if(flip)
						sleep(0.001);
					else
						yield();
					myat ^= x;
					___water_test(myat == at);
					}
				else {
					auto l = read_lock_move(my);
					___water_test(myat <= myend);
					___water_test(at <= myat);
					at = myat;
					flip = !flip;
					if(flip)
						sleep(0.001);
					else
						yield();
					___water_test(myat == at);
					}
				}
			}
	};

}}}
#endif
