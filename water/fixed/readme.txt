// fixed::memory_atomic
////////////////////////////////////////////////////////////////////////////////////////////////////

water::fixed::memory_atomic is a mostly lock-free fixed size memory allocator.
Useful as an allocator for elements used in lock free data structures.

template<typename allocator_ = void, bool statistics_ = false> class memory_atomic:

template
- allocator_ is the underlying allocator. It must be like water::allocator_nothrow (should not throw).
  void, the default, will use water::allocator_nothrow.
- statistics_ true enables statistics, for tuning during development

It works by allocating large blocks of memory from the underlying allocator, then splits that into
fixed size allocations. As long as it has free space in one of the large blocks, allocation is lock
free. When it needs to allocate a new large block it uses a lock and allocates a new block from the
underlying allocator.

You can manually allocate large blocks beforehand and use it as a 100% lock free allocator that will
fail if the large blocks are full.

It will never free memory (except when its destroyed) so it is always safe to read freed memory.

// Example
////////////////////////////////////////////////////////////////////////////////////////////////////

water::fixed::memory_atomic<> my_memory{128}; // allocator for 128 byte allocations
my_memory.block_size(512); // optional, set the size this allocates from the underlying allocator in number-of-allocations. A block is 512 * 128 bytes here
my_memory.allocate_block(); // optional, pre-allocate a block. Returns false if the underlying allocator returned 0

// make an allocator for my_memory that throw exceptions on out of memory,
// and that will my_memory.allocate_block() if my_memory is out of memory (this is the only operation that is not lock free)

auto allocator = allocator_for(my_memory);

void *pointer = allocator.allocate(128);
allocator.free(pointer, 128);

pointer = allocator.allocate(64); // this still allocates 128 bytes
allocator.free(pointer, 64);

try {
	pointer = allocate.allocate(1000); // will throw, more than 128 bytes
	}
catch(water::fixed::exception&) {
	// whoops
	}

// make a 100% lock free allocator for my_memory.
// You must my_memory.allocate_block() before you use this, it will only allocate in existing memory.

auto lock_free_allocator = allocator_lock_free_for(my_memory);
try {
	pointer = lock_free_allocator.allocate(128); // throws
	lock_free_allocator.free(pointer);
	}
catch(water::fixed::exception&) {
	}

// make a 100% lock free allocatator that returns 0 if its out of memory

auto lock_free_nothrow = allocator_nothrow_lock_free_for(my_memory);
pointer = lock_free_nothrow.allocate(128);
if(pointer)
	lock_free_nothrow.free(pointer);
	