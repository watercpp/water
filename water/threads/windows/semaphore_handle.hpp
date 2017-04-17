// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_WINDOWS_SEMAPHORE_HANDLE_HPP
#define WATER_THREADS_WINDOWS_SEMAPHORE_HANDLE_HPP
#include <water/threads/windows/bits.hpp>
namespace water { namespace threads {

class semaphore_handle {
	public:
		using needs = threads::needs<need_system, need_constexpr_constructor, need_timeout>;
	private:
		void *my;
	public:
		semaphore_handle(unsigned a = 0) noexcept :
			my(semaphore_create(a))
			{
			___water_assert(my);
			}
		~semaphore_handle() noexcept {
			if(my) CloseHandle(my);
			}
		semaphore_handle(semaphore_handle const&) = delete;
		semaphore_handle& operator=(semaphore_handle const&) = delete;
		explicit operator bool() const noexcept {
			return my != 0;
			}
		bool down() noexcept {
			return wait_milli();
			}
		bool down(double seconds) noexcept {
			return wait_milli(milli_from_seconds(seconds));
			}
		bool down(deadline d) noexcept {
			return wait_milli(d.milli_left());
			}
		bool try_down() noexcept {
			return wait_milli(0);
			}
		bool up(unsigned n = 1) noexcept {
			return my && ReleaseSemaphore(my, static_cast<long_t>(n), 0);
			}
		void* underlying() noexcept {
			return my;
			}
	private:
		bool wait_milli(dword_t a = wait_forever) noexcept {
			return my && handle_wait(my, a) == 0;
			}
	};

}}
#endif
