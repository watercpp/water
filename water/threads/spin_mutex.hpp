// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_SPIN_MUTEX_HPP
#define WATER_THREADS_SPIN_MUTEX_HPP
#include <water/threads/spin.hpp>
namespace water { namespace threads {

template<bool exists_ = spin_exists> class
 spin_mutex {
	public:
		using needs = threads::needs<need_water, need_spin, need_constexpr_constructor, need_trivial_destructor>;
	private:
		atomic::uint_t
			my = 0,
			myspin;
		___water_threads_statistics(threads::statistics::reference mystatistics;)
		___water_threads_statistics(using add_ = threads::statistics::add;)
	public:
		constexpr spin_mutex(unsigned spin_times = 0) noexcept :
			myspin(spin_times)
			{}
		spin_mutex(spin_mutex const&) = delete;
		spin_mutex& operator=(spin_mutex const&) = delete;
		void spin_times(unsigned a) noexcept {
			atomic::set(myspin, a);
			}
		unsigned spin_times() noexcept {
			unsigned r = static_cast<unsigned>(atomic::get<atomic::none>(myspin));
			return r ? r : threads::spin_times();
			}
		void lock() noexcept {
			___water_threads_statistics(add_ add(mystatistics, this, "spin_mutex"); add.wait(true));
			if(lock_once()) return;
			spin s(spin_times());
			do s(); while(!lock_once());
			___water_threads_statistics(add.wait(s.spinning()));
			}
		bool try_lock() noexcept {
			bool r = lock_once();
			___water_threads_statistics(add_(mystatistics, this, "spin_mutex").wait(r));
			return r;
			}
		void unlock() noexcept {
			atomic::set<atomic::release>(my, 0);
			}
		___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "spin_mutex"); })
	private:
		bool lock_once() noexcept {
			return atomic::get_set<atomic::acquire>(my, 1) == 0;
			}
	};
template<> class
 spin_mutex<false> {
	public:
		using needs = threads::needs<>;
 	};

}}
#endif
