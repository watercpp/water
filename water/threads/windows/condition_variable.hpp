// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_WINDOWS_CONDITION_VARIABLE_HPP
#define WATER_THREADS_WINDOWS_CONDITION_VARIABLE_HPP
#include <water/threads/windows/critical_section.hpp>
#include <water/threads/windows/srwlock.hpp>
#include <water/threads/windows/deadline.hpp>
namespace water { namespace threads {

inline bool wait(condition_variable_t& c, critical_section_t& l) noexcept {
    return SleepConditionVariableCS(&c, &l, wait_forever) != 0;
}

inline bool wait(condition_variable_t& c, critical_section_t& l, double seconds) noexcept {
    return SleepConditionVariableCS(&c, &l, milli_from_seconds(seconds)) != 0;
}

inline bool wait(condition_variable_t& c, srwlock_t& l, double seconds) noexcept {
    return SleepConditionVariableSRW(&c, &l, milli_from_seconds(seconds), 0) != 0;
}


inline bool wait(condition_variable_t& c, srwlock_t& l) noexcept {
    return SleepConditionVariableSRW(&c, &l, wait_forever, 0) != 0;
}

inline bool wait(condition_variable_t& c, critical_section_t& l, deadline d) noexcept {
    return SleepConditionVariableCS(&c, &l, d.milli_left()) != 0;
}

inline bool wait(condition_variable_t& c, srwlock_t& l, deadline d) noexcept {
    return SleepConditionVariableSRW(&c, &l, d.milli_left(), 0) != 0;
}


inline bool wake(condition_variable_t& c) noexcept {
    WakeConditionVariable(&c);
    return true;
}

inline bool wake_all(condition_variable_t& c) noexcept {
    WakeAllConditionVariable(&c);
    return true;
}

class condition_variable
{
public:
    using needs = threads::needs<need_system, need_constexpr_constructor, need_trivial_destructor, need_timeout>;
    using mutex =
        #ifdef WATER_THREADS_MUTEX_PREFER_WINDOWS_CRITICAL_SECTION
        critical_section;
        #else
        srwlock;
        #endif

private:
    condition_variable_t my{};

public:
    constexpr condition_variable() noexcept = default;
    condition_variable(condition_variable const&) = delete;
    condition_variable& operator=(condition_variable const&) = delete;

    bool wait(critical_section_t& lock) noexcept {
        return threads::wait(my, lock);
    }

    bool wait(critical_section& lock) noexcept {
        return threads::wait(my, lock.underlying());
    }

    bool wait(srwlock_t& lock) noexcept {
        return threads::wait(my, lock);
    }

    bool wait(srwlock& lock) noexcept {
        return threads::wait(my, lock.underlying());
    }

    bool wait(critical_section_t& lock, double seconds) noexcept {
        return threads::wait(my, lock, seconds);
    }

    bool wait(critical_section& lock, double seconds) noexcept {
        return threads::wait(my, lock.underlying(), seconds);
    }

    bool wait(srwlock_t& lock, double seconds) noexcept {
        return threads::wait(my, lock, seconds);
    }

    bool wait(srwlock& lock, double seconds) noexcept {
        return threads::wait(my, lock.underlying(), seconds);
    }

    bool wait(critical_section_t& lock, deadline d) noexcept {
        return threads::wait(my, lock, d);
    }

    bool wait(critical_section& lock, deadline d) noexcept {
        return threads::wait(my, lock.underlying(), d);
    }

    bool wait(srwlock_t& lock, deadline d) noexcept {
        return threads::wait(my, lock, d);
    }

    bool wait(srwlock& lock, deadline d) noexcept {
        return threads::wait(my, lock.underlying(), d);
    }

    bool wake() noexcept {
        return threads::wake(my);
    }

    bool wake_all() noexcept {
        return threads::wake_all(my);
    }

    condition_variable_t& underlying() noexcept {
        return my;
    }
};

}}
#endif
