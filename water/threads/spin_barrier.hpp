// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_SPIN_BARRIER_HPP
#define WATER_THREADS_SPIN_BARRIER_HPP
#include <water/threads/spin.hpp>
namespace water { namespace threads {

/*

atomic
- bit 0 is leave_bit, set if threads should leave
- the other bits are thread count

*/

template<bool exists_ = spin_exists> class
 spin_barrier {
 	public:
 		using needs = threads::needs<need_water, need_spin, need_constexpr_constructor, need_trivial_destructor>;
 	private:
		atomic_uint
			my{0},
			mycount,
			myspin;
		___water_threads_statistics(threads::statistics::reference mystatistics;)
	public:
		constexpr spin_barrier(unsigned count, unsigned spin_times = 0) noexcept :
			mycount(count),
			myspin(spin_times)
			{}
		spin_barrier(spin_barrier const&) = delete;
		spin_barrier& operator=(spin_barrier const&) = delete;
		explicit operator bool() const noexcept {
			return true;
			}
		bool reset(unsigned count) noexcept {
			mycount.store(count);
			return true;
			}
		void spin_times(unsigned a) noexcept {
			myspin.store(a);
			}
		unsigned spin_times() noexcept {
			unsigned r = static_cast<unsigned>(myspin.load(memory_order_relaxed));
			return r ? r : threads::spin_times();
			}
		bool operator()() noexcept {
			___water_threads_statistics(threads::statistics::add add(mystatistics, this, "spin_barrier"); add.wait(true));
			if(mycount <= 1)
				return true;
			bool
				last_out = false,
				inside = false;
			spin s;
			auto now = my.load(memory_order_relaxed);
			while(true) {
				auto count = my >> 1;
				last_out = false;
				bool
					leave_bit = (my & 1) != 0,
					hold = leave_bit && !inside,
					leave = false;
				if(!hold) {
					if(!inside && ++count == mycount) {
						leave = leave_bit = true;
						--count; // never add
						}
					else if(inside && leave_bit) {
						leave = true;
						if(!--count) {
							last_out = true;
							leave_bit = false;
							}
						}
					}
				if(!my.compare_exchange_weak(now, (count << 1) | (leave_bit ? 1 : 0)))
					continue;
				if(leave)
					break;
				inside = !hold;
				if(!s.once())
					s.times(spin_times());
				___water_threads_statistics(add.wait(s.spinning()));
				s();
				now = my; // re-read after pause
				}
			return last_out;
			}
		___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "spin_barrier"); })
	};
template<> class
 spin_barrier<false> {
	public:
		using needs = threads::needs<>;
 	};

}}
#endif
