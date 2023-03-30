// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_INITIALIZER_LIST_HPP
#define WATER_INITIALIZER_LIST_HPP
#include <water/water.hpp>
#ifdef WATER_NO_STD

#include <water/no_initializer_list.hpp>
namespace water {

    template<typename type_>
    using initializer_list = no_initializer_list<type_>;

    template<typename type_>
    using initializer_list_std_or_not_empty = no_initializer_list_not_empty<type_>;

}

#else

#include <initializer_list>
namespace water {

    using ::std::initializer_list;

    template<typename type_>
    using initializer_list_std_or_not_empty = ::std::initializer_list<type_>;

}

#endif
#endif
