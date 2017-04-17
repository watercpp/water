// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_TESTS_READ_WRITE_FUNCTIONS_HPP
#define WATER_THREADS_TESTS_READ_WRITE_FUNCTIONS_HPP
#include <water/threads/tests/bits.hpp>
#include <water/threads/read_write.hpp>
namespace water { namespace threads { namespace tests {

/*

test read_write functions

*/

template<typename read_write_> class
 read_write_functions {
	read_write_ my;
	public:
		read_write_functions() {
			___water_threads_statistics(name_if(statistics_pointer(my)) << "water::threads::tests::read_write_functions");
			bool locked = false;
				{
				auto l = lock_move(my);
				}
			spin_if(my, static_cast<typename types::ifel<is_spin<read_write_>(), bool, void>::result*>(0));
			my.lock();
			my.unlock();
			lock(my);
			unlock(my);
			___water_test(locked = try_lock(my));
			if(locked)
				auto l = unlock_move(my);
			___water_test(locked = my.try_lock());
			if(locked)
				my.unlock();
			
				{
				auto l = read_lock_move(my);
				}
			my.read_lock();
			my.read_unlock();
			read_lock(my);
			read_unlock(my);
			___water_test(locked = read_try_lock(my));
			if(locked)
				auto l = read_unlock_move(my);
			___water_test(locked = my.read_try_lock());
			if(locked)
				my.read_unlock();
			
			timeout_if(my, static_cast<typename types::ifel<has_timeout<read_write_>(), bool, void>::result*>(0));
			}
	private:
		template<typename m_>
		 void timeout_if(m_& my, bool*) {
		 	bool locked;
			___water_test(locked = lock(my, 0.01));
			if(locked)
				unlock(my);
			___water_test(locked = lock(my, deadline(0.01)));
			if(locked)
				my.unlock();
			___water_test(locked = read_lock(my, 0.01));
			if(locked)
				read_unlock(my);
			___water_test(locked = read_lock(my, deadline(0.01)));
			if(locked)
				my.read_unlock();
		 	}
		template<typename m_>
		 void timeout_if(m_&, void*) {
		 	}
		 	
		template<typename m_>
		 void spin_if(m_& my, bool*) {
		 	my.spin_times(1000);
		 	}
		template<typename m_>
		 void spin_if(m_& my, void*) {
		 	}
		
	};


}}}
#endif
