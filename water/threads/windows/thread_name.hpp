// Copyright 2022 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_WINDOWS_THREAD_NAME_HPP
#define WATER_THREADS_WINDOWS_THREAD_NAME_HPP
#include <water/threads/windows/once.hpp>
namespace water { namespace threads {

/*

Windows 10 1607 or later

*/

class set_thread_description_function
{
    long_t (WATER_WINDOWS_CALLTYPE *SetThreadDescription)(void*, wchar_t const*) = 0;

public:
    
    void load() noexcept {
        if(auto h = LoadLibraryExW(L"KernelBase.dll", 0, 0)) {
            SetThreadDescription = reinterpret_cast<decltype(SetThreadDescription)>(GetProcAddress(h, "SetThreadDescription"));
            if(!SetThreadDescription)
                FreeLibrary(h);
        }
    }
    
    bool operator()(char const *name) const noexcept {
        if(!SetThreadDescription)
            return false;
        wchar_t n[128];
        unsigned s = 0;
        if(name && *name)
            do n[s++] = static_cast<wchar_t>(*name); while(*++name && s != 127); // no unicode please
        n[s] = 0;
        return SetThreadDescription(GetCurrentThread(), n) >= 0;
    }
};


template<typename = void>
class set_thread_description
{
    static once load;
    static set_thread_description_function function;

public:

    static bool do_it(char const* name) noexcept {
        load([]() { set_thread_description::function.load(); });
        return function(name);
    }
};

template<typename a_> once set_thread_description<a_>::load {};
template<typename a_> set_thread_description_function set_thread_description<a_>::function {};

inline bool thread_name(char const* name) noexcept {
    return set_thread_description<>::do_it(name);
}

}}
#endif
