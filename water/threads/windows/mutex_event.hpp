// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_WINDOWS_MUTEX_EVENT_HPP
#define WATER_THREADS_WINDOWS_MUTEX_EVENT_HPP
#include <water/threads/pause.hpp>
#include <water/threads/windows/deadline.hpp>
#include <water/threads/windows/name.hpp>
namespace water { namespace threads {

class mutex_event {
 	public:
 		using needs = threads::needs<need_water, need_timeout, need_constexpr_constructor>;
 	private:
		atomic_uint mylock{};
		handle_atomic myevent{}; // created open
		___water_threads_statistics(threads::statistics::reference mystatistics;)
		___water_threads_statistics(using add_ = threads::statistics::add;)
	public:
		constexpr mutex_event() noexcept = default;
		mutex_event(mutex_event const&) = delete;
		mutex_event& operator=(mutex_event const&) = delete;
		~mutex_event() noexcept {
			if(void *e = myevent.load(memory_order_relaxed))
				CloseHandle(e);
			}
		void lock() noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "mutex_event"); add.wait(true);)
			if(!mylock.fetch_add(1, memory_order_acquire))
				return;
			___water_threads_statistics(add.wait(false));
			pause p = pause_wait();
			void *e = myevent.load(memory_order_relaxed);
			do {
				if(!e || e == handle_bad) {
					e = create();
					___water_threads_statistics(add.wait_object_created(e));
					}
				if(!e)
					p();
				else
					handle_wait(e);
				} while(mylock.exchange(2, memory_order_acquire));
			}
		bool lock(deadline d) noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "mutex_event"); add.wait(true).timeout(true));
			if(!mylock.fetch_add(1, memory_order_acquire))
				return true;
			___water_threads_statistics(add.wait(false).timeout(false));
			if(d.passed())
				return false;
			void *e = myevent.load(memory_order_relaxed);
			if(!e || e == handle_bad) {
				e = create();
				if(!e) return false;
				}
			___water_threads_statistics(add.wait_object_created());
			bool loop = 0;
			decltype(mylock.load()) l = 1;
			do {
				dword_t m = d.milli_left();
				loop = m && !handle_wait(e, m);
				} while((l = mylock.exchange(2, memory_order_acquire)) != 0 && loop);
			___water_threads_statistics(add.timeout(l == 0));
			return l == 0;
			}
		bool try_lock() noexcept {
			decltype(mylock.load()) x;
			bool r = mylock.compare_exchange_strong(x = 0, 1, memory_order_acquire);
			___water_threads_statistics(add_(mystatistics, this, "mutex_event").wait(r));
			return r;
			}
		void unlock() noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "mutex_event"); add.wake(true));
			if(mylock.exchange(0, memory_order_release) > 1) {
				void *e = myevent.load(memory_order_relaxed);
				if(e && e != handle_bad) {
					SetEvent(e);
					___water_threads_statistics(add.wake(false));
					}
				}
			}
		___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "mutex_event"); })
	private:
		void* create() {
			return atomic_create(myevent, []{ return CreateEventW(0, 0, 1, 0); });
			}
	};

// same with a named event
class mutex_event_named {
	public:
		using needs = threads::needs<need_water, need_timeout, need_constexpr_constructor, need_trivial_destructor>;
	private:
		atomic_uint mylock{};
		___water_threads_statistics(threads::statistics::reference mystatistics;)
		___water_threads_statistics(using add_ = threads::statistics::add;)
	public:
		constexpr mutex_event_named() noexcept = default;
		mutex_event_named(mutex_event_named const&) = delete;
		mutex_event_named& operator=(mutex_event_named const&) = delete;
		void lock() noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "mutex_event_named"); add.wait(true);)
			if(!mylock.fetch_add(1, memory_order_acquire))
				return;
			___water_threads_statistics(add.wait(false));
			pause p = pause_wait();
			handle_close e(0);
			do {
				if(!e.get()) {
					e.set(CreateEventW(0, 0, 1, name("mutex_event_named", this)));
					___water_threads_statistics(add.wait_object_created(e.get()));
					}
				if(!e.get())
					p();
				else
					handle_wait(e.get());
				} while(mylock.exchange(2, memory_order_acquire));
			}
		bool lock(deadline d) noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "mutex_event_named"); add.wait(true).timeout(true));
			if(!mylock.fetch_add(1, memory_order_acquire))
				return true;
			___water_threads_statistics(add.wait(false).timeout(false));
			if(d.passed())
				return false;
			handle_close e(CreateEventW(0, 0, 1, name("mutex_event_named", this)));
			if(!e.get())
				return false;
			___water_threads_statistics(add.wait_object_created());
			bool loop = 0;
			decltype(mylock.load()) l = 1;
			do {
				dword_t m = d.milli_left();
				loop = m && !handle_wait(e.get(), m);
				} while((l = mylock.exchange(2, memory_order_acquire)) != 0 && loop);
			___water_threads_statistics(add.timeout(l == 0));
			return l == 0;
			}
		bool try_lock() noexcept {
			decltype(mylock.load()) x;
			bool r = mylock.compare_exchange_strong(x = 0, 1, memory_order_acquire);
			___water_threads_statistics(add_(mystatistics, this, "mutex_event_named").wait(r));
			return r;
			}
		void unlock() noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "mutex_event_named"); add.wake(true));
			if(mylock.exchange(0, memory_order_release) > 1) {
				handle_close e(OpenEventW(event_modify_state, 0, name("mutex_event_named", this)));
				if(e.get()) {
					SetEvent(e.get());
					___water_threads_statistics(add.wake(false));
					}
				}
			}
		___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "mutex_event_named"); })
	};

}}
#endif
