// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_THREADS_WINDOWS_SRWLOCK_HPP
#define WATER_THREADS_WINDOWS_SRWLOCK_HPP
#include <water/threads/windows/bits.hpp>
namespace water { namespace threads {

inline void lock(srwlock_t& a) noexcept {
    AcquireSRWLockExclusive(&a);
}

inline bool try_lock(srwlock_t& a) noexcept {
    return TryAcquireSRWLockExclusive(&a) != 0;
}

inline void unlock(srwlock_t& a) noexcept {
    ReleaseSRWLockExclusive(&a);
}

inline void read_lock(srwlock_t& a) noexcept {
    AcquireSRWLockShared(&a);
}

inline bool read_try_lock(srwlock_t& a) noexcept {
    return TryAcquireSRWLockShared(&a) != 0;
}

inline void read_unlock(srwlock_t& a) noexcept {
    ReleaseSRWLockShared(&a);
}

class srwlock
{
public:
    using needs = threads::needs<need_system, need_constexpr_constructor, need_trivial_destructor>;

private:
    srwlock_t my{};
    ___water_threads_statistics(threads::statistics::reference mystatistics;)

public:
    constexpr srwlock() = default;
    srwlock(srwlock const&) = delete;
    srwlock& operator=(srwlock const&) = delete;

    void lock() noexcept {
        ___water_threads_statistics(if(try_lock()) return;)
        threads::lock(my);
    }

    bool try_lock() noexcept {
        bool r = threads::try_lock(my);
        ___water_threads_statistics(threads::statistics::add(mystatistics, this, "srwlock").wait(r));
        return r;
    }

    void unlock() noexcept {
        threads::unlock(my);
    }

    void read_lock() noexcept {
        ___water_threads_statistics(if(read_try_lock()) return;)
        threads::read_lock(my);
    }

    bool read_try_lock() noexcept {
        bool r = threads::read_try_lock(my);
        ___water_threads_statistics(threads::statistics::add(mystatistics, this, "srwlock").wait(r));
        return r;
    }

    void read_unlock() noexcept {
        threads::read_unlock(my);
    }

    srwlock_t& underlying() noexcept {
        return my;
    }

    ___water_threads_statistics(threads::statistics::data* statistics() noexcept { return get(mystatistics, this, "srwlock"); })
};

}}
#endif
