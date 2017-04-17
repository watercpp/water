// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_APPLE_CONDITION_SEMAPHORE_HPP
#define WATER_THREADS_APPLE_CONDITION_SEMAPHORE_HPP
#include <water/threads/apple/mutex_semaphore.hpp>
namespace water { namespace threads {

class
 condition_semaphore {
 	public:
 		using needs = threads::needs<need_water, need_constexpr_constructor, need_timeout>;
 		using mutex = mutex_semaphore;
 	private:
 		mach_t mylock = 0;
		mach_t mywait = 0;
	public:
		constexpr condition_semaphore() noexcept = default;
		condition_semaphore(condition_semaphore const&) = delete;
		condition_semaphore& operator=(condition_semaphore const&) = delete;
		~condition_semaphore() noexcept {
			if(atomic::get(mylock)) {
				semaphore_destroy(mylock);
				semaphore_destroy(mywait);
				}
			}
		template<typename mutex_>
		 bool wait(mutex_& l) {
			if(!mylock) {
				if(!create())
					return false;
				}
			else if(!semaphore_lock(mylock))
				return false;
			unlock(l);
			int e = semaphore_wait_signal(mywait, mylock);
			lock(l);
			return !e || e == mach_aborted;
			}
		template<typename mutex_>
		 bool wait(mutex_& l, deadline d) {
			if(!mylock) {
				if(!create())
					return false;
				}
			else {
				int e;
				while((e = semaphore_wait(mylock, d.left())) == mach_aborted);
				if(e) return false;
			}
			if(!d.left()) {
				semaphore_signal(mylock);
				return false;
				}
			unlock(l);
			int e = semaphore_wait_signal(mywait, mylock, d.left());
			lock(l);
			return e == 0;
			}
		bool wake() noexcept {
			auto l = atomic::get<atomic::none>(mylock);
			if(!l) return true;
			if(!semaphore_lock(l)) return false;
			semaphore_wake(mywait);
			semaphore_signal(l);
			return true;
			}
		bool wake_all() noexcept {
			auto l = atomic::get<atomic::none>(mylock);
			if(!l) return true;
			if(!semaphore_lock(l)) return false;
			bool r = semaphore_wake_all(mywait);
			semaphore_signal(l);
			return r;
			}
	private:
		bool create() noexcept {
			mach_t
				l = semaphore_create(),
				w = l ? semaphore_create() : 0;
			if(!w) {
				if(l) semaphore_destroy(l); 
				return false;
				}
			mywait = w;
			atomic::set(mylock, l);
			return true;
			}
	};

}}
#endif
