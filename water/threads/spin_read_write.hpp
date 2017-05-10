// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_SPIN_READ_WRITE_HPP
#define WATER_THREADS_SPIN_READ_WRITE_HPP
#include <water/threads/spin.hpp>
namespace water { namespace threads {

/*

bit 0: write waiting (no new readers)
bit 1: write lock (also always set bit 0)
bit 2+: read count

read can lock:
bit 0 and 1 are 0

read lock += 4
read unlock -= 4

write can lock
bit 1 and 2+ are 0, bit 0 does not matter

write lock += 2
write unlock = 0

*/

template<bool exists_ = spin_exists> class
 spin_read_write {
 	public:
 		using needs = threads::needs<need_water, need_spin, need_constexpr_constructor, need_trivial_destructor>;
 	private:
		atomic_uint
			my{0},
			myspin;
		___water_threads_statistics(threads::statistics::reference mystatistics;)
		___water_threads_statistics(using add_ = threads::statistics::add;)
	public:
		constexpr spin_read_write(unsigned spin_times = 0) noexcept :
			myspin(spin_times)
			{}
		spin_read_write(spin_read_write const&) = delete;
		spin_read_write& operator=(spin_read_write const&) = delete;
		void spin_times(unsigned a) noexcept {
			myspin.store(a);
			}
		unsigned spin_times() noexcept {
			unsigned r = static_cast<unsigned>(myspin.load(memory_order_relaxed));
			return r ? r : threads::spin_times();
			}
		void lock() noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "spin_read_write"); add.wait(true));
			if(write_once(true))
				return;
			spin s(spin_times());
			do s(); while(!write_once(true));
			___water_threads_statistics(add.wait(s.spinning()));
			}
		bool try_lock() noexcept {
			bool r = write_once(false);
			___water_threads_statistics(add_(mystatistics, this, "spin_read_write").wait(r));
			return r;
			}
		void unlock() noexcept {
			my.store(0, memory_order_release);
			}
		void read_lock() noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "spin_read_write"); add.wait(true));
			if(read_once()) return;
			spin s(spin_times());
			do s(); while(!read_once());
			___water_threads_statistics(add.wait(s.spinning()));
			}
		bool read_try_lock() noexcept {
			bool r = read_once();
			___water_threads_statistics(add_(mystatistics, this, "spin_read_write").wait(r));
			return r;
			}
		void read_unlock() noexcept {
			my.fetch_sub(4, memory_order_release);
			}
		___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "spin_read_write"); })
	private:
		bool read_once() noexcept {
			bool l;
			auto a = my.load(memory_order_relaxed);
			while(!my.compare_exchange_weak(a, (l = !(a & 1)) ? a + 4 : a));
			return l;
			}
		bool write_once(bool wait) noexcept {
			bool l;
			auto a = my.load(memory_order_relaxed);
			while(!my.compare_exchange_weak(a, (l = a <= 1) ? a | 3 : (wait ? a | 1 : a)));
			return l;
			}
	};
template<> class
 spin_read_write<false> {
	public:
		using needs = threads::needs<>;
 	};

}}
#endif
