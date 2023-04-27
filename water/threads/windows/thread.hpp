// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_WINDOWS_THREAD_HPP
#define WATER_THREADS_WINDOWS_THREAD_HPP
#include <water/threads/windows/bits.hpp>
#if \
defined(__MINGW32__) && \
!defined(WATER_THREADS_USE_BEGINTHREADEX) && \
!defined(WATER_THREADS_NO_BEGINTHREADEX)
    #define WATER_THREADS_NO_BEGINTHREADEX
#endif
#ifndef WATER_THREADS_NO_BEGINTHREADEX
    #include <process.h> // _beginthreadex
#endif
namespace water { namespace threads {

bool constexpr
    priority_exists = true,
    stack_size_exists = true,
    qos_exists = false,
    relative_priority_exists = true;

using thread_t = dword_t;
// 0 is not a valid thread id because GetThreadId(handle) returns 0 if it fails
    
struct join_t {
    void *handle;
    void* underlying() const noexcept { return handle; }
};

inline thread_t thread() noexcept {
    return GetCurrentThreadId();
}

inline thread_t thread(join_t a) noexcept {
    return GetThreadId(a.handle);
}

inline bool equal(thread_t a, thread_t b) noexcept {
    // this exists because water::posix::threads::equal,
    return a == b;
}

inline bool dont_join(join_t a) noexcept {
    return CloseHandle(a.handle) != 0;
}

inline bool join(join_t a) noexcept {
    ___water_debug(trace << "water::threads::join thread=" << thread(a) << " from thread=" << thread();)
    if(!handle_wait(a.handle)) {
        ___water_debug(trace << "water::threads::join thread=" << thread(a) << " from thread=" << thread() << " joined!";)
        CloseHandle(a.handle);
        return true;
    }
    ___water_debug(trace << "water::threads::join thread=" << thread(a) << " from thread=" << thread() << " failed!";)
    return false;
}

// priority
//
// SetThreadPriority will not succeed for any priority between
// THREAD_PRIORITY_IDLE and THREAD_PRIORITY_TIME_CRITICAL
// - test it with water/threads/windows/!tests/priority.hpp
//
// when process is REALTIME_PRIORITY_CLASS:
//
//   -15 THREAD_PRIORITY_IDLE
//    -7 -6 -5 -4 -3
//    -2 THREAD_PRIORITY_LOWEST
//    -1 THREAD_PRIORITY_BELOW_NORMAL
//     0 THREAD_PRIORITY_NORMAL
//     1 THREAD_PRIORITY_ABOVE_NORMAL
//     2 THREAD_PRIORITY_HIGHEST
//     3 4 5 6
//    15 THREAD_PRIORITY_TIME_CRITICAL
//
// when process is any other class:
//
//   -15 THREAD_PRIORITY_IDLE
//    -2 THREAD_PRIORITY_LOWEST
//    -1 THREAD_PRIORITY_BELOW_NORMAL
//     0 THREAD_PRIORITY_NORMAL
//     1 THREAD_PRIORITY_ABOVE_NORMAL
//     2 THREAD_PRIORITY_HIGHEST
//    15 THREAD_PRIORITY_TIME_CRITICAL
//
// water range is
// - realtime 1 - 16, default is 9
// - not realtime 1 - 7, default is 4
//

static_assert(thread_priority_time_critical == 15, "");
static_assert(thread_priority_normal == 0, "");
static_assert(thread_priority_idle == -15, "");

inline unsigned priority_water_from_windows(int p, unsigned max) noexcept {
    if(!max || p < thread_priority_idle || thread_priority_time_critical < p)
        return 0;
    if(p <= (max == 16 ? -8 : -3))
        return 1;
    if(p >= (max == 16 ? 7 : 3))
        return max;
    return static_cast<unsigned>(p + (max == 16 ? 9 : 4));
}

inline int priority_windows_from_water(unsigned p, unsigned max) noexcept {
    int r = numeric_limits<int>::min();
    if(max && p && p <= max) {
        if(p == 1)
            r = thread_priority_idle;
        else if(p == max)
            r = thread_priority_time_critical;
        else if(max == 7)
            r = static_cast<int>(p) - 4;
        else
            r = static_cast<int>(p) - 9;
    }
    return r;
}

inline unsigned priority_default() noexcept {
    dword_t p = GetPriorityClass(GetCurrentProcess());
    return
        p == realtime_priority_class ? 9 :
        p ? 4 :
        0;
}

inline unsigned priority_max() noexcept {
    dword_t p = GetPriorityClass(GetCurrentProcess());
    return
        p == realtime_priority_class ? 16 :
        p ? 7 :
        0;
}

class priority
{
    unsigned
        my,
        my_max;
    void *myhandle;
    thread_t myid;

public:
    explicit priority(thread_t a = 0) noexcept :
        my(0),
        my_max(priority_max()),
        myhandle(0),
        myid(a)
    {
        a = thread();
        if(!myid || myid == a) {
            myid = a;
            myhandle = GetCurrentThread();
        }
        else
            myhandle = OpenThread(thread_query_information, 0, myid);
        if(myhandle) {
            my = priority_water_from_windows(GetThreadPriority(myhandle), my_max);
            if(myid != a)
                CloseHandle(myhandle);
        }
        myhandle = 0;
    }
    
    explicit priority(join_t a) noexcept :
        my_max(priority_max()),
        myhandle(a.handle),
        myid(0)
    {
        my = priority_water_from_windows(GetThreadPriority(myhandle), my_max);
    }
    
    operator unsigned() const noexcept {
        return my;
    }

    unsigned get() const noexcept {
        return my;
    }

    unsigned max() const noexcept {
        return my ? my_max : 0;
    }

    unsigned min() const noexcept {
        return my ? 1 : 0;
    }

    bool set(unsigned a) noexcept {
        bool r = false;
        int p;
        if(my && (p = priority_windows_from_water(a, my_max)) != numeric_limits<int>::min()) {
            bool close = false;
            void *h = myhandle;
            if(myid) {
                if(myid == thread())
                    h = GetCurrentThread();
                else
                    close = (h = OpenThread(thread_set_information, 0, myid)) != 0;
            }
            if(h && SetThreadPriority(h, p)) {
                r = true;
                my = a;
            }
            if(close)
                CloseHandle(h);
        }
        return r;
    }
};

enum relative_priority_t {
    priority_lower = thread_priority_lowest,
    priority_low = thread_priority_below_normal,
    priority_normal = thread_priority_normal,
    priority_high = thread_priority_above_normal,
    priority_higher = thread_priority_highest
};

inline bool relative_priority(relative_priority_t p) {
    return SetThreadPriority(GetCurrentThread(), p) != 0;
}

enum qos_t {
    qos_error = 0,
    qos_user_interactive = 1,
    qos_user_initiated = 1,
    qos_default = 1,
    qos_utility = 1,
    qos_background = 1,
    qos_unspecified = 1
};

inline qos_t qos(thread_t) noexcept {
    return qos_error;
}

inline qos_t qos(join_t) noexcept {
    return qos_error;
}

inline qos_t qos() noexcept {
    return qos_error;
}

inline bool qos(qos_t) noexcept {
    return false;
}

template<typename call_>
#ifdef WATER_THREADS_NO_BEGINTHREADEX
dword_t WATER_WINDOWS_CALLTYPE
#else
unsigned __stdcall
#endif
thread(void* pointer) {
    ___water_debug(trace << "water::threads thread=" << GetCurrentThreadId() << " begin";)
    call_()(static_cast<typename call_::pointer>(pointer));
    ___water_debug(trace << "water::threads thread=" << GetCurrentThreadId() << " end";)
    return 0; // never return STILL_ACTIVE, 259
}

inline void* run_pointer(void* a) { return a; }
inline void* run_pointer(void const* a) { return const_cast<void*>(a); }

template<typename call_>
void* run(dword_t& id, typename call_::pointer pointer, security_attributes_t *security, size_t stack_size, dword_t creation_flags, int priority) noexcept {
    id = 0; // GetThreadId returns 0 on error
    void *r = 0;
    if(thread_priority_idle <= priority && priority <= thread_priority_time_critical) {
        #ifdef WATER_THREADS_NO_BEGINTHREADEX
        dword_t id2 = 0;
        r = CreateThread(security, static_cast<windows_size_t>(stack_size), &thread<call_>, run_pointer(pointer), creation_flags | (priority ? thread_create_suspended : 0), &id2);
        #else
        unsigned id2 = 0;
        r = reinterpret_cast<void*>(
            _beginthreadex(security, static_cast<unsigned>(stack_size), &thread<call_>, run_pointer(pointer), creation_flags | (priority ? thread_create_suspended : 0), &id2)
        );
        #endif
        if(r) {
            id = id2;
            ___water_debug(trace << "water::threads::run() started thread=" << id;)
            // if priority was not 0, the threads was created suspended. there is
            // no nice way to kill a suspended thread, and if something fails now
            // it is horrible
            // - if SetThreadPriority fails, it will run with the default. the
            //   error is not visible to who ever called run()
            // - if ResumeThread fails, the thread leaks
            if(priority) {
                ___water_debug(bool error = 0 == ) SetThreadPriority(r, priority);
                ___water_assert(!error && "SetThreadPriority failed!");
                if(!(creation_flags & thread_create_suspended)) {
                    // ResumeThread returns previous suspend-count, 0 means not suspended, dword_t(-1) if error
                    dword_t resume;
                    while((resume = ResumeThread(r)) > 1 && resume != static_cast<dword_t>(-1));
                    ___water_assert(resume <= 1 && "ResumeThread failed!");
                    if(resume > 1) {
                        r = 0;
                        id = 0;
                    }
                }
            }
        }
    }
    ___water_assert(r && "thread start failed");
    return r;
}

class run_options
{
    dword_t myflags = 0;
    int mypriority = thread_priority_normal; // is 0
    security_attributes_t *mysecurity = 0;
    size_t mysize = 0;

public:
    dword_t flags() const noexcept {
        return myflags;
    }

    run_options& flags(dword_t f) noexcept {
        myflags = f;
        return *this;
    }

    run_options& priority(unsigned a) noexcept {
        mypriority = priority_windows_from_water(a, priority_max());
        return *this;
    }

    run_options& qos(qos_t) noexcept {
        return *this;
    }

    int priority_windows() const noexcept {
        // returns the windows priority, or int-min if error
        return mypriority;
    }

    run_options& priority_windows(int a) noexcept {
        mypriority = a;
        return *this;
    }
    
    run_options& relative_priority(relative_priority_t a) noexcept {
        return priority_windows(a);
    }

    security_attributes_t* security() const noexcept {
        return mysecurity;
    }

    run_options& security(security_attributes_t* a) noexcept {
        mysecurity = a;
        return *this;
    }

    size_t stack_size() const noexcept {
        return mysize;
    }

    run_options& stack_size(size_t a) noexcept {
        mysize = a;
        return *this;
    }
};

template<typename call_>
bool run(typename call_::pointer p, join_t& j, run_options const& o = run_options()) noexcept {
    thread_t id;
    return (j.handle = run<call_>(id, p, o.security(), o.stack_size(), o.flags(), o.priority_windows())) != 0;
}

template<typename call_>
bool run(typename call_::pointer p, thread_t& t, run_options const& o = run_options()) noexcept {
    if(void *h = run<call_>(t, p, o.security(), o.stack_size(), o.flags(), o.priority_windows())) {
        CloseHandle(h);
        return true;
    }
    return false;
}

template<typename call_>
bool run(typename call_::pointer p, run_options const& o = {}) noexcept {
    thread_t t;
    return run<call_>(p, t, o);
}

template<typename function_>
struct run_function {
    using pointer = function_*;
    void operator()(pointer p) { (*p)(); }
};

template<typename function_>
bool run(function_& f, join_t& j, run_options const& o = {}) noexcept {
    return run<run_function<function_>>(&f, j, o);
}

template<typename function_>
bool run(function_& f, thread_t& t, run_options const& o = {}) noexcept {
    return run<run_function<function_>>(&f, t, o);
}

template<typename function_>
bool run(function_& f, run_options const& o = {}) noexcept {
    return run<run_function<function_>>(&f, o);
}

}}
#endif
