// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_STATISTICS_OUT_HPP
#define WATER_THREADS_STATISTICS_OUT_HPP
#include <water/threads/statistics/statistics.hpp>
#include <water/str/str.hpp>
namespace water { namespace threads { namespace statistics {

inline double out_ratio(size_t good, size_t bad) {
	bad += good;
	if(bad) return static_cast<double>(good) / bad;
	return 0;
	}

template<typename o_> void
 out(str::out<o_>& o) {
	size_t i = 0;
	while(data *d = at(i)) {
		size_t
			wait[] = {d->wait(false), d->wait(true)},
			wake[] = {d->wake(false), d->wake(true)},
			timeout[] = {d->timeout(false), d->timeout(true)};
		o << "water::threads::statistics " << i << '\n'
			<< "  class ............. " << d->class_name() << '\n'
			<< "  name .............. " << d->name().begin() << '\n'
			<< "  address ........... " << d->address() << '\n'
			<< "  wait good ......... " << wait[true] << '\n'
			<< "  wait bad .......... " << wait[false] << '\n'
			<< "  wait good ratio ... " << out_ratio(wait[true], wait[false]) << '\n'
			<< "  wake good ......... " << wake[true] << '\n'
			<< "  wake bad .......... " << wake[false] << '\n'
			<< "  wait good ratio ... " << out_ratio(wake[true], wake[false]) << '\n'
			<< "  timeout true ...... " << timeout[true] << '\n'
			<< "  timeout false ..... " << timeout[false] << '\n'
			<< "  timeout good ratio  " << out_ratio(timeout[true], timeout[false]) << '\n'
			<< "  wait_object_created " << d->wait_object_created() << '\n' << str::flush;
		++i;
		}
	if(!i)
		o << "water::threads::statistics empty\n" << str::flush;
	}

}}}
#endif
