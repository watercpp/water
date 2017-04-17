// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_ALGORITHMS_READ_WRITE_BIT_HPP
#define WATER_THREADS_ALGORITHMS_READ_WRITE_BIT_HPP
#include <water/threads/algorithms/bits.hpp>
namespace water { namespace threads { namespace algorithms {

/*

this could be useful if the unsigned_ type used as the lock state is very small.
but it never is?

the problem with this algorithm is that write_unlock is likley to return ture,
meaning wake all waiting threads, even if no threads are waiting.

read_write_count avoids this problem, but uses half of the unsigned_ as wait-count

in the unsigned_
- 1 bit is wait-bit, if set threads are maybe waiting
- all other bits is lock-count

the lock state is
- free
  - lock-count == 0
  - wait-bit == 0
- read-locked
  - 0 < lock-count < max
  - if wait-bit == 1, writers wait (and maybe readers too)
- write-locked
  - lock-count == max
  - if wait-bit == 1, someone waits
- free for write only
  - lock-count == 0
  - wait-bit == 1

only write_unlock clears wait-bit (and sometimes write_timeout if it does what write_unlock does)

if unsigned_ is 16 bits, there can be at most 32766 read locks

this does not check for overflows

*/

template<typename unsigned_> class
 read_write_bit {
	static constexpr unsigned_
		lock = static_cast<unsigned_>(-1) >> 1,
		wait = lock + 1;
	unsigned_ *my;
	public:
		read_write_bit(unsigned_& a) noexcept :
			my{&a}
			{}
		bool read_lock(bool set_wait) noexcept {
			// returns true if acquired
			//
			// post
			// - if set_wait == true, and return == false, wait-bit is set
			//
			// is trylock if set_wait == false
			//
			// lock loop:
			//
			//   read_write_bit rw(lock_atomic);
			//   while(!rw.read_lock(false)) {
			//     mutex.lock();
			//     if(rw.read_lock(true)) {
			//       mutex.unlock();
			//       break;
			//       }
			//     condition.wait_unlock(mutex);
			//     }
			//
			bool r;
			unsigned_ l0 = *my, l;
			do {
				l = l0;
				if(r = l < lock)
					++l;
				else if(set_wait)
					l |= wait;
				} while(!atomic::compare_set_else_non_atomic_get<atomic::acquire>(*my, l0, l, l0));
			return r;
			}
		bool read_unlock() noexcept {
			// returns true if this thread should wake a writer
			return atomic::subtract1_get<atomic::release>(*my) == wait;
			}
		bool write_lock(bool set_wait) noexcept {
			// returns true if acquired
			//
			// post
			// - if wait_ == true, and return == false, wait-bit is set
			//
			// is trylock if wait_ == false
			//
			// lock loop:
			//
			//   read_write_bit rw(lock_atomic);
			//   while(!rw.write_lock(false)) {
			//     mutex.lock();
			//     if(rw.write_lock(true)) {
			//       mutex.unlock();
			//       break;
			//       }
			//     condition.wait_unlock(mutex);
			//     }
			//
			// if the lock was not aquired at the write_lock(true) above, the
			// wait-bit is set
			// - only write_unlock clears wait-bit
			// - it is very likley that write_unlock() for this thread returns
			//   true (means wake everyone) even if no one is waiting
			//
			// if a timeout is used, use write_timeout if it times out
			//
			bool r;
			unsigned_ l0 = *my, l;
			do {
				l = l0;
				if(r = (l & lock) == 0)
					l |= lock;
				else if(set_wait)
					l |= wait;
				} while(!atomic::compare_set_else_non_atomic_get<atomic::acquire>(*my, l0, l, l0));
			return r;
			}
		bool write_timeout(bool& acquired) noexcept {
			// call after write-lock has timed out
			//
			// returns true if this thread should wake everyone
			//
			// post
			// - acquired is true if lock was acquired (return was false)
			//
			bool wake_all = false;
			unsigned_ l0 = *my, l;
			do {
				l = l0 & lock;
				if(acquired = l == 0) // it is avalible
					l = l0 | lock;
				else if(
					l != lock && // no one else has write locked and
					(l0 & wait) // someone waits (possibly only this thread)
					)
						wake_all = true; // l is l0 without wait-bit
				else
					l = l0; // do nothing
				} while(!atomic::compare_set_else_non_atomic_get<atomic::full>(*my, l0, l, l0));
			return wake_all;
			}
		bool write_unlock() noexcept {
			// returns true if this thread should wake everyone
			//
			// post
			// - lock == 0
			//
			return atomic::get_set<atomic::release>(*my, 0) == static_cast<unsigned_>(-1);
			}
	};

}}}
#endif
