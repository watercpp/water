// Copyright 2022-2026 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_POSIX_THREAD_NAME_HPP
#define WATER_THREADS_POSIX_THREAD_NAME_HPP
#include <water/threads/posix/thread.hpp>
#if !defined(WATER_THREADS_NO_THREAD_NAME) && defined(__ANDROID_MIN_SDK_VERSION__) && __ANDROID_MIN_SDK_VERSION__ < 26
    #include <dlfcn.h>
    #include <water/threads/posix/once.hpp>
#endif
namespace water { namespace threads {

/*

int pthread_setname_np(pthread_t thread, const char *name)
int pthread_getname_np(pthread_t, char*, size_t)

These function seems to exist on Linux, Android, FreeBSD, OpenBSD, NetBSD and more.
Linux and Android seem to limit the length to 16 chars, and will error ERANGE when it's too long.
It was added in glibc 2.12 on 2010-05-03

Android added pthread_getname_np in SDK 26 (Android 8.0, 2017)

*/

#ifdef WATER_THREADS_NO_THREAD_NAME


inline bool thread_name(char const* name) noexcept {
    return false;
}

inline bool thread_name(thread_t thread, char const* name) noexcept {
    return false;
}

inline bool thread_name(join_t thread, char const* name) noexcept {
    return false;
}

inline char* thread_name(char* begin, char* end) noexcept {
    return begin;
}

inline char* thread_name(char* begin, char* end, thread_t thread) noexcept {
    return begin;
}

inline char* thread_name(char* begin, char* end, joint_t thread) noexcept {
    return begin;
}


#else


inline bool thread_name(thread_t thread, char const* name) noexcept {
    int e = pthread_setname_np(thread, name);
    if(!e)
        return true;
    if(e == ERANGE) {
        // try cutting it
        char n[64] {};
        unsigned length = 0;
        if(name && *name) {
            do n[length++] = *name; while(*++name && length != 64);
            unsigned sizes[] = {64, 32, 16, 8};
            for(auto const s : sizes) {
                if(length >= s) {
                    n[s - 1] = 0;
                    e = pthread_setname_np(thread, n);
                    if(e != ERANGE)
                        return e == 0;
                }
            }
        }
    }
    return false;
}

inline bool thread_name(char const* name) noexcept {
    return thread_name(thread(), name);
}

inline bool thread_name(join_t thread, char const* name) noexcept {
    return thread_name(thread.thread, name);
}


template<typename function_>
inline size_t thread_name_do(function_&& function, char* to, size_t to_size, thread_t thread) noexcept {
    // will end with zero
    if(!to || !to_size)
        return 0;
    size_t r = 0;
    if(!function(thread, to, to_size)) {
        --to_size;
        while(to[r] && r != to_size)
            ++r;
    }
    to[r] = 0;
    return r;
}


#if defined(__ANDROID_MIN_SDK_VERSION__) && (__ANDROID_MIN_SDK_VERSION__) < 26


using pthread_getname_np_pointer = int (*)(pthread_t, char*, size_t);

template<typename = void>
struct pthread_getname_np_load
{
    static pthread_getname_np_pointer function;
    
    static bool load() {
        once(load_once);
        return function != 0;
    }
    
private:

    static threads::once once;

    static void load_once() {
        void *l = dlopen("libc.so", RTLD_LAZY);
        if(!l) {
            ___water_debug(trace << "water::threads::thread_name dlopen libc.so failed");
            return;
        }
        void *s = dlsym(l, "pthread_getname_np");
        ___water_debug(trace << "water::threads::thread_name loaded pthread_getname_np " << s);
        if(!s)
            dlclose(l); // kind of pointless
        else
            function = reinterpret_cast<pthread_getname_np_pointer>(s);
    }
};
template<typename a_> pthread_getname_np_pointer pthread_getname_np_load<a_>::function = 0;
template<typename a_> once pthread_getname_np_load<a_>::once {};

inline size_t thread_name(char* to, size_t to_size, thread_t thread) noexcept {
    if(pthread_getname_np_load<>::load())
        return thread_name_do(pthread_getname_np_load<>::function, to, to_size, thread);
    return 0;
}


#else


inline size_t thread_name(char* to, size_t to_size, thread_t thread) noexcept {
    return thread_name_do(
        [](pthread_t p, char* c, size_t s) { return pthread_getname_np(p, c, s); }, // lambda if pthread_getname_np is a macro
        to,
        to_size,
        thread
    );
}


#endif


inline size_t thread_name(char* to, size_t to_size) noexcept {
    return thread_name(to, to_size, thread());
}

inline size_t thread_name(char* to, size_t to_size, join_t thread) noexcept {
    return thread_name(to, to_size, thread.thread);
}


#endif

}}
#endif
