// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_POSIX_THREAD_HPP
#define WATER_THREADS_POSIX_THREAD_HPP
#include <water/threads/posix/bits.hpp>
namespace water { namespace threads {

bool constexpr stack_size_exists =
    #ifdef WATER_POSIX_THREAD_ATTR_STACKSIZE
    true;
    #else
    false;
    #endif

bool constexpr priority_exists =
    #ifdef WATER_POSIX_THREAD_PRIORITY_SCHEDULING
    true;
    #else
    false;
    #endif

bool constexpr qos_exists =
    #ifdef WATER_THREADS_APPLE
    true;
    #else
    false;
    #endif

typedef pthread_t thread_t; // this could be a struct. then things below will break

struct join_t {
    pthread_t thread;
    pthread_t underlying() const noexcept { return thread; }
};

inline thread_t thread() noexcept {
    return pthread_self();
}

inline thread_t thread(join_t a) noexcept {
    return a.thread;
}

inline bool equal(thread_t a, thread_t b) noexcept {
    // both a and b must be valid (running, or not yet join()-ed), see pthread_equal
    return pthread_equal(a, b);
}

inline bool dont_join(join_t a) noexcept {
    return pthread_detach(a.thread) == 0;
}

inline bool join(join_t a) noexcept {
    ___water_debug(trace << "water::threads::join thread=" << thread(a) << " from thread=" << thread();)
    int e = pthread_join(a.thread, 0);
    if(!e) {
        ___water_debug(trace << "water::threads::join thread=" << thread(a) << " from thread=" << thread() << " joined!";)
        return true;
    }
    ___water_debug(trace << "water::threads::join thread=" << thread(a) << " from thread=" << thread() << " failed! error=" << e;)
    return false;
}

#ifdef WATER_POSIX_THREAD_PRIORITY_SCHEDULING

// mac has PTHREAD_MIN_PRIORITY and PTHREAD_MAX_PRIORITY, look at pthread_getschedparam. any use?
// sched_get_priority_min returns -1 on error, it probably means, but does not have to
// - minimum is 0 or more
// - maximum is abovoe minimum

class priority_translate // between posix range and water range 1 or more
{
    int mymin = 0;
    int mymax = 0;
    bool myerror = false;

public:
    priority_translate() noexcept = default;

    explicit priority_translate(int sched) noexcept :
        mymin(sched_get_priority_min(sched)), // returns -1 if error
        mymax(sched_get_priority_max(sched)) // returns -1 if error
    {
        ___water_assert((mymin == -1 && mymax == -1) || (mymin >= 0 && mymax >= mymin));
        myerror = mymin < 0 || mymax < mymin;
        if(mymin < 0 || mymax <= mymin)
            mymin = mymax = 0;
    }

    explicit operator bool() const noexcept {
        // returns false if priority cannot be changed. that is not always an error
        return mymin != mymax;
    }

    bool error() const noexcept {
        return myerror;
    }

    unsigned water_from_posix(int a) const noexcept {
        return static_cast<unsigned>(a - mymin) + 1;
    }

    int posix_from_water(unsigned a) const noexcept {
        return static_cast<int>(a) + mymin - 1;
    }

    bool posix_is_valid(int a) const noexcept {
        return mymin <= a && a <= mymax;
    }

    unsigned water_max() const noexcept {
        return static_cast<unsigned>(mymax - mymin) + 1;
    }
};

inline unsigned priority_default() noexcept {
    unsigned r = 0;
    #ifdef WATER_POSIX_PRIORITY_SCHEDULING
    int s = sched_getscheduler(0);
    if(s != -1) {
        priority_translate t(s);
        sched_param p;
        if(t && !sched_getparam(0, &p) && t.posix_is_valid(p.sched_priority))
            r = t.water_from_posix(p.sched_priority);
    }
    #else
    pthread_attr_t a;
    if(!pthread_attr_init(&a)) {
        sched_param p{};
        int s = 0;
        if(!pthread_attr_getschedpolicy(&a, &s) && !pthread_attr_getschedparam(&a, &p)) {
            priority_translate t(s);
            if(t && t.posix_is_valid(p.sched_priority))
                r = t.water_from_posix(p.sched_priority);
        }
        pthread_attr_destroy(&a);
    }
    #endif
    return r;
}

class priority
{
    thread_t my;
    int mysched = 0;
    sched_param myparam{};
    priority_translate mytranslate;
    unsigned mypriority = 0;
    bool myerror = false;

public:
    explicit priority(thread_t a) noexcept :
        my(a)
    {
        myerror = true;
        if(!pthread_getschedparam(my, &mysched, &myparam)) {
            mytranslate = priority_translate(mysched);
            myerror = mytranslate.error();
            if(mytranslate && mytranslate.posix_is_valid(myparam.sched_priority))
                mypriority = mytranslate.water_from_posix(myparam.sched_priority);
        }
    }

    priority() noexcept :
        priority(thread())
    {}

    explicit priority(join_t a) noexcept :
        priority(thread(a))
    {}

    bool error() const noexcept {
        // this can return false, even if get() or max() return 0, if it is not possible to set the priority
        return myerror;
    }

    operator unsigned() const noexcept {
        return mypriority;
    }

    unsigned get() const noexcept {
        return mypriority;
    }

    unsigned max() const noexcept {
        return mypriority ? mytranslate.water_max() : 0;
    }

    unsigned min() const noexcept {
        return mypriority ? 1 : 0;
    }

    bool set(unsigned a) noexcept {
        // mac does not have pthread_setschedprio and _POSIX_THREAD_PRIORITY_SCHEDULING is -1
        // cygwin has _POSIX_THREAD_PRIORITY_SCHEDULING == 1, but no pthread_setschedprio maybe because _POSIX_VERSION is 199309
        if(!mypriority || !a || a > mytranslate.water_max())
            return false;
        int i = mytranslate.posix_from_water(a);
        #if _POSIX_THREAD_PRIORITY_SCHEDULING > 1 && (!defined(__ANDROID_API__) || __ANDROID_API__ >= 28)
        if(pthread_setschedprio(my, i))
            return false;
        #else
        sched_param p = myparam;
        p.sched_priority = i;
        if(pthread_setschedparam(my, mysched, &p))
            return false;
        #endif
        mypriority = a;
        myparam.sched_priority = i;
        return true;
    }
};

#else //WATER_POSIX_THREAD_PRIORITY_SCHEDULING

inline unsigned priority_default() noexcept {
    return 0;
}

class priority
{
public:
    priority() noexcept
    {}

    explicit priority(thread_t) noexcept
    {}

    explicit priority(join_t) noexcept
    {}

    operator unsigned() const noexcept {
        return 0;
    }

    unsigned get() const noexcept {
        return 0;
    }

    unsigned max() const noexcept {
        return 0;
    }

    unsigned min() const noexcept {
        return 0;
    }

    bool set(unsigned) const noexcept {
        return false;
    }
};

#endif //WATER_POSIX_THREAD_PRIORITY_SCHEDULING


#ifdef WATER_THREADS_APPLE

enum qos_t {
    qos_error = 0,
    qos_user_interactive = QOS_CLASS_USER_INTERACTIVE + 1,
    qos_user_initiated = QOS_CLASS_USER_INITIATED + 1,
    qos_default = QOS_CLASS_DEFAULT + 1, // new threads get this by default
    qos_utility = QOS_CLASS_UTILITY + 1,
    qos_background = QOS_CLASS_BACKGROUND + 1,
    qos_unspecified = QOS_CLASS_UNSPECIFIED + 1 // is 0
};

inline qos_t qos(thread_t a) noexcept {
    if(&pthread_get_qos_class_np == NULL)
        return qos_error;
    qos_class_t c = QOS_CLASS_UNSPECIFIED;
    int p = 0;
    auto e = pthread_get_qos_class_np(a, &c, &p);
    ___water_assert(!e && "pthread_get_qos_class_np failed");
    return e ? qos_error : static_cast<qos_t>(static_cast<unsigned>(c) + 1);
}

inline qos_t qos(join_t a) noexcept {
    return qos(thread(a));
}

inline qos_t qos() noexcept {
    return qos(thread());
}

inline bool qos(qos_t a) noexcept {
    if(&pthread_set_qos_class_self_np == NULL || !a)
        return 0;
    auto e = pthread_set_qos_class_self_np(static_cast<qos_class_t>(static_cast<unsigned>(a) - 1), 0);
    if(!e)
        return true;
    // if thread priority has been set with pthread_setschedparam, this will fail with EPERM
    ___water_debug(e = errno;)
    ___water_assert((e != EPERM) && "pthread_set_qos_class_self_np failed, probably because the thread priority was set");
    ___water_assert((e == EPERM) && "pthread_set_qos_class_self_np failed");
    return false;
}

#else

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

#endif

enum relative_priority_t {
    priority_lower = -2,
    priority_low = -1,
    priority_normal = 0,
    priority_high = 1,
    priority_higher = 2
};

inline unsigned priority_from_relative_priority(int relative, unsigned normal, unsigned max) {
    int r = static_cast<int>(normal) - relative;
    if(r < 1)
        return 1;
    if(static_cast<unsigned>(r) > max)
        return max;
    return static_cast<unsigned>(r);
}

#ifdef WATER_THREADS_APPLE

bool constexpr relative_priority_exists = true;

struct qos_from_relative_priority_return {
    qos_class_t qos = QOS_CLASS_DEFAULT;
    int relative = 0;
};

inline qos_from_relative_priority_return qos_from_relative_priority(int p) {
    qos_from_relative_priority_return r;
    r.qos = p < priority_normal ? QOS_CLASS_UTILITY :
        p == priority_high ? QOS_CLASS_USER_INITIATED :
        p == priority_higher ? QOS_CLASS_USER_INTERACTIVE :
        QOS_CLASS_DEFAULT;
    if(p == priority_lower)
        r.relative = QOS_MIN_RELATIVE_PRIORITY / 2; // min is -15
    return r;
}

inline bool relative_priority(relative_priority_t p) {
    if(&pthread_set_qos_class_self_np == NULL)
        return true;
    auto q = qos_from_relative_priority(p);
    auto e = pthread_set_qos_class_self_np(q.qos, q.relative);
    ___water_assert(!e && "pthread_set_qos_class_self_np failed");
    return e == 0;
}

#elif defined(WATER_POSIX_THREAD_PRIORITY_SCHEDULING)

bool constexpr relative_priority_exists = true;

inline bool relative_priority(relative_priority_t p) {
    priority set;
    if(set.max() < 2)
        return !set.error();
    return set.set(priority_from_relative_priority(p, priority_default(), set.max()));
}

#else

bool constexpr relative_priority_exists = false;

bool constexpr relative_priority(relative_priority_t) {
    return true;
}

#endif


class run_options
{
    unsigned mypriority = 0;
    int myrelative = priority_lower - 1;
    size_t mysize = 0;
    qos_t myqos = qos_error;

public:
    constexpr run_options() noexcept = default;

    constexpr run_options(relative_priority_t a) noexcept :
        myrelative{a}
    {}

    run_options& priority(unsigned a) noexcept {
        ___water_assert(myrelative < priority_lower && myqos == qos_error && "use one of priority, relative_priority, quos");
        mypriority = a;
        return *this;
    }
    
    run_options& relative_priority(relative_priority_t a) noexcept {
        ___water_assert(!mypriority && myqos == qos_error && "use one of priority, relative_priority, quos");
        myrelative = a;
        return *this;
    }

    run_options& stack_size(size_t a) noexcept {
        mysize = a;
        return *this;
    }

    run_options& qos(qos_t a) noexcept {
        ___water_assert(!mypriority && myrelative < priority_lower && "use one of priority, relative_priority, quos");
        myqos = a;
        return *this;
    }

    bool to(pthread_attr_t& a) const noexcept {
        bool r = true;
        
        #ifdef WATER_POSIX_THREAD_ATTR_STACKSIZE
        r = !mysize || !pthread_attr_setstacksize(&a, mysize);
        #else
        r = mysize == 0;
        ___water_assert(!mysize && "stack size used, but stack_size_exists = false");
        #endif
        
        #ifdef WATER_POSIX_THREAD_PRIORITY_SCHEDULING
        if(r && (mypriority || (myrelative >= priority_lower && !qos_exists))) {
            r = false;
            sched_param p {};
            int s;
            if(
                #if defined(PTHREAD_EXPLICIT_SCHED) && (!defined(__ANDROID_API__) || __ANDROID_API__ >= 28)
                !pthread_attr_setinheritsched(&a, PTHREAD_EXPLICIT_SCHED) && // not defined on android, not sure if this hurts?
                #endif
                !pthread_attr_getschedpolicy(&a, &s) &&
                !pthread_attr_getschedparam(&a, &p)
            ) {
                priority_translate t(s);
                if(!t)
                    r = !t.error() && !mypriority; // its ok if relative_priority cannot be set because there is only 1 priority
                else {
                    ___water_assert(mypriority <= t.water_max() && "cannot set priority higher than max");
                    if(myrelative >= priority_lower) {
                        if(t.posix_is_valid(p.sched_priority)) {
                            p.sched_priority = priority_from_relative_priority(myrelative, t.water_from_posix(p.sched_priority), t.water_max());
                            r = true;
                        }
                    }
                    else if(mypriority <= t.water_max()) {
                        p.sched_priority = t.posix_from_water(mypriority);
                        r = true;
                    }
                    if(r)
                        r = pthread_attr_setschedparam(&a, &p) == 0;
                }
            }
        }
        #else
        r = r && !mypriority;
        ___water_assert(!mypriority && "priority used, but priority_exists = false");
        #endif
        #ifdef WATER_THREADS_APPLE
        if(r && (myqos != qos_error || myrelative >= priority_lower) && &pthread_attr_set_qos_class_np != NULL) {
            qos_from_relative_priority_return q;
            if(myrelative >= priority_lower)
                q = qos_from_relative_priority(myrelative);
            else
                q.qos = static_cast<qos_class_t>(static_cast<unsigned>(myqos) - 1);
            auto e = pthread_attr_set_qos_class_np(&a, q.qos, q.relative);
            ___water_assert(!e && "pthread_attr_set_qos_class_np");
            r = e == 0;
        }
        #endif
        return r;
    }
};

template<typename call_>
void* thread(void* pointer) {
    ___water_debug(trace << "water::threads thread=" << thread() << " begin";)
    call_()(static_cast<typename call_::pointer>(pointer));
    ___water_debug(trace << "water::threads thread=" << thread() << " end";)
    return 0;
}

inline void* run_pointer(void* a) { return a; }
inline void* run_pointer(void const* a) { return const_cast<void*>(a); }

template<typename call_>
bool run(typename call_::pointer p, thread_t& t, pthread_attr_t const* a) noexcept {
    // when pthread_attr_t is 0, the default attr is used. the default detach state is joinable
    int e = pthread_create(&t, a, &thread<call_>, run_pointer(p));
    if(!e) {
        ___water_debug(trace << "water::threads::run() started thread=" << t;)
        return true;
    }
    ___water_assert(!e && "thread start failed");
    return false;
}

template<typename call_>
bool run(typename call_::pointer p, join_t& j, pthread_attr_t const* a) noexcept {
    return run<call_>(p, j.thread, a);
}

template<typename call_>
bool run(typename call_::pointer p, join_t& j) noexcept {
    return run<call_>(p, j.thread, 0);
}

template<typename call_>
bool run(typename call_::pointer p, join_t& j, run_options const& o) noexcept {
    pthread_attr_t a;
    bool r = false;
    if(!pthread_attr_init(&a)) {
        if(o.to(a) && run<call_>(p, j.thread, &a))
            r = true;
        pthread_attr_destroy(&a);
    }
    return r;
}

template<typename call_>
bool run(typename call_::pointer p, thread_t& t, run_options const& o = {}) noexcept {
    pthread_attr_t a;
    bool r = false;
    if(!pthread_attr_init(&a)) {
        if(!pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED) && o.to(a) && run<call_>(p, t, &a))
            r = true;
        pthread_attr_destroy(&a);
    }
    return r;
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
bool run(function_& f, join_t& j) noexcept {
    return run<run_function<function_>>(&f, j);
}

template<typename function_>
bool run(function_& f, join_t& j, run_options const& o) noexcept {
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
