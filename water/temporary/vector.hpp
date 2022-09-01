// Copyright 2017-2022 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TEMPORARY_VECTOR_HPP
#define WATER_TEMPORARY_VECTOR_HPP
#include <water/temporary/allocator.hpp>
#include <water/iterator.hpp>
#include <water/reverse_iterator.hpp>
#include <water/type_traits.hpp>
#include <water/vectool.hpp>
#include <water/swap.hpp>
namespace water { namespace temporary {

struct vector_sizer {
    size_t operator()(size_t a) const {
        return a + 4;
    }
};

/*

temporary::vector is a container similar to std::vector except a temporary::allocator is used to
allocate memory. It can be useful when something similar to std::vector is needed for a short time.

This holds a temporary::allocator. The allocator must be passed to the constructor. This is the main
difference to a std::vector that holds a copy of a std::allocator. Other than that the interface is
almost excactly like std::vector.

template
- value_ is the value_type
- allocator_ should be a temporary::allocator
- sizer_ see below

Example:

  temporary::memory<> mymemory;
  allocator = allocator_for(mymemory);
  temporary::vector<some_type, decltype(allocator)> v{allocator};
  v.push_back(some_type{});

temporary::allocator allocation can fail by throwing or returning 0. To make it possible to see
return-0-failures functions in this also returns 0. For example push_back() returns an iterator (a
pointer) that can be 0.

The constructors and copy-assignment operator cannot return 0, if the allocator returned 0, empty() == true

Differences from std::vector
- The constructors need an allocator
- at(size_type i) does not throw if i >= size()
- clear() deallocates memory
- get_allocator() does not exist, this has allocator() instead
- reserve(max_size() + 1) does not throw
- max_size() always returns size_type_max / sizeof(value_type)
- vector operators < > <= >= need value_type operators == and <
- vector<bool> is not specialized

This has constructor, assign and insert functions for begin-size ranges, std::vector does not:

   input_iterator begin;
   size_t size;
   ...
   vector<value, allocator_type> v(begin, size, allocator);
   v.assign(begin, size);
   v.insert(v.end(), begin, size);

This also has a shrink() function that makes the allocated memory just large enough.

Exceptions
- If the value_type copy constructor or copy assignment operator throws from assign() erase() or
  insert() this can be in a half finished state just like std::vector.
- erase()-ing from the end can never throw or fail
- When insert()-ing at the end this will always be unchanged if something throws
- If something throws from anything other than assign() erase() or insert() this is unchanged

sizer_ is a function object that decides the amount of extra space this allocates in addition to
what it needs right now. A larger value can reduce the number of reallocations (it is slow). The
default, void, means use vector_sizer.

A sizer_ looks like this, and should return need_capacity or more:

  struct sizer {
    size_t operator()(size_t need_capacity);
    };

swap(sizer_&, sizer_&) and move constructor must work and never ever throw or fail.

This tries to temporary::resize it's memory before it reallocates. If sizer_ returns 0 it does not
mean every push_back reallocates. If only a few things use the temporary::allocator resizing is more
likley to succeed.

*/

template<typename value_, typename allocator_, typename sizer_ = void>
class vector
{
public:
    typedef value_
        value_type,
        *pointer,
        *iterator,
        &reference;
    typedef value_type const
        *const_pointer,
        *const_iterator,
        &const_reference;
    using reverse_iterator = water::reverse_iterator<iterator>;
    using const_reverse_iterator = water::reverse_iterator<const_iterator>;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using allocator_type = allocator_;
    using sizer_type = typename types::if_not_void<sizer_, vector_sizer>::result;

private:
    typedef vectool<value_type> tool;

private:
    allocator_type myallocator{};
    iterator
        mybegin = 0, // begin
        myend = 0, // end of values
        mystop = 0; // end of memory
    sizer_type mysizer{};

public:
    vector() = default;

    explicit vector(allocator_type const& a, sizer_type const& s = sizer_type()) noexcept :
        myallocator(a),
        mysizer(s)
    {}

    explicit vector(sizer_type const& s, allocator_type const& a) noexcept :
        myallocator(a),
        mysizer(s)
    {}

    vector(vector const& a) :
        myallocator(a.myallocator),
        mysizer(a.mysizer)
    {
        // cannot throw if a.empty()
        if(size_type s = a.size())
            if(allocate(s))
                tool::construct(myend, a.begin(), s);
    }

    vector(vector&& a) noexcept :
        myallocator(static_cast<allocator_type&&>(a.myallocator)),
        mybegin(a.mybegin),
        myend(a.myend),
        mystop(a.mystop),
        mysizer(static_cast<sizer_type&&>(a.mysizer))
    {
        a.mystop = a.myend = a.mybegin = 0;
    }

    explicit vector(size_type count, value_type const& value, allocator_type const& a) :
        myallocator(a)
    {
        assign(count, value);
    }

    template<typename input_iterator_>
    vector(input_iterator_ begin, input_iterator_ end, allocator_type const& a) :
        myallocator(a)
    {
        assign(begin, end);
    }

    template<typename input_iterator_>
    vector(
        input_iterator_ begin,
        size_type count,
        typename types::ifel_type_not<types::is_int<input_iterator_>, allocator_type>::result const& a
    ) :
        myallocator(a)
    {
        assign(begin, count);
    }

    ~vector() noexcept {
        if(mybegin) {
            tool::destruct(mybegin, myend);
            myallocator.pop(mybegin, mystop);
        }
    }

    vector& operator=(vector const& a) {
        if(&a != this)
            assign(a.begin(), a.end());
        return *this;
    }

    vector& operator=(vector&& a) noexcept {
        swap(a);
        return *this;
    }

    reference operator[](size_type a) noexcept {
        ___water_assert(a < size());
        return mybegin[a];
    }

    const_reference operator[](size_type a) const noexcept {
        ___water_assert(a < size());
        return mybegin[a];
    }

    allocator_type allocator() const noexcept {
        return myallocator;
    }

    iterator assign(size_type count, value_type const& value) {
        // see the other assign
        if(count <= capacity() || grow(count))
            tool::assign_value(mybegin, myend, value, count);
        else {
            vector v(myallocator, mysizer);
            if(!v.allocate(count))
                return 0;
            tool::construct_value(v.myend, value, count);
            swap(v);
        }
        return mybegin;
    }

    template<typename input_iterator_>
    iterator assign(input_iterator_ begin, input_iterator_ end) {
        // if input_iterator_ is not an iterator (pointer or class-type) this has the same effect as
        //
        //   assign(static_cast<size_type>(begin), static_cast<value_type>(end));
        //
        // see the other assign
        typedef typename
            types::ifel_type<is_random_access_iterator<input_iterator_>, random_access_iterator_tag*,
            types::ifel_type<is_forward_iterator<input_iterator_>, forward_iterator_tag*,
            types::ifel_type<types::is_class_struct_union<input_iterator_>, input_iterator_tag*,
            void*
        > > >::result select_;
        return assign_do(begin, end, static_cast<select_>(0));
    }

    template<typename input_iterator_>
    typename types::ifel_type_not<types::is_int<input_iterator_>, iterator>::result assign(input_iterator_ begin, size_type count) {
        // replace the content of this
        //
        // return
        // - this->begin()
        // - 0 if allocator returned 0
        //
        // pre
        // - begin is not an iterator from this
        //
        // post
        // - iterators may not be valid if capacity() was less than count
        //
        // exceptions
        // - if the allocator() throws or returns 0, this is unchanged
        // - if any of these expressions throw, this could be in a half finished state:
        //
        //     value_type v1(*begin);
        //     value_type v2(v1);
        //     v1 = v2;
        //     v1 = *begin;
        //     ++begin;
        //
        // - in the half finished state this is still usable as long as the value_type is still
        //   usable after it's assignemnt operator threw
        //
        if(count <= capacity() || grow(count))
            tool::assign(mybegin, myend, begin, count);
        else {
            vector v(myallocator, mysizer);
            if(!v.allocate(count))
                return 0;
            tool::construct(v.myend, begin, count);
            swap(v);
        }
        return mybegin;
    }

    reference at(size_type a) noexcept {
        // does *not* throw if a >= size()
        ___water_assert(a < size());
        return mybegin[a];
    }

    const_reference at(size_type a) const noexcept {
        // does *not* throw if a >= size()
        ___water_assert(a < size());
        return mybegin[a];
    }

    reference back() noexcept {
        ___water_assert(!empty());
        return myend[-1];
    }

    const_reference back() const noexcept {
        ___water_assert(!empty());
        return myend[-1];
    }

    iterator begin() noexcept {
        return mybegin;
    }

    const_iterator begin() const noexcept {
        return mybegin;
    }
    
    const_iterator cbegin() const noexcept {
        return mybegin;
    }

    size_type capacity() const noexcept {
        return static_cast<size_type>(mystop - mybegin);
    }

    void clear() noexcept {
        // frees memory
        //
        // post
        // - capacity() == 0
        //
        vector(myallocator, mysizer).swap(*this);
    }

    pointer data() noexcept {
        return mybegin;
    }

    const_pointer data() const noexcept {
        return mybegin;
    }

    template<typename ...arguments_>
    iterator emplace(const_iterator at, arguments_&& ...arguments) {
        if(at == myend)
            return emplace_back(static_cast<arguments_&&>(arguments)...);
        return insert(at, value_type{static_cast<arguments_&&>(arguments)...});
    }

    template<typename ...arguments_>
    iterator emplace_back(arguments_&& ...arguments) {
        // excatly like push back except it will new value_type(arguments...)
        if(!push_back_do())
            return 0;
        new(here(myend)) value_type{static_cast<arguments_&&>(arguments)...};
        return myend++;
    }

    bool empty() const noexcept {
        return mybegin == myend;
    }

    iterator end() noexcept {
        return myend;
    }

    const_iterator end() const noexcept {
        return myend;
    }
    
    const_iterator cend() const noexcept {
        return myend;
    }

    iterator erase(const_iterator at) {
        // see the other erase
        return erase(at, at + 1);
    }

    iterator erase(const_iterator begin, const_iterator end) {
        // if end != this->end() this can throw if the value_type copy assignment operator can.
        // if it throws this can be in a half finished state, but as long as the value whose copy
        // assign threw is valid, all values are valid
        //
        // returns begin
        //
        ___water_assert(this->begin() <= begin && begin <= end && end <= this->end());
        auto
            b = const_cast<iterator>(begin),
            e = const_cast<iterator>(end);
        tool::erase(b, e, myend);
        return b;
    }

    reference front() noexcept {
        ___water_assert(!empty());
        return *mybegin;
    }

    const_reference front() const noexcept {
        ___water_assert(!empty());
        return *mybegin;
    }

    iterator insert(const_iterator at, value_type&& value) {
        ___water_assert(this->begin() <= at && at <= this->end());
        if(at == myend)
            return push_back(static_cast<value_type&&>(value));
        auto a = const_cast<iterator>(at);
        if(size() < capacity() || grow(size() + 1))
            return tool::insert_move(a, myend, static_cast<value_type&&>(value));
        vector v(myallocator, mysizer);
        if(!v.allocate(v.mysizer(size() + 1)))
            return 0;
        a = tool::insert_move_copy(v.myend, mybegin, a, myend, static_cast<value_type&&>(value));
        swap(v);
        return a;
    }

    iterator insert(const_iterator at, value_type const& value) {
        // see the other insert
        return at == myend ? push_back(value) : insert(at, static_cast<size_type>(1), value);
    }

    iterator insert(const_iterator at, size_type count, value_type const& value) {
        // see the other insert
        ___water_assert(this->begin() <= at && at <= this->end());
        auto a = const_cast<iterator>(at);
        if(count) {
            if(size() + count <= capacity() || grow(size() + count))
                return tool::insert_value(a, myend, value, count);
            vector v(myallocator, mysizer);
            if(!v.allocate(v.mysizer(size() + count)))
                return 0;
            a = tool::insert_value_copy(v.myend, mybegin, a, myend, value, count);
            swap(v);
        }
        return a;
    }

    template<typename input_iterator_>
    iterator insert(const_iterator at, input_iterator_ begin, input_iterator_ end) {
        // if input_iterator_ is not an iterator (pointer or class-type) this has the same effect as
        //
        //   insert(at, static_cast<size_type>(begin), static_cast<value_type>(end));
        //
        // see the other insert
        typedef typename
            types::ifel_type<is_random_access_iterator<input_iterator_>, random_access_iterator_tag*,
            types::ifel_type<is_forward_iterator<input_iterator_>, forward_iterator_tag*,
            types::ifel_type<types::is_class_struct_union<input_iterator_>, input_iterator_tag*,
            void*
        > > >::result select_;
        return insert_do(const_cast<iterator>(at), begin, end, static_cast<select_>(0));
    }

    template<typename input_iterator_>
    typename types::ifel_type_not<types::is_int<input_iterator_>, iterator>::result insert(const_iterator at, input_iterator_ begin, size_type count) {
        // insert count items from begin before at in this
        //
        // return
        // - iterator to the first value after the inserted range (at + count)
        // - 0 if allocator returned 0
        //
        // pre
        // - begin is not an iterator from this, or begin + size <= at
        //
        // post
        // - iterators may no be valid if capacity() was less than size() + count
        //
        // exceptions
        // - if the allocator() throws or returns 0, this is unchanged
        // - if at == end(), and something throws, this is unchanged
        // - otherwise, if any of these expressions throw, this could be in a half finished state:
        //
        //     value_type v1(*begin);
        //     value_type v2(v1);
        //     v1 = v2;
        //     v1 = *begin;
        //     ++begin;
        //
        // - in the half finished state this is still usable as long as the value_type is still
        //   usable after it's assignemnt operator threw
        //
        ___water_assert(this->begin() <= at && at <= this->end());
        auto a = const_cast<iterator>(at);
        if(count) {
            if(size() + count <= capacity() || grow(size() + count))
                return tool::insert(a, myend, begin, count);
            vector v(myallocator, mysizer);
            if(!v.allocate(v.mysizer(size() + count)))
                return 0;
            a = tool::insert_copy(v.myend, mybegin, a, myend, begin, count);
            swap(v);
        }
        return a;
    }

    size_type max_size() const noexcept {
        return static_cast<size_type>(-1) / sizeof(value_type);
    }

    void pop_back() noexcept {
        ___water_assert(!empty());
        tool::destruct(--myend);
    }

    iterator push_back(value_type&& value) {
        if(!push_back_do())
            return 0;
        new(here(myend)) value_type(static_cast<value_type&&>(value));
        return myend++;
    }

    iterator push_back(value_type const& value) {
        // returns
        // - iterator to the inserted value
        // - 0 if allocator() returns 0 if it fails instead of throwing
        //
        // post
        // - iterators may not be valid if size() == capacity() before
        // - if something threw or allocator() returns 0 this is unchanged
        //
        if(!push_back_do())
            return 0;
        new(here(myend)) value_type(value);
        return myend++;
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator(myend ? myend - 1 : myend);
    }

    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(myend ? myend - 1 : myend);
    }
    
    const_reverse_iterator crbegin() const noexcept {
        return rbegin();
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator(mybegin ? mybegin - 1 : mybegin);
    }

    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(mybegin ? mybegin - 1 : mybegin);
    }
    
    const_reverse_iterator crend() const noexcept {
        return rend();
    }

    size_type reserve(size_type count) {
        // if count > capacity() this will temporary::resize or reallocate memory to make capacity() == count
        //
        // returns
        // - capacity()
        // - 0 if reallocation failed and allocator() does not throw
        //
        // post
        // - iterators are not valid if it did reallocate
        //
        // exceptions
        // - if allocator throws or returns 0 this is unchanged
        //
        size_type c = capacity();
        if(c < count) {
            if(myallocator.resize(mybegin, c, count))
                mystop = mybegin + count;
            else if(!reallocate(count)) // no extra
                return 0;
        }
        return capacity();
    }

    iterator resize(size_type count, value_type const& value) {
        // same effect as
        //
        //   if(count > this->size())
        //     this->insert(this->end(), count - this->size(), value);
        //   if(count < this->size())
        //     this->erase(this->begin() + count, this->end());
        //
        // returns
        // - end()
        // - 0 if insert returned 0
        //
        // see
        // - insert
        // - erase
        //
        size_type s = size();
        if(count > s)
            return insert(myend, count - s, value) ? myend : 0; // can throw/fail
        if(count < s) {
            tool::destruct(mybegin + count, myend);
            myend = mybegin + count;
        }
        return myend;
    }
    
    iterator resize(size_type count) {
        // same as the other resize except it will default-construct new elements at the end
        size_type s = size();
        if(count > s) {
            if(!reserve(count))
                return 0;
            tool::construct(myend, count - s);
        }
        if(count < s) {
            tool::destruct(mybegin + count, myend);
            myend = mybegin + count;
        }
        return myend;
    }

    bool shrink(bool reallocate = false) {
        // make capacity() == size()
        //
        // pre
        // - if reallocate == false this cannot throw, if capacity() > size()
        //   it tries to temporary::resize the memory
        // - if reallocate == true this will reallocate if temporary::resize fails and capacity() > size().
        //   that could throw or fail, if it happens this is unchanged
        //
        // returns true if capacity() == size()
        //
        // post
        // - if it did reallocate iterators are not valid
        //
        if(myend != mystop) {
            if(mybegin == myend)
                clear();
            else if(myallocator.resize(mybegin, capacity(), size()))
                mystop = myend;
            else if(reallocate)
                this->reallocate(size()); // no extra
        }
        return myend == mystop;
    }

    bool shrink_to_fit() {
        // calls shrink(true)
        return shrink(true);
    }

    size_type size() const noexcept {
        return static_cast<size_type>(myend - mybegin);
    }

    sizer_type& sizer() noexcept {
        return mysizer;
    }

    sizer_type const& sizer() const noexcept {
        return mysizer;
    }

    vector& sizer(sizer_type const& a) {
        mysizer = a;
        return *this;
    }

    void swap(vector& a) noexcept {
        swap_from_swap(myallocator, a.myallocator);
        swap_from_swap(mybegin, a.mybegin);
        swap_from_swap(myend, a.myend);
        swap_from_swap(mystop, a.mystop);
        swap_from_swap(mysizer, a.mysizer);
    }

private:
    iterator allocate(size_type count) {
        // this must be empty, mybegin == 0
        myend = mybegin = myallocator.template push<value_type>(count);
        if(mybegin)
            mystop = mybegin + count;
        return mybegin;
    }

    iterator push_back_do() {
        // grow by 1, returns end or 0 if it failed
        if(myend != mystop || grow(capacity() + 1))
            return myend;
        vector v(myallocator, mysizer);
        if(!v.allocate(v.mysizer(capacity() + 1)))
            return 0;
        tool::construct_move(v.myend, mybegin, myend);
        swap(v);
        return myend;
    }

    bool grow(size_t size) noexcept {
        if(mybegin && myallocator.resize(mybegin, capacity(), size)) {
            mystop = mybegin + size;
            return true;
        }
        return false;
    }

    bool reallocate(size_type count) {
        // size must be >= this->size()
        vector v(myallocator, mysizer);
        if(!v.allocate(count)) return false;
        tool::construct_move(v.myend, mybegin, myend);
        swap(v);
        return true;
    }

    template<typename iterator_>
    iterator assign_do(iterator_ begin, iterator_ end, random_access_iterator_tag*) {
        ___water_assert(begin <= end);
        return assign(begin, static_cast<size_type>(end - begin));
    }

    template<typename iterator_>
    iterator assign_do(iterator_ begin, iterator_ end, forward_iterator_tag*) {
        size_type s = 0;
        iterator_ i(begin);
        while(i != end) {
            ++s;
            ++i;
        }
        return assign(begin, s);
    }

    template<typename iterator_>
    iterator assign_do(iterator_ begin, iterator_ end, input_iterator_tag*) {
        iterator i = mybegin;
        while(i != myend && begin != end) {
            *i = *begin; // can throw
            ++i;
            ++begin; // can throw?
        }
        if(i != myend) {
            tool::destruct(i, myend);
            myend = i;
        }
        else
            while(begin != end && (i = emplace_back(*begin)) != 0) // can throw
                ++begin; // can throw?
        return i ? mybegin : 0;
    }

    template<typename other_>
    iterator assign_do(other_& count, other_& value, void*) {
        return assign(static_cast<size_type>(count), static_cast<value_type>(value));
    }

    template<typename iterator_>
    iterator insert_do(iterator at, iterator_ begin, iterator_ end, random_access_iterator_tag*) {
        ___water_assert(begin <= end);
        return insert(at, begin, static_cast<size_type>(end - begin));
    }

    template<typename iterator_>
    iterator insert_do(iterator at, iterator_ begin, iterator_ end, forward_iterator_tag*) {
        size_type s = 0;
        iterator_ i(begin);
        while(i != end) {
            ++s;
            ++i;
        }
        return insert(at, begin, s);
    }

    template<typename iterator_>
    iterator insert_do(iterator at, iterator_ begin, iterator_ end, input_iterator_tag*) {
        auto x = at - mybegin;
        while(begin != end && (at = insert(at, *begin)) != 0) {
            ++at;
            ++begin;
        }
        return at ? mybegin + x : 0;
    }

    template<typename other_>
    iterator insert_do(iterator at, other_& count, other_& value, void*) {
        return insert(at, static_cast<size_type>(count), static_cast<value_type>(value));
    }
};

template<typename v_, typename a_, typename s_>
void swap(vector<v_, a_, s_>& a, vector<v_, a_, s_>& b) noexcept {
    a.swap(b);
}

template<typename v_, typename a_, typename s_>
bool operator==(vector<v_, a_, s_> const& a, vector<v_, a_, s_> const& b) {
    return vectool<typename vector<v_, a_, s_>::value_type>::equal(a.begin(), a.end(), b.begin(), b.end());
}

template<typename v_, typename a_, typename s_>
bool operator!=(vector<v_, a_, s_> const& a, vector<v_, a_, s_> const& b) {
    return !(a == b);
}

template<typename v_, typename a_, typename s_>
bool operator<(vector<v_, a_, s_> const& a, vector<v_, a_, s_> const& b) {
    return vectool<typename vector<v_, a_>::value_type>::less(a.begin(), a.end(), b.begin(), b.end());
}

template<typename v_, typename a_, typename s_>
bool operator>(vector<v_, a_, s_> const& a, vector<v_, a_, s_> const& b) {
    return b < a;
}

template<typename v_, typename a_, typename s_>
bool operator<=(vector<v_, a_, s_> const& a, vector<v_, a_, s_> const& b) {
    return !(b < a);
}

template<typename v_, typename a_, typename s_>
bool operator>=(vector<v_, a_, s_> const& a, vector<v_, a_, s_> const& b) {
    return !(a < b);
}

}}
#endif
