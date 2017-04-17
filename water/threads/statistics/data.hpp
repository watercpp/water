// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_STATISTICS_DATA_HPP
#define WATER_THREADS_STATISTICS_DATA_HPP
#include <water/threads/statistics/name.hpp>
#include <water/ministr/out.hpp>
namespace water { namespace threads { namespace statistics {

/*

each statistic counter has a good (true) or bad (false) number

the good is incremented when something completed using the most desirable/fast path.
the path that did not block, succeeded, did not have to make system calls.
a lock was good if it succeeded at once, bad if the thread needed to wait.

wait
if mutex.lock() semaphore.down() had to wait or not
if barrier() had to wait for other threads to exit or not
- good = no wait
- bad = yes had to wait

wake
if mutex.unlock semaphore.up had to wake threads or not
- good = no threads to wake
- bad = had to wake threads

timeout
if wait completed before timeout or not.
- good no timeout
- bad timeout

wait_object_created
just a bool, if the system wait object has been created

*/

class data {
	using atomic_ = atomic_if<>;
	count_t
		mywait[2] = {},
		mywake[2] = {},
		mytimeout[2] = {},
		mycreated = 0;
	void const* myaddress = 0;
	char const* myclass = 0;
	statistics::name myname;
	public:
		constexpr data() noexcept = default;
		data(void const* address, char const* class_name) :
			myaddress(address),
			myclass(class_name)
			{}
		void const* address() const noexcept {
			return myaddress;
			}
		char const* class_name() const noexcept {
			return myclass;
			}
		statistics::name& name() noexcept {
			return myname;
			}
		statistics::name const& name() const noexcept {
			return myname;
			}
		void name(char const* cstring) noexcept {
			myname.assign(cstring);
			}
		ministr::out<name_assign> name_ministr() noexcept {
			return ministr::out<name_assign>(myname);
			}
		void wait_add(bool good) noexcept {
			atomic_::add(mywait[good]);
			}
		size_t wait(bool good) noexcept {
			return atomic_::get(mywait[good]);
			}
		void wake_add(bool good) noexcept {
			atomic_::add(mywake[good]);
			}
		size_t wake(bool good) noexcept {
			return atomic_::get(mywake[good]);
			}
		void timeout_add(bool good) noexcept {
			atomic_::add(mytimeout[good]);
			}
		size_t timeout(bool good) noexcept {
			return atomic_::get(mytimeout[good]);
			}
		void wait_object_created_set() noexcept {
			atomic_::set(mycreated, 1);
			}
		bool wait_object_created() noexcept {
			return atomic_::get(mycreated) != 0;
			}
};

inline ministr::out<name_assign> name_if(data *a) noexcept {
	if(a) return ministr::out<name_assign>(a->name());
	return ministr::out<name_assign>();
	}

inline void name_if(data *a, char const* cstring) noexcept {
	if(a) a->name(cstring);
	}

}}}
#endif
