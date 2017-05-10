// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_SPIN_SEMAPHORE_HPP
#define WATER_THREADS_SPIN_SEMAPHORE_HPP
#include <water/threads/spin.hpp>
namespace water { namespace threads {

// useless? but took 5 minutes to write

template<bool exists_ = spin_exists> class
 spin_semaphore {
	public:
		using needs = threads::needs<need_water, need_spin, need_constexpr_constructor, need_trivial_destructor>;
	private:
		atomic_uint
			my,
			myspin;
		___water_threads_statistics(threads::statistics::reference mystatistics;)
		___water_threads_statistics(using add_ = threads::statistics::add;)
	public:
		constexpr spin_semaphore(unsigned value = 0, unsigned spin_times = 0) noexcept :
			my(value),
			myspin(spin_times)
			{}
		spin_semaphore(spin_semaphore const&) = delete;
		spin_semaphore& operator=(spin_semaphore const&) = delete;
		explicit operator bool() const noexcept {
			return true;
			}
		void spin_times(unsigned a) noexcept {
			myspin.store(a);
			}
		unsigned spin_times() noexcept {
			unsigned r = static_cast<unsigned>(myspin.load(memory_order_relaxed));
			return r ? r : threads::spin_times();
			}
		bool down() noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "spin_semaphore"); add.wait(true));
			if(down_once()) return true;
			spin s(spin_times());
			do s(); while(!down_once());
			___water_threads_statistics(add.wait(s.spinning()));
			return true;
			}
		bool try_down() noexcept {
			bool r = down_once();
			___water_threads_statistics(add_(mystatistics, this, "spin_semaphore").wait(r));
			return r;
			}
		bool up(unsigned a = 1) noexcept {
			my.fetch_add(a);
			return true;
			}
		___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "spin_semaphore"); })
	private:
		bool down_once() noexcept {
			bool r;
			auto a = my.load(memory_order_relaxed);
			while(!my.compare_exchange_weak(a, (r = a != 0) ? a - 1 : a));
			return r;
			}
	};
template<> class
 spin_semaphore<false> {
	public:
		using needs = threads::needs<>;
 	};

}}
#endif
