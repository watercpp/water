// threads
///////////////////////////////////////////////////////////////////////////////////////////////////

water::threads contains functions to run threads, and synchronizaton objects: mutexes, condition
variables, read write locks, semaphores, barriers and call-once.

One part is a thin layer over posix and windows threads and synchronizaton objects, another part is
synchronizaton objects implemented here. The system synchronizaton objects are prefered over the
water variants, most of the time.

Define WATER_THREADS_STATISTICS to collect statistics from the synchronizaton objects. This makes
it possible to see how the program uses each synchronizaton objects, if threads are waiting for a
specific mutex fequently.

Some include files in water/threads have pseudo code for documentation, then they include a system
specific file with the actual code.


// Running threads
///////////////////////////////////////////////////////////////////////////////////////////////////

See
<water/threads/thread.hpp>
<water/threads/sleep.hpp>
<water/threads/yield.hpp>


// Synchronizaton objects
///////////////////////////////////////////////////////////////////////////////////////////////////

These are:
- barrier<...>
- condition<...>
- mutex<...>
- read_write<...>
- semaphore<....>

Include the header with the same name, <water/threads/mutex.hpp> for mutex

These are not classes, but templates that select a variant based on a list of needs. You specify
what you need, the template will select something that fulfills those needs or void if it cannot.

Example, mutex:

mutex<> one; // no special needs
mutex<need_timeout> two; // has timed waits
mutex<need_constexpr_constructor, need_recursive> three; // recursive mutex with constexpr construction

The variants that can selected can be divided into 3 groups
- system variants, for example pthred_mutex windows CRITICAL_SECTION
- water variants, made with system wait objects like mach semaphores, futex, windows events
- water spin variants, made with atomic operations + yield

The needs are
- need_system = selects a system variant                
- need_water = selects a water variant (not a spin variant)
- need_timeout = timed waits
- need_recursive = only for mutex 
- need_constexpr_constructor
- need_trivial_destructor
- need_spin = selects a water spin variant 

Define WATER_THREADS_PREFER_WATER to prefer water variants over system variants

A condition variable might not work with any mutex. Use the mutex_for_condition template to make a
mutex that can be used with the condition variable:

condition<need_water> c;
mutex_for_condition<condition<need_water>> m;
lock(m);
wait(c, m);
unlock(m);

The spin variants are not system specific, the other variants are system specific.
Spin variants are never selected unless need_spin is specified.

// Statistics
///////////////////////////////////////////////////////////////////////////////////////////////////

When WATER_THREADS_STATISTICS is defined these things generate statistics
- barrier
- mutex
- read_write
- semaphore

The statistcs are simple counters when lock/wait/unlock succeeded at once or when the thread had
to wait or make system calls. This can help to find what parts of a program that have room for
imporvement, where threads are waiting for each other. 

Nothing is collected for condition variables, because you know before waiting on a condition
variable that a thread cannot make progress.

A threads::statistics::data object is created for every synchronizaton object. This list of data is
never deallocated, it is intentinally leaked beceause it is impossible to know when it is safe to
free the memory.

Water variants collect more statistcs that system variants. The system variants collect them by
replacing mutex lock with try_lock + lock.

Look at:
threads/statistics/data.hpp
threads/statistics/statistics.hpp
threads/statistics/out.hpp

___water_threads_statistics(code) is defined to code only when WATER_THREADS_STATISTICS is defined

You can assign a name to each synchronizaton object so it will be easier to find in the generated
data. Use the name_if + statistics_pointer functions:

mutex<> m;
semaphore<> s;
___water_threads_statistics(name_if(statistics_pointer(m), "hello");)
___water_threads_statistics(name_if(statistics_pointer(s)) << "hello" << 123;)

The second name_if is a ministr.


// Call once
///////////////////////////////////////////////////////////////////////////////////////////////////

once
spin_once


// Errors and exceptions
///////////////////////////////////////////////////////////////////////////////////////////////////

Nothing here throws exceptions. Many things can fail, and will return bool false or 0 when if it
happens.

panic() is called when something really bad happens that is hard to detect or handle. It does nothing
unless you define WATER_THREADS_PANIC to do something. Terminate or throw an exception.

panic() can be called when a mutex lock or unlock fails. It is really hard to do something useful
after a mutex unlock fails. A few mutex constructors can also panic. Search the code for "panic" to
see where it can happen.

The barriers and semaphores convert to false if construction failed.

These things are expected to never fail
- mutex construction (most of them are constexpr)
- read_write construction (most of them are constexpr)
- condition construction (most of them are constexpr)
- mutex/read_write lock
- mutex/read_write unlock
