// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TEMPORARY_BLOCK_HPP
#define WATER_TEMPORARY_BLOCK_HPP
#include <water/temporary/bits.hpp>
namespace water { namespace temporary {

/*

This file contains low level allocation functions, the block structure and this description of how
it works:

The block struct looks like this

struct block {
    block *next;
    byte *begin;
    byte *free;
    size_t retain;
};


In memory a block looks like this:

  [...alloctated.space...]...free.space...[block]
  ^                       ^               ^
  |                       |               |
  begin                   free            this

Block is at the end of the memory block, and block->begin points to where the memory block (and
allocations) begin.

When something is allocated, the block->free pointer is pushed up to where the alloction ends, the
beginning of the free space. When the block->free pointer points to the block there is no free
space, the block is full.

block->retain (size_t) is the retain count (reference count) of the block. Allocation increments the
retain count.

Memory is "deallocated" by decrementing the retain count. When the retain count is 0 the block is
unused. Where the free pointer points does not matter. The next allocation can reuse the block as if
the free-pointer pointed to the beginning.

If things are deallocated in the reverse order of allocation, it is possible to pop the free-pointer
and make the space avalible for allocation again before the retain count reaches 0.

This means a block is some kind of reference-counted stack, and is only useful if things are
deallocated...
- in the reverse order of allocation, like a stack, or..
- after a short time

A block is also a circular list. the block->next pointer points to the next block in the list (or to
itself if the list has only one block)

To allocate inside a block, use
- temporary::push (allocate memory)
- temporary::push_at_least (allocate as much memory as possible)
- temporary::pop (decrement retain count and pop free pointer)
- temporary::release (decrement retain count)
- temporary::retain (increment retain count)

To allocate or free the block itself, use
- temporary::allocate_block
- temporary::free_block
- temporary::free_all_blocks

water::temporary functions are low level things meant to be used as building blocks for something
more useful.

When an object allocated in a block has no destructor, the blocks retain count can be used as a
retain count for the object. It is also possible to use the object when the retain count is 0. The
memory is overwritten the next time something is allocated in the block, but if temporary::retain is
used to increment the count again before that, the next allocation will not overwrite it.

*/

struct block {
    block *next;
    byte *begin;
    byte *free;
    size_t retain;
    
    size_t memory_use() const noexcept {
        return static_cast<size_t>(static_cast<byte const*>(static_cast<void const*>(this + 1)) - begin);
    }
    size_t capacity() const noexcept {
        return static_cast<size_t>(static_cast<byte const*>(static_cast<void const*>(this)) - begin);
    }
};

template<typename allocator_, typename sizer_>
block* allocate_block(allocator_* allocator, block* list, size_t bytes, sizer_ const& sizer) noexcept(allocator_noexcept<allocator_>()) {
    // allocate a block with at least bytes free space.
    // this will allocate a larger block than any of those in the list now.
    //
    // returns
    // - pointer to the new block
    // - 0 or throws exception if allocator->allocate throws
    //
    // pre
    // - list and/or bytes can be 0
    // - allocator can be 0 (this returns 0)
    //
    // post (if it succeeded)
    // - list != 0, the returned block is in the list
    // - list == 0, the returned block is a list with only one block
    //
    // exceptions
    // - this will only throw exceptions if allocator->allocate() throws
    //
    if(allocator) {
        size_t constexpr align = alignof(block);
        size_t largest = 0;
        if(block *l = list)
            do {
                size_t c = l->capacity();
                if(largest < c) largest = c;
                l = l->next;
            } while(l != list);
        if(!bytes)
            bytes = align;
        if(bytes <= largest)
            bytes = largest + 1;
        else if(size_t a = bytes % align)
            bytes += align - a;
        size_t size = sizer(bytes += sizeof(block)); // let sizer decide the size of the allocation, not just the free space
        ___water_assert(size >= bytes && "sizer returned something to small");
        if(size >= bytes) {
            size -= sizeof(block);
            if(size_t a = size % align)
                size += align - a;
            if(byte *m = static_cast<byte*>(allocator->allocate(size + sizeof(block)))) {
                ___water_assert(!(reinterpret_cast<upointer_t>(m) % align) && "allocator bad align");
                block *b = static_cast<block*>(static_cast<void*>(m + size));
                if(list) {
                    b->next = list->next;
                    list->next = b;
                }
                else
                    b->next = b;
                b->begin = b->free = m;
                b->retain = 0;
                return b;
            }
        }
    }
    return 0;
}

template<typename allocator_>
block* free_block(allocator_* allocator, block* list) noexcept {
    // remove the block list points to from the list, then free it
    //
    // returns
    // - 0 if list is empty now
    // - otherwise pointer to another block in the list
    //
    // this returns list, and does nothing else, if list == 0 or allocator == 0
    //
    if(allocator && list) {
        block *f = list;
        do list = list->next; while(list->next != f);
        list->next = f->next;
        allocator->free(f->begin, f->memory_use());
        if(list == f) list = 0;
    }
    return list;
}

template<typename allocator_>
void free_all_blocks(allocator_* allocator, block* list) noexcept {
    // free all blocks in list
    //
    // this does nothing if list == 0 or allocator == 0
    //
    if(allocator && list) {
        block *e = list;
        do {
            block *f = list;
            list = list->next;
            allocator->free(f->begin, f->memory_use());
        } while(list != e);
    }
}

template<typename allocator_>
block* free_unused_blocks(allocator_* allocator, block* list) noexcept {
    // free all unused blocks in list
    //
    // this returns list, and does nothing else, if list == 0 or allocator == 0
    //
    if(allocator && list) {
        block
            *p = list,
            *n = list->next;
        while(n != list) {
            if(!n->retain) { // free except list
                p->next = n->next;
                allocator->free(n->begin, n->memory_use());
                n = p;
            }
            n = (p = n)->next;
        }
        if(!list->retain) { // free list
            n = p->next = list->next;
            allocator->free(list->begin, list->memory_use());
            list = (n == list) ? 0 : n;
        }
    }
    return list;
}

template<typename allocator_>
block* free_unused_blocks(allocator_* allocator, block* list, unsigned keep) noexcept {
    // free unused blocks in list, but keep at least keep number of blocks.
    // this will free the smallest unused blocks, although the blocks in use could be smaller.
    // keep == 0 means remove all unused blocks
    //
    // this returns list, and does nothing else, if list == 0 or allocator == 0
    //
    if(allocator && list)
        while(true) {
            block
                *f = 0,
                *ff = 0,
                *b = 0,
                *bb = list;
            unsigned
                count = 0,
                unused = 0;
            size_t min = static_cast<size_t>(-1);
            do {
                ++count;
                b = bb->next;
                if(!b->retain) {
                    ++unused;
                    size_t c = b->capacity();
                    if(min > c) {
                        min = c;
                        f = b;
                        ff = bb;
                    }
                }
            } while((bb = b) != list);
            if(f && count > keep) {
                ff->next = f->next;
                allocator->free(f->begin, f->memory_use());
                if(--count)
                    list = ff;
                else
                    list = 0;
            }
            if(!f || unused == 1 || count <= keep)
                break;
        }
    return list;
}

template<typename sizer_>
block* allocate_block(void*, block*, size_t, sizer_ const&) noexcept {
    return 0;
}

inline block* free_block(void*, block* list) noexcept {
    return list;
}

inline void free_all_blocks(void*, block*) noexcept {
}

inline block* free_unused_blocks(void*, block* list, unsigned = 0) noexcept {
    return list;
}

inline block* block_from_memory(block* list, void* memory, size_t bytes) noexcept {
    // create a new block in memory
    //
    // returns
    // - pointer to the block if it succeeded
    // - 0 if memory == 0, or bytes was too small
    //
    // post (if it return != 0)
    // - returned block->begin == memory
    // - list != 0, the returned block is in the list
    // - list == 0, the returned block is a list with only one block
    //
    // the block struct is placed at the end of memory, aligned to
    // alignof(block). this will succeed if the block struct
    // fits and at least 1 byte is avalible for allocation.
    //
    // to not waste memory, make sure memory + bytes - sizeof(block)
    // is aligned to alignof(block)
    //
    // this can be used to make a block-list from memory allocated manually.
    // the blocks must be freed manually. pass a void* to the functions that
    // have an allocator argument
    if(memory && bytes > sizeof(block)) {
        byte *m = static_cast<byte*>(memory);
        bytes -= sizeof(block);
        size_t a = static_cast<size_t>(reinterpret_cast<upointer_t>(m + bytes) % alignof(block));
        if(bytes > a) {
            block *b = static_cast<block*>(static_cast<void*>(m + (bytes -= a)));
            if(list) {
                b->next = list->next;
                list->next = b;
            }
            else
                b->next = b;
            b->begin = b->free = m;
            b->retain = 0;
            return b;
        }
    }
    return 0;
}

inline block* find(block* list, void const* pointer) noexcept {
    // returns block containing pointer, or 0 if it was not found or if list is 0
    if(block *e = list)
        do {
            if(list->begin <= pointer && pointer < list) return list;
        } while((list = list->next) != e);
    return 0;
}

inline block* find_smallest_unused(block* list) noexcept {
    // return the smallest unused block, 0 if all blocks are used or if list is 0.
    block *r = 0;
    if(block *e = list) {
        size_t x, s = static_cast<size_t>(-1);
        do {
            if(!list->retain && (x = static_cast<size_t>(static_cast<byte*>(static_cast<void*>(list)) - list->begin)) < s) {
                s = x;
                r = list;
            }
        } while((list = list->next) != e);
    }
    return r;
}

inline size_t release(block*& list, void const* pointer) noexcept {
    // decrement the retain count of the block containing pointer, if
    // the count is not 0
    //
    // return
    // - the old retain count (it could be 0)
    // - 0 if list == 0 or pointer is not inside a block
    //
    // pre
    // - list and/or pointer can be 0
    //
    // post
    // - if pointer was in a block, list == that block
    //
    block *f = find(list, pointer);
    if(f && (list = f)->retain) return list->retain--;
    return 0;
}

inline size_t release(temporary::block* block) noexcept {
    // decrement the retain count of block, if the count is not 0
    //
    // return
    // - the old retain count (it could be 0)
    // - 0 if block == 0
    //
    return block && block->retain ? block->retain-- : 0;
}

inline size_t retain(block*& list, void const* pointer) noexcept {
    // increment the retain count of the block containing pointer
    //
    // return
    // - the new retain count
    // - 0 if list == 0 or pointer is not inside a block
    //
    // pre
    // - list and/or pointer can be 0
    //
    // post
    // - if pointer was in a block, list == that block
    //
    if(block *f = find(list, pointer)) return ++(list = f)->retain;
    return 0;
}

inline size_t retain(temporary::block* block) noexcept {
    // increment the retain count of block
    //
    // return
    // - the new retain count
    // - 0 if block == 0
    //
    return block ? ++block->retain : 0;
}

namespace _ {
    
    inline byte* push_align(byte* pointer, size_t align) {
        if(align > 1)
            if(upointer_t a = reinterpret_cast<upointer_t>(pointer) % align)
                pointer += align - a;
        return pointer;
    }
    
    inline void push_waste(block* list, byte *push) {
        if(list->free + waste <= push)
            list->free = push;
        else if(list->free + waste <= static_cast<void*>(list))
            list->free += waste;
        else
            list->free = static_cast<byte*>(static_cast<void*>(list));
    }

}

template<typename allocator_, typename sizer_>
void* push(block*& list, size_t bytes, size_t align, unsigned allocate_block_and_keep, allocator_* allocator, sizer_ const& sizer) noexcept(allocator_noexcept<allocator_>()) {
    // allocate bytes aligned to align
    //
    // if no block in the list has enough free space the allocate_block_and_keep
    // argument determines what happens. if it is:
    // - 0: return 0
    // - 1: allocate a new block and free any unused blocks
    // - 2: allocate a new block and free any unused blocks, keep 2 blocks
    // - n: allocate a new block and free any unused blocks, keep n blocks
    //
    // pre
    // - list can be 0
    // - bytes can be 0, but should not. if it is, the next alloction
    //   could return the same pointer
    // - align 0 will use align = temporary::waste
    //
    // post
    // - if it succeeded, list is the block of the returned pointer
    //
    // exceptions
    // - if allocator == 0 this will never throw
    // - if allocate_block_and_keep >= 1 the allocator can throw
    // - if allocate_block_and_keep == 0 this will never throw
    //
    ___water_assert(align <= waste);
    if(!align) align = waste;
    if(block *e = list) {
        byte *r;
        do {
            r = _::push_align(list->retain ? list->free : list->begin, align);
            if(r + bytes <= static_cast<void*>(list)) {
                _::push_waste(list, r + bytes); // list->free = r + bytes
                ++list->retain;
                return r;
            }
        } while((list = list->next) != e);
    }
    if(allocate_block_and_keep)
        if(block *b = allocate_block(allocator, list, bytes + (align ? align - 1 : 0), sizer)) { // throw maybe
            byte *r = _::push_align(b->begin, align);
            _::push_waste(b, r + bytes);
            b->retain = 1;
            free_unused_blocks(allocator, list, allocate_block_and_keep);
            list = b;
            return r;
        }
    return 0;
}

template<typename allocator_, typename sizer_>
void* push_at_least(block*& list, size_t& pushed, size_t at_least, size_t multiply, size_t align, unsigned allocate_block_and_keep, allocator_* allocator, sizer_ const& sizer) noexcept(allocator_noexcept<allocator_>()) {
    // allocate as much memory as possible, at least at_least * multiply
    // bytes, aligned to align.
    //
    // multiply is usually the sizeof(type) that is being allocated.
    // multiply 0 is seen as 1, pushed and at_least are then bytes.
    //
    // if no block in the list has enough free space the allocate_block_and_keep
    // argument determines what happens. see temporary::push
    //
    // pre
    // - list can be 0
    // - at_least can be 0, it has the same effect as 1
    // - pushed is overwritten
    // - align 0 will use align = temporary::waste
    //
    // post
    // - pushed * multiply is the number of bytes actually allocated, 0
    //   if it failed, otherwise at least at_least * multiply
    // - if it succeeded, list is the block of the returned pointer
    //
    // exceptions
    // - if allocator == 0 this will never throw
    // - if allocate_block_and_keep >= 1 the allocator can throw
    // - if allocate_block_and_keep == 0 this will never throw
    //
    //
    // use temporary::resize to change the size of the allocation if it was too large.
    //
    ___water_assert(align <= waste);
    if(!align) align = waste;
    pushed = 0;
    if(!multiply) multiply = 1;
    at_least = at_least ? at_least * multiply : multiply;
    if(block *e = list) {
        block *i = list;
        ptrdiff_t s = 0;
        byte *r = 0, *a;
        do {
            a = _::push_align(i->retain ? i->free : i->begin, align);
            if((s = static_cast<byte*>(static_cast<void*>(i)) - a) > static_cast<ptrdiff_t>(pushed)) {
                list = i;
                r = a;
                pushed = static_cast<size_t>(s);
            }
        } while((i = i->next) != e);
        if(pushed >= at_least) {
            if(multiply > 1) {
                pushed /= multiply;
                _::push_waste(list, r + pushed * multiply);
            }
            ++list->retain;
            return r;
        }
        pushed = 0;
    }
    if(allocate_block_and_keep)
        if(block *b = allocate_block(allocator, list, at_least + (align ? align - 1 : 0), sizer)) { // throw maybe
            byte *r = _::push_align(b->begin, align);
            pushed = static_cast<size_t>(static_cast<byte*>(static_cast<void*>(b)) - r);
            if(multiply > 1) {
                pushed /= multiply;
                _::push_waste(b, r + pushed * multiply);
            }
            b->retain = 1;
            free_unused_blocks(allocator, list, allocate_block_and_keep);
            list = b;
            return r;
        }
    return 0;
}

inline void pop(block*& list, void const* pointer, size_t bytes) noexcept {
    // pop something retuned from temporary::push or temporary::push_at_least
    // - pointer is a pointer to the allocation
    // - bytes is the size of the allocation
    //
    // decrements the retain count of the block containing pointer,
    // and if it can sets block->free to pointer to make the space
    // avalible for allocation again.
    //
    // this does nothing if
    // - list or pointer is 0
    // - pointer was not found in the blocks
    //
    // post
    // - list is the block containing pointer, if it was found
    //
    // block->free can be set to pointer if block->free <= pointer + bytes + temporary::waste - 1
    //
    // if pointer is the last thing allocated in a block this will
    // always work. otherwise it may not always work unless all things
    // are pop()-ed in the reverse order of allocation (like a stack).
    //
    // temporary::push and temporary::push_at_least inserts padding to return
    // aligned memory, a hole between allocations. without temporary::waste
    // the hole can be a problem:
    // - block->free pointer has address 0x1000
    // - temporary::push allocates 1 byte with no aligment
    //   - block->free is pushed to 0x1001
    //   - temporary::push returns 0x1000
    // - temporary::push allocates 5 bytes aligned to 4
    //   - block->free is pushed to 0x1009
    //   - temporary::push returns 0x1004 (0x1001 aligned up to 4)
    //   - there is a 3-byte hole, 0x1001 to 0x1004
    // - temporary::pop(0x1004, 5)
    //   - block->free pointer is popped to 0x1004
    // - temporary::pop(0x1000, 1)
    //   - block->free pointer is not popped because 0x1000 + 1 is 0x1001
    //     and 0x1001 is less than 0x1004
    //
    // to avoid this problem temporary::waste is used. it is:
    // - the maximum alignment, the hole is always less than this
    // - the minimum number of bytes the free pointer can be pushed,
    //   an allocation is always larger than the largest possible hole
    //
    if(block *b = find(list, pointer))
        if(
            (list = b)->retain &&
            --list->retain &&
            list->free <= static_cast<byte const*>(pointer) + bytes + (waste - 1)
        )
            list->free = list->begin + (static_cast<byte const*>(pointer) - list->begin); // const cast
}

inline void pop(block*& list, void const* begin, void const* end) noexcept {
    // calls pop(list, begin, begin < end ? end - begin : 0)
    pop(list, begin, static_cast<size_t>(begin < end ? static_cast<byte const*>(end) - static_cast<byte const*>(begin) : 0));
}

inline void* resize(block*& list, void const* pointer, size_t old_size, size_t new_size) noexcept {
    // try to move the free-pointer in a block
    //
    // if a block containing pointer is found, and pointer + new_size
    // fits in that block this will succeed if:
    // - the retain count of that block is 1, or
    // - the space after pointer + old_size + temporary::waste - 1 is free
    //
    // this means it is always possible to resize the last allocation
    // made in a block, because space after the last is free.
    //
    // make sure the thing calling this holds a retain count on the
    // memory, otherwise bad things can happen if the retain count is
    // 1 but belongs to someone else.
    //
    // returns
    // - non-const pointer if it succeeded
    // - else 0
    //
    // pre
    // - if list or pointer is 0, this returns 0
    // - avoid new_size == 0 if temporary::pop is used to free the memory
    //
    // post
    // - if pointer was found in a block, list points to that block
    //
    if(block *e = list)
        do {
            if(list->begin <= pointer && pointer <= list) { // different from find() pointer < list
                byte const *p = static_cast<byte const*>(pointer);
                if(
                    (p + new_size <= static_cast<void const*>(list)) &&
                    (list->retain == 1 || list->free <= p + old_size + (waste - 1)) // this is the last allocation
                ) {
                    byte *r = list->begin + (p - list->begin); // const cast
                    size_t min_size = waste < old_size ? waste : old_size; // the minimum size is the smaller of waste and old_size
                    list->free = r + (new_size > min_size ? new_size : min_size);
                    return r;
                }
                return 0;
            }
        } while((list = list->next) != e);
    return 0;
}


}}
#endif
