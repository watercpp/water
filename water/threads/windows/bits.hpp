// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_WINDOWS_BITS_HPP
#define WATER_THREADS_WINDOWS_BITS_HPP
#include <water/threads/bits.hpp>
#include <water/windows.hpp>
#ifdef WATER_THREADS_STATISTICS
    #include <water/threads/statistics/statistics.hpp>
#endif
namespace water { namespace threads {

#ifdef WATER_NO_WINDOWS_H

using bool_t = int;
using boolean_t = unsigned char;
using dword_t = unsigned long;
using long_t = long;
using ulong_t = unsigned long;
using ulongptr_t = ifel<sizeof(unsigned long) == sizeof(void*), unsigned long, uint_size<sizeof(void*)>>;
using ulonglong_t = unsigned long long;
using windows_size_t = ulongptr_t;

using security_attributes_t = void*;

struct synchronization_barrier_t {
    dword_t reserved1;
    dword_t reserved2;
    ulongptr_t reserved3[2];
    dword_t reserved4;
    dword_t reserved5;
};

struct critical_section_t {
    void *debug_info;
    long_t lock_count;
    long_t recursion_count;
    void *owning_thread;
    void *lock_semaphore;
    ulongptr_t spin_count;
};

struct condition_variable_t { void* pointer; };
struct init_once_t { void* pointer; };
struct srwlock_t { void* pointer; };

// SRWLOCK_INIT {0}
// CONDITION_VARIABLE_INIT {0}, is undocumented
// RTL_RUN_ONCE_INIT {0}

#else

using bool_t = BOOL;
using boolean_t = BOOLEAN;
using dword_t = DWORD;
using long_t = LONG;
using ulong_t = ULONG;
using ulongptr_t = ULONG_PTR;
using ulonglong_t = ULONGLONG;
using windows_size_t = SIZE_T;

using security_attributes_t = SECURITY_ATTRIBUTES;

using condition_variable_t = CONDITION_VARIABLE;
using critical_section_t = CRITICAL_SECTION;
using init_once_t = INIT_ONCE;
using srwlock_t = SRWLOCK;

#if WATER_WINDOWS_VERSION >= WATER_WINDOWS_8
    // only for windows desktop apps, not store apps
    using synchronization_barrier_t = SYNCHRONIZATION_BARRIER;
#endif

#endif

WATER_WINDOWS_FUNCTION(bool_t,  CloseHandle, (void*));
//WATER_WINDOWS_FUNCTION(dword_t, GetLastError, ());

WATER_WINDOWS_FUNCTION(void*,   GetCurrentThread, ());
WATER_WINDOWS_FUNCTION(dword_t, GetCurrentThreadId, ());
WATER_WINDOWS_FUNCTION(dword_t, GetThreadId, (void*));
WATER_WINDOWS_FUNCTION(void*,   GetCurrentProcess, ());
WATER_WINDOWS_FUNCTION(dword_t, GetCurrentProcessId, ());
WATER_WINDOWS_FUNCTION(void,    Sleep, (dword_t));
WATER_WINDOWS_FUNCTION(bool_t,  SwitchToThread, ());

WATER_WINDOWS_FUNCTION(void*,   CreateThread, (security_attributes_t*, windows_size_t, dword_t (WATER_WINDOWS_CALLTYPE*)(void*), void*, dword_t, dword_t*));
WATER_WINDOWS_FUNCTION(dword_t, GetPriorityClass, (void*));
WATER_WINDOWS_FUNCTION(int,     GetThreadPriority, (void*));
WATER_WINDOWS_FUNCTION(void*,   OpenThread, (dword_t, bool_t, dword_t));
WATER_WINDOWS_FUNCTION(dword_t, ResumeThread, (void*));
WATER_WINDOWS_FUNCTION(bool_t,  SetThreadPriority, (void*, int));

WATER_WINDOWS_FUNCTION(ulonglong_t, GetTickCount64, ());

WATER_WINDOWS_FUNCTION(dword_t, WaitForMultipleObjects, (dword_t, void *const*, bool_t, dword_t));
WATER_WINDOWS_FUNCTION(dword_t, WaitForSingleObject, (void*, dword_t));

WATER_WINDOWS_FUNCTION(void*,  CreateEventW, (security_attributes_t*, bool_t, bool_t, wchar_t const*));
WATER_WINDOWS_FUNCTION(void*,  OpenEventW, (dword_t, bool_t, wchar_t const*));
WATER_WINDOWS_FUNCTION(bool_t, ResetEvent, (void*));
WATER_WINDOWS_FUNCTION(bool_t, SetEvent, (void*));

WATER_WINDOWS_FUNCTION(void*,  CreateSemaphoreW, (security_attributes_t*, long_t, long_t, wchar_t const*));
WATER_WINDOWS_FUNCTION(void*,  OpenSemaphoreW, (dword_t, bool_t, wchar_t const*));
WATER_WINDOWS_FUNCTION(bool_t, ReleaseSemaphore, (void*, long_t, long_t*));

WATER_WINDOWS_FUNCTION(void*,   CreateMutexW, (security_attributes_t*, bool_t, wchar_t const*));
WATER_WINDOWS_FUNCTION(void*,   OpenMutexW, (dword_t, bool_t, wchar_t const*));
WATER_WINDOWS_FUNCTION(bool_t,  ReleaseMutex, (void*));

WATER_WINDOWS_FUNCTION(void,   DeleteCriticalSection, (critical_section_t*));
WATER_WINDOWS_FUNCTION(void,   EnterCriticalSection, (critical_section_t*));
WATER_WINDOWS_FUNCTION(void,   InitializeCriticalSection, (critical_section_t*));
WATER_WINDOWS_FUNCTION(void,   LeaveCriticalSection, (critical_section_t*));
WATER_WINDOWS_FUNCTION(bool_t, TryEnterCriticalSection, (critical_section_t*));

WATER_WINDOWS_FUNCTION(void,      AcquireSRWLockExclusive, (srwlock_t*));
WATER_WINDOWS_FUNCTION(void,      AcquireSRWLockShared, (srwlock_t*));
WATER_WINDOWS_FUNCTION(void,      ReleaseSRWLockExclusive, (srwlock_t*));
WATER_WINDOWS_FUNCTION(void,      ReleaseSRWLockShared, (srwlock_t*));
WATER_WINDOWS_FUNCTION(boolean_t, TryAcquireSRWLockExclusive, (srwlock_t*));
WATER_WINDOWS_FUNCTION(boolean_t, TryAcquireSRWLockShared, (srwlock_t*));

WATER_WINDOWS_FUNCTION(void, WakeAllConditionVariable, (condition_variable_t*));
WATER_WINDOWS_FUNCTION(void, WakeConditionVariable, (condition_variable_t*));
WATER_WINDOWS_FUNCTION(bool_t, SleepConditionVariableCS, (condition_variable_t*, critical_section_t*, dword_t milli));
WATER_WINDOWS_FUNCTION(bool_t, SleepConditionVariableSRW, (condition_variable_t*, srwlock_t*, dword_t milli, ulong_t flags));

WATER_WINDOWS_FUNCTION(bool_t, InitOnceExecuteOnce, (init_once_t*, bool_t (WATER_WINDOWS_CALLTYPE*)(init_once_t*, void*, void**), void *pointer, void **return_));

#if !(defined(NTDDI_VERSION) && defined(NTDDI_WIN8)) || NTDDI_VERSION >= NTDDI_WIN8
    
    WATER_WINDOWS_FUNCTION(bool_t, DeleteSynchronizationBarrier, (synchronization_barrier_t*));
    WATER_WINDOWS_FUNCTION(bool_t, EnterSynchronizationBarrier, (synchronization_barrier_t*, dword_t));
    WATER_WINDOWS_FUNCTION(bool_t, InitializeSynchronizationBarrier, (synchronization_barrier_t*, long_t count, long_t spin_count));

    WATER_WINDOWS_FUNCTION(bool_t, WaitOnAddress, (void volatile* address, void *compare, windows_size_t size, dword_t milli));
    WATER_WINDOWS_FUNCTION(void, WakeByAddressSingle, (void*));
    WATER_WINDOWS_FUNCTION(void, WakeByAddressAll, (void*));

#endif

dword_t constexpr
    wait_forever           = WATER_WINDOWS_SELECT(INFINITE, 0xfffffffful),
    wait_timeout           = WATER_WINDOWS_SELECT(WAIT_TIMEOUT, 0x102),
    event_synchronize      = WATER_WINDOWS_SELECT(SYNCHRONIZE, 0x100000ul),
    event_modify_state     = WATER_WINDOWS_SELECT(EVENT_MODIFY_STATE, 0x2),
    event_all_access       = WATER_WINDOWS_SELECT(EVENT_ALL_ACCESS, 0x1f0003ul),
    semaphore_synchronize  = WATER_WINDOWS_SELECT(SYNCHRONIZE, event_synchronize),
    mutex_modify_state     = WATER_WINDOWS_SELECT(EVENT_MODIFY_STATE, 0x1),
    semaphore_modify_state = WATER_WINDOWS_SELECT(SEMAPHORE_MODIFY_STATE, event_modify_state),
    semaphore_all_access   = WATER_WINDOWS_SELECT(SEMAPHORE_ALL_ACCESS, event_all_access),
    realtime_priority_class  = WATER_WINDOWS_SELECT(REALTIME_PRIORITY_CLASS, 0x100),
    thread_create_suspended  = WATER_WINDOWS_SELECT(CREATE_SUSPENDED, 0x4),
    thread_set_information   = WATER_WINDOWS_SELECT(THREAD_SET_INFORMATION, 0x20),
    thread_query_information = WATER_WINDOWS_SELECT(THREAD_QUERY_INFORMATION, 0x40);

int constexpr
    thread_priority_idle          = WATER_WINDOWS_SELECT(THREAD_PRIORITY_IDLE, -15),
    thread_priority_lowest        = WATER_WINDOWS_SELECT(THREAD_PRIORITY_LOWEST, -2),
    thread_priority_below_normal  = WATER_WINDOWS_SELECT(THREAD_PRIORITY_BELOW_NORMAL, -1),
    thread_priority_normal        = WATER_WINDOWS_SELECT(THREAD_PRIORITY_NORMAL, 0),
    thread_priority_above_normal  = WATER_WINDOWS_SELECT(THREAD_PRIORITY_ABOVE_NORMAL, 1),
    thread_priority_highest       = WATER_WINDOWS_SELECT(THREAD_PRIORITY_HIGHEST, 2),
    thread_priority_time_critical = WATER_WINDOWS_SELECT(THREAD_PRIORITY_TIME_CRITICAL, 15);
    // THREAD_PRIORITY_ERROR_RETURN is 32-bit int-max

void *constexpr handle_bad = WATER_WINDOWS_SELECT(INVALID_HANDLE_VALUE, reinterpret_cast<void*>(static_cast<uint_size<sizeof(void*)>>(-1)));

using handle_atomic = atomic<void*>;

inline dword_t handle_wait(void *handle, dword_t milli = wait_forever) noexcept {
    ___water_assert(handle);
    dword_t error = WaitForSingleObject(handle, milli);
    ___water_assert(!error || (milli != wait_forever && error == wait_timeout));
    return error;
}

inline dword_t handle_wait_any(void *handle0, void *handle1, dword_t milli = wait_forever) noexcept {
    ___water_assert(handle0 && handle1);
    void *h[2] = { handle0, handle1 };
    dword_t error = WaitForMultipleObjects(2, h, 0, milli);
    ___water_assert(error < 2 || (milli != wait_forever && error == wait_timeout));
    return error;
}


class handle_close
{
    void *my;

public:
    explicit handle_close(void *a = 0) noexcept :
        my(a)
    {}
    
    ~handle_close() noexcept {
        if(my)
            CloseHandle(my);
    }
    
    handle_close(handle_close const&) = delete;
    handle_close& operator=(handle_close const&) = delete;
    
    void* get() const noexcept {
        return my;
    }
    
    void set(void *a) noexcept {
        my = a;
    }
};


class handle_hold
{
    void *my;

public:
    explicit handle_hold(void *a = 0) noexcept :
        my(a)
    {}
    
    void* get() const noexcept {
        return my;
    }
    
    void set(void *a) noexcept {
        my = a;
    }
};


inline dword_t milli_from_seconds(double a) noexcept {
    a *= 1e3;
    return a < 0.5 ? 0 : static_cast<dword_t>(a + 0.5);
}

template<typename create_>
void* atomic_create(handle_atomic& handle, create_ create) noexcept {
    // this uses
    // - handle == 0 means not inited
    // - handle == handle_bad (INVALID_HANDLE_VALUE) means locked, someone else is initing (yield() then retry)
    // - anything else means it is inited
    //
    // because UnregisterWaitEx and DeleteTimerQueueEx, event-handles cannot be INVALID_HANDLE_VALUE, but other handles could
    //
    // if create() returns INVALID_HANDLE_VALUE, that handle will leak
    //
    void *h;
    while(!handle.compare_exchange_strong(h = 0, static_cast<void*>(handle_bad), memory_order_acquire) && h == handle_bad)
        if(!SwitchToThread())
            Sleep(1);
    if(!h) {
        if((h = create()) == handle_bad) {
            ___water_assert(h != handle_bad && "create() returned INVALID_HANDLE_VALUE");
            h = 0; // leak it
        }
        handle.store(h);
    }
    return h;
}

inline void* semaphore_create(long_t value = 0, wchar_t const* name = 0) noexcept {
    ___water_assert(value >= 0);
    return CreateSemaphoreW(0, value, numeric_limits<long_t>::max(), name);
}

}}
#endif
