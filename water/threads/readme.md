# water::threads

`water::threads` contains functions to run threads, and synchronization objects: mutexes, condition
variables, read write locks, semaphores, barriers and call-once.

One part is a thin layer over posix and windows threads and synchronization objects, another part is
synchronization objects implemented here. The system synchronization objects are preferred over the
water variants, most of the time.

Define `WATER_THREADS_STATISTICS` to collect statistics from the synchronization objects. This makes
it possible to see how the program uses each synchronization objects, if threads are waiting for a
specific mutex frequently.

Some include files in `water/threads` have pseudo code for documentation, then they include a system
specific file with the actual code.


## Synchronization objects

These are:

- `barrier<...>`
- `condition<...>`
- `mutex<...>`
- `read_write<...>`
- `semaphore<....>`

Include the header with the same name, `<water/threads/mutex.hpp>` for `mutex`

These are not classes, but templates that select a variant based on a list of needs. You specify
what you need, the template will select something that fulfills those needs or void if it cannot.

Example, `mutex`:

	mutex<> one; // no special needs
	mutex<need_timeout> two; // has timed waits
	mutex<need_constexpr_constructor, need_recursive> three; // recursive mutex with constexpr construction

The variants that can selected can be divided into 3 groups

- system variants, for example `pthred_mutex` or windows `CRITICAL_SECTION`
- water variants, made with system wait objects like mach semaphores, futex, windows events
- water spin variants, made with atomic operations + yield

The needs are

- `need_system` = selects a system variant                
- `need_water` = selects a water variant (not a spin variant)
- `need_timeout` = timed waits
- `need_recursive` = only for mutex 
- `need_constexpr_constructor`
- `need_trivial_destructor`
- `need_spin` = selects a water spin variant 

Define `WATER_THREADS_PREFER_WATER` to prefer water variants over system variants.

A condition variable might not work with any mutex. Use the `mutex_for_condition` template to make a
mutex that can be used with the condition variable:

	condition<need_water> c;
	mutex_for_condition<condition<need_water>> m;
	lock(m);
	wait(c, m);
	unlock(m);

The spin variants are not system specific, the other variants are system specific.
Spin variants are never selected unless `need_spin` is specified.

## Statistics

When `WATER_THREADS_STATISTICS` is defined these things generate statistics

- `barrier`
- `mutex`
- `read_write`
- `semaphore`

The statistics are simple counters when lock/wait/unlock succeeded at once or when the thread had
to wait or make system calls. This can help to find what parts of a program that have room for
improvement, where threads are waiting for each other. 

Nothing is collected for condition variables, because you know before waiting on a condition
variable that a thread cannot make progress.

A `threads::statistics::data` object is created for every synchronization object. This list of data is
never deallocated, it is intentionally leaked because it is impossible to know when it is safe to
free the memory.

Water variants collect more statistics that system variants. The system variants collect them by
replacing mutex lock with try_lock + lock.

Look at:

- `threads/statistics/data.hpp`
- `threads/statistics/statistics.hpp`
- `threads/statistics/out.hpp`

`___water_threads_statistics(code)` is defined to code only when `WATER_THREADS_STATISTICS` is defined

You can assign a name to each synchronization object so it will be easier to find in the generated
data. Use the `name_if` + `statistics_pointer` functions:

	mutex<> m;
	semaphore<> s;
	___water_threads_statistics(name_if(statistics_pointer(m), "hello");)
	___water_threads_statistics(name_if(statistics_pointer(s)) << "hello" << 123;)

The second `name_if` is a `water::ministr::out`.


## Running threads

In `water::threads` a thread is represented by a simple type `thread_t` or `join_t`. A thread is started with the `run` function. This is a low level, minimal layer over posix `pthread_create` and windows `CreateThread`/`_beginthreadex`.

	auto function = [] { do_something(); };
	water::threads::join_t thread;
	bool success = water::threads::run(function, thread);
	if(success)
		join(thread);

In this example, notice that `function` is **not** copied. Make sure that the function object passed to threads::run exists as long as the thread is running. For this reason, you usually want a `water::threads::join_t` variable that you can `join` later to know when the thread has finished (There are `run` overloads without a `join_t`).

You can set priority, stack size and the Apple specific "quality of service" (that can be quite important on iOS) on a thread. If the operating system does not let you set the priority, stack_size or quality of service those options are ignored. This example shows how to use `run` with a regular function plus a pointer:

	some_type *pointer_to_something = new some_type;
	void function(some_type*);
	
	water::threads::priority priority;
	unsigned average_priority = priority.min() + (priority.max() - priority.min()) / 2;
	
	water::threads::run_options options;
	options.priority(average_priority);
	options.stack_size(1024 * 1024);
	options.qos(water::threads::qos_user_interactive);
	
	water::threads::join_t thread;
	bool success = water::threads::run<water::threads::function<some_type, &function>>(
		pointer_to_something,
		join,
		options
		);
	if(success) {
		join(thread);
		delete pointer_to_something;
		}

See

- `<water/threads/thread.hpp>`
- `<water/threads/sleep.hpp>`
- `<water/threads/yield.hpp>`




## Call once

- `once`
- `spin_once`


## Errors and exceptions

Nothing here throws exceptions. Many things can fail, and will return bool false or 0 when if it
happens.

`panic()` is called when something really bad happens that is hard to detect or handle. It does nothing
unless you define `WATER_THREADS_PANIC` to do something. Terminate or throw an exception.

`panic()` can be called when a mutex lock or unlock fails. It is really hard to do something useful
after a mutex unlock fails. A few mutex constructors can also panic. Search the code for "panic" to
see where it can happen.

The barriers and semaphores convert to false if construction failed.

These things are expected to never fail

- `mutex` construction (most of them are constexpr)
- `read_write` construction (most of them are constexpr)
- `condition` construction (most of them are constexpr)
- `mutex/read_write` lock
- `mutex/read_write` unlock

## History

Much of the code here was originally created to work on Windows 2000 and Windows XP that do not have read write locks or condition variables. It was very useful for writing multithreaded code that worked across linux, mac and windows. Since then the C++ standard library has added thread, mutex, condition_variable and you might prefer to use those classes.

