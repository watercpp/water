// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_LINUX_READ_WRITE_COUNT_HPP
#define WATER_THREADS_LINUX_READ_WRITE_COUNT_HPP
#include <water/threads/linux/futex.hpp>
#include <water/threads/pause.hpp>
#include <water/threads/algorithms/read_write_count.hpp>
namespace water { namespace threads {

template<bool exists_ = futex_exists> class
 read_write_count {
	using algorithm = algorithms::read_write_count<futex_t>;
	public:
		using needs = threads::needs<need_water, need_constexpr_constructor, need_trivial_destructor, need_timeout>;
	private:
		futex_t my = 0;
		futex_t mywrite = 0; // writers wait on this
		___water_threads_statistics(threads::statistics::reference mystatistics;)
		___water_threads_statistics(using add_ = threads::statistics::add;)
	public:
		constexpr read_write_count() noexcept = default;
		read_write_count(read_write_count const&) = delete;
		read_write_count& operator=(read_write_count const&) = delete;
		void lock() noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "read_write_count"); add.wait(true);)
			algorithm a(my);
			auto v = mywrite;
			if(a.write_lock(true)) return;
			___water_threads_statistics(add.wait(false));
			pause p = pause_wait();
			do {
				int e = futex_wait(mywrite, v);
				if(e == futex_again)
					v = mywrite;
				else if(e && e != futex_signal)
					p();
				} while(!a.write_lock(false));
			}
		bool lock(deadline_clock<clockid::monotonic_maybe> d) noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "read_write_count"); add.wait(true).timeout(true));
			algorithm a(my);
			auto v = mywrite;
			if(a.write_lock(true)) return true;
			___water_threads_statistics(add.wait(false).timeout(false));
			bool locked = false;
			double left;
			while((left = d.left()) >= 1e-9) {
				int e = futex_wait(mywrite, v, left);
				if(e == futex_again)
					v = mywrite;
				else if(e && e != futex_signal)
					break;
				locked = a.write_lock(false);
				if(locked)
					break;
				}
			if(!locked)
				if(a.write_timeout(locked)) {
					write_wake();
					futex_wake_all(my);	
					}
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
				write_wake();
				futex_wake_all(my);
				___water_threads_statistics(add.wake(false));
				}
			}
		void read_lock() noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "read_write_count"); add.wait(true);)
			algorithm a(my);
			if(a.read_lock(true)) return;
			___water_threads_statistics(add.wait(false));
			pause p = pause_wait();
			do {
				int e = futex_wait(my, a.value());
				if(e && e != futex_again && e != futex_signal)
					p();
				} while(!a.read_lock(false));
			}
		bool read_lock(deadline_clock<clockid::monotonic_maybe> d) noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "read_write_count"); add.wait(true).timeout(true));
			algorithm a(my);
			if(a.read_lock(true)) return true;
			___water_threads_statistics(add.wait(false).timeout(false));
			bool locked = false;
			double left;
			while((left = d.left()) >= 1e-9) {
				int e = futex_wait(my, a.value(), left);
				if(e && e != futex_again && e != futex_signal)
					break;
				locked = a.read_lock(false);
				if(locked)
					break;
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
				write_wake();
				___water_threads_statistics(add.wake(false));
				}
			}
		___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "read_write_count"); })
	private:
		void write_wake() noexcept {
			auto a = mywrite;
			while(!atomic::compare_set_else_non_atomic_get(mywrite, a, (a + 1) & futex_max, a));
			futex_wake(mywrite);
		}
	};
template<> class
 read_write_count<false> {
	public:
		using needs = threads::needs<>;
 	};
 
}}
#endif
