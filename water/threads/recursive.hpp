// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_RECURSIVE_HPP
#define WATER_THREADS_RECURSIVE_HPP
#include <water/threads/thread.hpp>
namespace water { namespace threads {

// make any mutex or the write lock of read_write lock recursive
// this needs thread_t 0 to be "no thread"

bool constexpr recursive_exists =
	#ifdef WATER_THREADS_NO_RECURSIVE
	false;
	#else
	(types::is_int<thread_t>::result || types::is_pointer<thread_t>::result) &&
	atomic_exists;
	#endif

template<typename mutex_, bool exists_ = recursive_exists> class
 recursive : public mutex_ {
 	public:
		using needs = threads::needs<
			need_water,
			need_recursive,
			typename types::ifel<has_timeout<mutex_>(), need_timeout, need_nothing>::result,
			typename types::ifel<is_spin<mutex_>(), need_spin, need_nothing>::result,
			typename types::ifel<has_constexpr_constructor<mutex_>(), need_constexpr_constructor, need_nothing>::result,
			typename types::ifel<has_trivial_destructor<mutex_>(), need_trivial_destructor, need_nothing>::result
			>;
	private:
		atomic<thread_t> mythread {};
		size_t myrecurse = 0;
	public:
		void lock() {
			thread_t t = thread();
			if(recurse(t))
				return;
			mutex_::lock();
			locked(t);
			}
		template<typename timeout_>
		 auto lock(timeout_ timeout) -> decltype(types::make<mutex_&>().lock(timeout)) {
		 	thread_t t = thread();
			if(recurse(t))
				return true;
			if(!mutex_::lock(timeout))
				return false;
			locked(t);
			return true;
		 	}
		bool try_lock() {
			thread_t t = thread();
			if(recurse(t))
				return true;
			if(!mutex_::try_lock())
				return false;
			locked(t);
			return true;
			}
		void unlock() {
			if(--myrecurse)
				return;
			mythread.store(thread_t{}, memory_order_relaxed);
			mutex_::unlock();
			}
	private:
		bool recurse(thread_t t) noexcept {
			if(equal(mythread.load(memory_order_relaxed), t)) {
				++myrecurse;
				return true;
				}
			return false;
			}
		void locked(thread_t t) noexcept {
			mythread.store(t, memory_order_relaxed);
			myrecurse = 1;
			}
	};
template<typename mutex_> class
 recursive<mutex_, false> {
 	public:
		using needs = threads::needs<>;
 	};

}}
#endif
