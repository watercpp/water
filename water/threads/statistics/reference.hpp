// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_STATISTICS_REFERENCE_HPP
#define WATER_THREADS_STATISTICS_REFERENCE_HPP
#include <water/threads/statistics/data.hpp>
namespace water { namespace threads { namespace statistics {

class reference
{
    atomic<data*> my{};

public:
    constexpr reference() noexcept = default;
    reference(reference const&) = delete;
    reference& operator=(reference const&) = delete;

    data* get() const noexcept {
        return my.load(memory_order_relaxed);
    }

    void set(data *a) noexcept {
        my.store(a, memory_order_relaxed);
    }
};

}}}
#endif
