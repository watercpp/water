// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_WINDOWS_READ_WRITE_COUNT_HPP
#define WATER_THREADS_WINDOWS_READ_WRITE_COUNT_HPP
#include <water/threads/pause.hpp>
#include <water/threads/windows/deadline.hpp>
#include <water/threads/algorithms/read_write_count.hpp>
namespace water { namespace threads {

class read_write_count {
	using algorithm = algorithms::read_write_count<atomic::uint_t>;
	public:
		using needs = threads::needs<need_water, need_constexpr_constructor, need_timeout>;		
	private:
		atomic::uint_t my = 0;
		atomic::alias<void*> mywait_all {}; // semaphore
		atomic::alias<void*> mywait_write {}; // event
		___water_threads_statistics(threads::statistics::reference mystatistics;)
		___water_threads_statistics(using add_ = threads::statistics::add;)
	public:
		constexpr read_write_count() noexcept = default;
		read_write_count(read_write_count const&) = delete;
		read_write_count& operator=(read_write_count const&) = delete;
		~read_write_count() {
			if(void *h = atomic::get<atomic::acquire>(mywait_all)) CloseHandle(h);
			if(void *h = atomic::get<atomic::none>(mywait_write)) CloseHandle(h);
			}
		void lock() noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "read_write_count"); add.wait(true);)
			algorithm a(my);
			if(a.write_lock(true)) return;
			___water_threads_statistics(add.wait(false));
			pause p = pause_wait();
			void *s = atomic::get<atomic::none>(mywait_all);
			void *e = atomic::get<atomic::none>(mywait_write);
			bool once = false; // have to atomic lock once after the semaphore is created, only wait after that
			do {
				bool wait = once;
				if(!s || s == handle_bad)
					s = atomic_create(mywait_all, []{ return semaphore_create(); });
				if(!e || e == handle_bad)
					e = atomic_create(mywait_write, []{ return CreateEventW(0, 0, 0, 0); });
				if(s && e) {
					once = true;
					___water_threads_statistics(add.wait_object_created());
					}
				else
					p();
				if(wait)
					handle_wait_any(s, e);
				} while(!a.write_lock(false));
			}
		bool lock(deadline d) noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "read_write_count"); add.wait(true).timeout(true);)
			algorithm a(my);
			if(a.write_lock(true)) return true;
			___water_threads_statistics(add.wait(false).timeout(false));
			bool locked = false;
			if(!d.passed()) {
				void *s = atomic::get<atomic::none>(mywait_all);
				if(!s || s == handle_bad)
					s = atomic_create(mywait_all, []{ return semaphore_create(); });
				void *e = atomic::get<atomic::none>(mywait_write);
				if(!e || e == handle_bad)
					e = atomic_create(mywait_write, []{ return CreateEventW(0, 0, 0, 0); });
				if(s && e) {
					___water_threads_statistics(add.wait_object_created());
					dword_t left;
					while(
						!(locked = a.write_lock(false)) &&
						(left = d.milli_left()) &&
						!handle_wait_any(s, e, left)
						);
					}
				}
			if(!locked)
				if(auto n = a.write_timeout(locked))
					wake(n);
			___water_threads_statistics(add.timeout(locked));
			return locked;
			}
		bool try_lock() noexcept {
			bool r = algorithm(my).write_try_lock();
			___water_threads_statistics(add_(mystatistics, this, "read_write_count").wait(r));
			return r;
			}
		void unlock() noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "read_write_count"); add.wake(true));
			if(auto n = algorithm(my).write_unlock()) {
				wake(n);
				___water_threads_statistics(add.wake(false));
				}
			}
		void read_lock() noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "read_write_count"); add.wait(true);)
			algorithm a(my);
			if(a.read_lock(true)) return;
			___water_threads_statistics(add.wait(false));
			pause p = pause_wait();
			void *s = atomic::get<atomic::none>(mywait_all);
			bool once = false; // have to atomic lock once after the semaphore is created, only wait after that
			do {
				bool wait = once;
				if(!s || s == handle_bad) {
					s = atomic_create(mywait_all, []{ return semaphore_create(); });
					once = s != 0;
					___water_threads_statistics(add.wait_object_created(s));
					}
				if(!s)
					p();
				else if(wait)
					handle_wait(s);
				} while(!a.read_lock(false));
			}
		bool read_lock(deadline d) noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "read_write_count"); add.wait(true).timeout(true);)
			algorithm a(my);
			if(a.read_lock(true)) return true;
			___water_threads_statistics(add.wait(false).timeout(false));
			bool locked = false;
			if(!d.passed()) {
				void *s = atomic::get<atomic::none>(mywait_all);
				if(!s || s == handle_bad)
					s = atomic_create(mywait_all, []{ return semaphore_create(); });
				if(s) {
					___water_threads_statistics(add.wait_object_created());
					dword_t left;
					while(
						!(locked = a.read_lock(false)) &&
						(left = d.milli_left()) &&
						!handle_wait(s, left)
						);
					}
				}
			if(!locked) a.read_timeout();
			___water_threads_statistics(add.timeout(locked));
			return locked;
			}
		bool read_try_lock() noexcept {
			bool r = algorithm(my).read_try_lock();
			___water_threads_statistics(add_(mystatistics, this, "read_write_count").wait(r));
			return r;
			}
		void read_unlock() noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "read_write_count"); add.wake(true));
			if(algorithm(my).read_unlock()) {
				wake_write();
				___water_threads_statistics(add.wake(false));
				}
			}
		___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "read_write_count"); })
	private:
		void wake(atomic::uint_t n) {
			void *s = atomic::get<atomic::acquire>(mywait_all);
			if(s && s != handle_bad)
				ReleaseSemaphore(s, static_cast<long_t>(n), 0);
			}
		void wake_write() {
			void *e = atomic::get<atomic::acquire>(mywait_write);
			if(e && e != handle_bad)
				SetEvent(e);
			}
	};

}}
#endif
