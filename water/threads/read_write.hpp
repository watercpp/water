// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_READ_WRITE_HPP
#define WATER_THREADS_READ_WRITE_HPP
#include <water/threads/deadline.hpp>
#include WATER_THREADS_INCLUDE(read_write.hpp)
namespace water { namespace threads {

template<typename ...needs_> using
 read_write = typename need_select<needs<needs_...>, read_write_list>::result;

#if 0

template<typename ...needs_> class
 read_write {
	public:
		read_write() = default;
		read_write(read_write const&) = delete;
		read_write& operator=(read_write const&) = delete;
		
		void lock();
		bool lock(double seconds);
		bool lock(deadline);
		bool try_lock();
		void unlock();

		void read_lock();
		bool read_lock(double seconds);
		bool read_lock(deadline);
		bool read_try_lock();
		void read_unlock();
	};

#endif

}}
#endif
