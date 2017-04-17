// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_ALGORITHMS_READ_WRITE_COUNT_HPP
#define WATER_THREADS_ALGORITHMS_READ_WRITE_COUNT_HPP
#include <water/threads/algorithms/bits.hpp>
namespace water { namespace threads { namespace algorithms {

/*

in the unsigned_
- half is wait-count
- half is lock-state, where
  - 1 bit is write-bit
  - all other bits is lock-count

the lock state is
- free
  - lock-count == 0
  - wait-bit == 0
- read-locked
  - 0 < lock-count < max
  - if write-bit == 1, writers wait
- write-locked
  - lock-count == max
  - write-bit == 1
- free for write only
  - lock-count == 0
  - write-bit == 1

the wait-count is the number of waiting threads (readers + writers). only the thread that ++wait-count can --wait-count

if the unsigned_ type is small, algorithm::read_write_bit could be used instead of this algorithm

if unsigned_ is 32 bits, there can be at most
- 32766 read locks
- 65535 waiting threads

if unsigned_ is 16 bits, there can be at most
- 126 read locks
- 256 waiting threads

if unsigned_ is 21 bits, there can be at most
- 1022 read locks
- 1023 waiting threads

this does not check for overflows

*/

template<typename unsigned_> class
 read_write_count {
	static constexpr unsigned_
		shift = numeric_limits<unsigned_>::digits / 2 - (numeric_limits<unsigned_>::digits % 2 ? 1 : 0),
		lock = static_cast<unsigned_>(-1) >> shift,
		read = lock >> 1,
		write = read + 1,
		wait = static_cast<unsigned_>(-1) ^ lock,
		wait1 = lock + 1;
	unsigned_ *my;
	unsigned_ myvalue = 0; 
	public:
		read_write_count(unsigned_& a) noexcept :
			my{&a}
			{}
		unsigned_ value() const noexcept {
			// the last read value of the atomic, for futex wait
			return myvalue;
			}
		bool read_lock(bool first_time) noexcept {
			// returns true if acquired
			//
			// if first_time is
			// - true, this will ++wait-count if cannot lock
			// - false, this will --wait-count if can lock
			//
			// use first_time == true only the first time
			//
			// lock loop:
			//
			//   read_write_count c(lock_atomic);
			//   if(!c.read_lock(true)) {
			//     bool wait = false;
			//     do {
			//       if(wait) {
			//         condition.wait_unlock(mutex);
			//         wait = false;
			//         }
			//       else
			//         wait = mutex.lock();
			//       } while(!c.read_lock(false))
			//     if(wait)
			//       mutex.unlock();
			//     }
			//
			bool r = false;
			unsigned_ l0 = *my, l;
			do {
				l = l0;
				if(r = (l & write) == 0) {
					++l;
					if(!first_time) l -= wait1;
					}
				else if(first_time)
					l += wait1;
				myvalue = l;
				} while(!atomic::compare_set_else_non_atomic_get<atomic::acquire>(*my, l0, l, l0));
			return r;
			}
		void read_timeout() noexcept {
			// use if read_lock times out *after* read_lock(true) was called.
			// will only --wait-count, it will not acquire the lock ever
			// 
			atomic::subtract<atomic::release>(*my, wait1);
			}
		bool read_try_lock() noexcept {
			bool r;
			unsigned_ l = *my;
			while(!atomic::compare_set_else_non_atomic_get<atomic::acquire>(
				*my,
				l,
				(r = (l & write) == 0) ? l + 1 : l,
				l
				));
			return r;
			}
		bool read_unlock() noexcept {
			// returns ture if this thread should wake a writer
			return (atomic::subtract1_get<atomic::release>(*my) & lock) == write;
			}
		bool write_lock(bool first_time) noexcept {
			// use like read_lock
			//
			// post
			// - write-bit is set
			//
			bool r;
			unsigned_ l0 = *my, l;
			do {
				if(r = (l0 & read) == 0) {
					l = l0 | lock;
					if(!first_time) l -= wait1;
					}
				else {
					l = l0 | write;
					if(first_time) l += wait1;
					}
				myvalue = l;
				} while(!atomic::compare_set_else_non_atomic_get<atomic::acquire>(*my, l0, l, l0));
			return r;
			}
		unsigned_ write_timeout(bool& acquired) noexcept {
			// use if write_lock times out *after* write_lock<true> was called.
			// - this can acquire the lock.
			// - will always --wait-count
			//
			// returns number of threads to wake up
			//
			// post
			// - acquired == true and return == 0 if lock was acquired
			//
			unsigned_ l0 = *my, l, wake;
			do {
				wake = 0;
				l = l0 - wait1;
				if(acquired = (l & read) == 0)
					l |= lock;
				else if(
					(l & write) && // write-bit is set and
					(l & lock) != lock // no one is write-locked
					) {
						wake = l >> shift; // wake everyone, possibly no one
						l ^= write; // drop write-bit
						}
				} while(!atomic::compare_set_else_non_atomic_get<atomic::full>(*my, l0, l, l0));
			return wake;
			}
		bool write_try_lock() noexcept {
			bool r;
			unsigned_ l = *my;
			while(!atomic::compare_set_else_non_atomic_get<atomic::acquire>(
				*my,
				l,
				(r = (l & read) == 0) ? l | lock : l,
				l
				));
			return r;
			}
		unsigned_ write_unlock() noexcept {
			// returns number of threads to wake up
			return atomic::subtract_get<atomic::release>(*my, lock) >> shift;
			}
	};

}}}
#endif
