// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_WATER_HPP
#define WATER_WATER_HPP
#include <water/configuration.hpp>
#include <water/macros.hpp>
namespace water {

using size_t = decltype(sizeof(0));
using ptrdiff_t = decltype(static_cast<int*>(0) - static_cast<int*>(0));

}
#endif
