// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPE_NAME_HPP
#define WATER_TYPE_NAME_HPP
#if !defined(WATER_NO_RTTI) && (defined(WATER_TYPE_NAME_RTTI_ONLY) || !(defined(WATER_COMPILER_CLANG) || defined(WATER_COMPILER_GCC) || defined(WATER_COMPILER_MICROSOFT)))
    #include <typeinfo>
#endif
namespace water {

/*

name of a type as a string

*/

namespace _ {
    
    #if !defined(WATER_STR_NAMEOF_RTTI_ONLY) && (defined(WATER_COMPILER_CLANG) || defined(WATER_COMPILER_GCC) || defined(WATER_COMPILER_MICROSOFT))
    
    template<typename type_>
    char* type_name_copy(char *tb, char *te, unsigned& pos) {
        // gcc         __PRETTY_FUNCTION__ char* typename_cpy(...) [with type_ = some_type ]
        // visual c++  __FUNCSIG__         char* typename_cpy<some_type>(...)
        char const
            *fb =
                #ifdef WATER_COMPILER_MICROSOFT
                __FUNCSIG__
                #else
                __PRETTY_FUNCTION__
                #endif
                ,
            *fe = fb,
            *f = fb;
        char *t = tb;
        unsigned p = pos;
        pos = 0;
        if(f && *f) {
            while(*fe) ++fe;
            #ifdef WATER_COMPILER_MICROSOFT
            while(f != fe && *--fe != '>');
            if(p) f += p;
            else while(f != fe && *f++ != '<');
            #else //gcc
            while(f != fe && *--fe != ']');
            while(f != fe && *fe == ' ') --fe;
            if(p) f += p;
            else {
                while(f != fe && *f++ != '=');
                while(f != fe && *f == ' ') ++f;
            }
            #endif
            if(f < fe) {
                do { *t = *f++; } while(++t != te && f != fe);
                if(f != fe) pos = static_cast<unsigned>(f - fb);
            }
        }
        return t;
    }
    
    template<typename type_, typename function_>
    void type_name_do(function_& function) {
        char c[256], *e;
        unsigned p = 0;
        bool any = false;
        do {
            e = type_name_copy<type_>(c, c + sizeof(c) - 1, p);
            if(e != c) {
                *e = 0;
                function(c + 0, e);
                any = true;
            }
        } while(p);
        if(!any) {
            char const *b = "type name error";
            function(b, b + sizeof("type name error") - 1);
        }
    }
    
    #elif defined(WATER_NO_RTTI)
    
    template<typename type_, typename function_>
    void type_name_do(function_& function) {
        char const *b = "unknown";
        function(b, b + sizeof("unknown") - 1);
    }
    
    #else
    
    template<typename type_, typename function_>
    void type_name_do(function_& function) {
        char const
            *b = typeid(type_).name(),
            *e = b;
        if(e) while(*e) ++e;
        if(b == e) {
            b = "typeid error";
            e = b + sizeof("typeid error") - 1;
        }
        function(b, e);
    }
    
    #endif

}

template<typename type_, typename function_>
void type_name_to(function_&& function) {
    // can call function more than once
    // function(char const* begin, char const *end)
    // *end = 0 always, begin != end always
    _::type_name_do<type_>(function);
}

}
#endif
