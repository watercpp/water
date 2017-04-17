// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_BARRIER_2_HPP
#define WATER_THREADS_TESTS_BARRIER_2_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/barrier.hpp>
#include <water/threads/mutex.hpp>
namespace water { namespace threads { namespace tests {

template<typename barrier_> class
 barrier_2 {
	barrier_ my;
	unsigned myat = 0;
	unsigned myloop;
	public:
		barrier_2(unsigned threads = 32, unsigned loop = 512) :
			my{threads},
			myloop{loop}
			{
			___water_test(my);
			___water_threads_statistics(name_if(statistics_pointer(my), "water::threads::tests::barrier_2"));
			run_many_reference(*this, threads);
			___water_test(myat == myloop);
			}
		void operator()() {
			unsigned at = 0;
			do {
				if(my()) {
					___water_test(at == myat);
					// mess up myat and sleep, should verify that only 1 thread does this ever
					if(!(at & 0xf)) {
						struct {thread_t a; deadline b;} x {thread(), deadline(0)};
						unsigned char
							*ab = static_cast<unsigned char*>(static_cast<void*>(&myat)),
							*xb = static_cast<unsigned char*>(static_cast<void*>(&x));
						unsigned i = 0;
						do ab[i % sizeof(myat)] ^= xb[i]; while(++i != sizeof(x));
						sleep(0.002);
						i = 0;
						do ab[i % sizeof(myat)] ^= xb[i]; while(++i != sizeof(x));
						}
					++myat;
					}
				} while(++at != myloop);
			}
	};

}}}
#endif
