// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_WINDOWS_HOLD_SEMAPHORE_HPP
#define WATER_THREADS_WINDOWS_HOLD_SEMAPHORE_HPP
#include <water/threads/windows/bits.hpp>
#include <water/threads/windows/name.hpp>
namespace water { namespace threads {

class hold_semaphore {
	void *my = 0;
	public:
		using hold_type = handle_hold;
		constexpr hold_semaphore() noexcept = default;
		hold_semaphore(hold_semaphore const&) = delete;
		hold_semaphore& operator=(hold_semaphore const&) = delete;
		~hold_semaphore() noexcept {
			if(my) CloseHandle(my);
			}
		void get_or_create(hold_type& to) noexcept {
			to.set(semaphore_create(0));
			}
		void get(hold_type& to) noexcept {
			to.set(my);
			}
	};

class hold_semaphore_atomic {
	atomic::alias<void*> my {};
	public:
		using hold_type = handle_hold;
		constexpr hold_semaphore_atomic() noexcept = default;
		hold_semaphore_atomic(hold_semaphore_atomic const&) = delete;
		hold_semaphore_atomic& operator=(hold_semaphore_atomic const&) = delete;
		~hold_semaphore_atomic() noexcept {
			if(void *h = atomic::get<atomic::none>(my))
				CloseHandle(h);
			}
		void get_or_create(hold_type& to) noexcept {
			void *h = atomic::get<atomic::none>(my);
			if(!h || h == handle_bad)
				h = atomic_create(my, []{ return semaphore_create(); });
			to.set(h);
			}
		void get(hold_type& to) noexcept {
			void *h = atomic::get<atomic::none>(my);
			if(h == handle_bad) h = 0;
			to.set(h);
			}
	};

class hold_semaphore_named {
	public:
		using hold_type = handle_close;
		void get_or_create(hold_type& to) noexcept {
			to.set(semaphore_create(0, name("hold_semaphore_named", this)));
			}
		void get(hold_type& to) noexcept {
			to.set(OpenSemaphoreW(semaphore_modify_state, 0, name("hold_semaphore_named", this)));
			}
	};

}}
#endif
