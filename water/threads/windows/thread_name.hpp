// Copyright 2022-2026 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_WINDOWS_THREAD_NAME_HPP
#define WATER_THREADS_WINDOWS_THREAD_NAME_HPP
#include <water/threads/windows/once.hpp>
#include <water/threads/windows/thread.hpp>
namespace water { namespace threads {

/*

Windows 10 1607 or later

*/

WATER_WINDOWS_FUNCTION(void*, LocalFree, (void*));

using SetThreadDescription_pointer = long_t (WATER_WINDOWS_CALLTYPE*)(void*, wchar_t const*);
using GetThreadDescription_pointer = long_t (WATER_WINDOWS_CALLTYPE*)(void*, wchar_t**);

template<typename = void>
struct thread_description_functions
{
    static SetThreadDescription_pointer SetThreadDescription;
    static GetThreadDescription_pointer GetThreadDescription;

    static void load() noexcept {
        once(load_once);
    }

private:

    static threads::once once;

    static void load_once() noexcept {
        if(auto h = LoadLibraryExW(L"KernelBase.dll", 0, 0)) {
            SetThreadDescription = reinterpret_cast<SetThreadDescription_pointer>(GetProcAddress(h, "SetThreadDescription"));
            GetThreadDescription = reinterpret_cast<GetThreadDescription_pointer>(GetProcAddress(h, "GetThreadDescription"));
            if(!SetThreadDescription && !GetThreadDescription)
                FreeLibrary(h);
        }
    }

};
template<typename a_> SetThreadDescription_pointer thread_description_functions<a_>::SetThreadDescription = 0;
template<typename a_> GetThreadDescription_pointer thread_description_functions<a_>::GetThreadDescription = 0;
template<typename a_> once thread_description_functions<a_>::once {};




class thread_description
{
    using functions = thread_description_functions<>;

private:

    void *my = 0;
    bool myclose = false;

public:
    
    thread_description() noexcept :
        my{GetCurrentThread()}
    {}

    explicit thread_description(void *thread) noexcept :
        my{thread}
    {}
    
    explicit thread_description(thread_t id) noexcept {
        if(id == thread())
            my = GetCurrentThread();
        else {
            my = OpenThread(thread_query_information, 0, id);
            myclose = true;
        }

    }

    ~thread_description() {
        if(myclose)
            CloseHandle(my);
    }

    thread_description(thread_description const&) = delete;
    thread_description& operator=(thread_description const&) = delete;

    bool set(char const* name) noexcept {
        functions::load();
        if(!functions::SetThreadDescription)
            return false;
        wchar_t n[128];
        unsigned s = 0;
        if(name && *name)
            do n[s++] = static_cast<wchar_t>(ascii_subset(*name)); while(*++name && s != 127); // no unicode please
        n[s] = 0;
        return functions::SetThreadDescription(my, n) >= 0;
    }

    size_t get(char* to, size_t to_size) noexcept {
        // will zero-terminate
        functions::load();
        if(!functions::GetThreadDescription || !to || !to_size)
            return 0;
        wchar_t *w = 0;
        if(functions::GetThreadDescription(my, &w) < 0 || !w) {
            *to = 0;
            return 0;
        }
        wchar_t *i = w;
        --to_size;
        size_t r = 0;
        while(*i && r != to_size)
            to[r++] = static_cast<char>(ascii_subset(*i++));
        to[r] = 0;
        LocalFree(w);
        return r;
    }

private:

    template<typename char_>
    static char_ ascii_subset(char_ a) {
        return 0x20 <= a && a <= 0x7e ? a : static_cast<char_>(u'?');
    }
};



inline bool thread_name(char const* name) noexcept {
    return thread_description{}.set(name);
}

inline bool thread_name(join_t a, char const* name) noexcept {
    return thread_description{a.handle}.set(name);
}

inline bool thread_name(thread_t a, char const* name) noexcept {
    return thread_description{a}.set(name);
}



inline size_t thread_name(char* to, size_t to_size) noexcept {
    return thread_description{}.get(to, to_size);
}

inline size_t thread_name(char* to, size_t to_size, join_t a) noexcept {
    return thread_description{a.handle}.get(to, to_size);
}

inline size_t thread_name(char* to, size_t to_size, thread_t a) noexcept {
    return thread_description{a}.get(to, to_size);
}



}}
#endif
