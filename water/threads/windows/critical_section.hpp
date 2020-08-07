// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_WINDOWS_CRITICAL_SECTION_HPP
#define WATER_THREADS_WINDOWS_CRITICAL_SECTION_HPP
#include <water/threads/windows/bits.hpp>
namespace water { namespace threads {

/*

struct RTL_CRITICAL_SECTION {
  RTL_CRITICAL_SECTION_DEBUG* DebugInfo;
  LONG LockCount; // 0 is unlocked, 1 or more is thread count
  LONG RecursionCount;
  HANDLE OwningThread;  // looks like dword_t GetCurrentThreadId(), even if void*
  HANDLE LockSemaphore; // auto-reset event?
  ULONG_PTR SpinCount;  // used only on multiprocessor?
  };

Before vista InitializeCriticalSection and EnterCriticalSection could raise windows structured exceptions, maybe if
- initialize cannot allocate DebugInfo
- enter cannot create LockSemaphore (probably never on windows
  xp or later with "keyed events")

*/

inline void lock(critical_section_t& a) noexcept {
    EnterCriticalSection(&a);
}

inline bool try_lock(critical_section_t& a) noexcept {
    return TryEnterCriticalSection(&a) != 0;
}

inline void unlock(critical_section_t& a) noexcept {
    LeaveCriticalSection(&a);
}

class critical_section
{
public:
    using needs = threads::needs<need_system, need_recursive>;

private:
    critical_section_t my;
    ___water_threads_statistics(threads::statistics::reference mystatistics;)

public:
    critical_section() noexcept {
        InitializeCriticalSection(&my);
    }

    ~critical_section() noexcept {
        DeleteCriticalSection(&my);
    }

    critical_section(critical_section const&) = delete;
    critical_section& operator=(critical_section const&) = delete;

    void lock() noexcept {
        ___water_threads_statistics(if(try_lock()) return;)
        threads::lock(my);
    }

    bool try_lock() noexcept {
        bool r = threads::try_lock(my);
        ___water_threads_statistics(threads::statistics::add(mystatistics, this, "critical_section").wait(r));
        return r;
    }

    void unlock() noexcept {
        threads::unlock(my);
    }

    critical_section_t& underlying() noexcept {
        return my;
    }

    ___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "critical_section"); })
};

}}
#endif
