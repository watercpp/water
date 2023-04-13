// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_VECTOR_HPP
#define WATER_VECTOR_HPP
#include <water/water.hpp>
#include <water/types.hpp>
#include <water/type_traits.hpp>
#include <water/iterator.hpp>
#include <water/reverse_iterator.hpp>
#include <water/allocator.hpp>
#include <water/vectool.hpp>
#include <water/swap.hpp>
#include <water/initializer_list.hpp>
namespace water {

/*

water::vector is very similar to std::vector and there are few if any reasons to use this instead.
It was made because it was easy to borrow the code from water::temporary::vector

template
- value_ is the value_type
- allocator_ is the allocator. the default, void, means use water::allocator
- sizer_ is the resize function, the default, void, means use water::vector_sizer.

Differences from std::vector:
- at(size_type i) does not throw if i >= size()
- get_allocator() does not exist, this has allocator() instead
- reserve(max_size() + 1) does not throw
- max_size() always returns size_type_max / sizeof(value_type)
- vector operators < > <= >= need value_type operators == and <
- vector<bool> is not specialized

The allocator_ and sizer_ template arguments are totally different.
This can be used with nothrow allocators, see below.

Constructor, assignment, assign(), insert() for arrays. std::vector does not have this:

    int array[] {1, 2, 3};
    vector<int> v{a};
    v = a;
    v.insert(v.end(), a);
    
    vector<char> c{"hello"};

Constructor, assignment, assign(), insert() for any type with begin() and end() functions,
std::vector does not have that:

    std::deque<short> d{}; // has begin() and end(), value converts to int
    
    vector<int> v{d};
    v = d;
    v.insert(v.end(), d);

Constructor, assign and insert functions for begin-size ranges, std::vector does not have it:

    input_iterator begin;
    size_t size;
    ...
    vector<value> v(begin, size);
    v.assign(begin, size);
    v.insert(v.end(), begin, size);
    
iterator and const_iterator are pointers

Exceptions
- if the value_type copy constructor or copy assignment operator throws from assign() erase() or
  insert() this can be in a half finished state just like std::vector.
- erase()-ing from the end can never throw or fail
- when insert()-ing at the end this will always be unchanged if something throws
- if something throws from anything other than assign() erase() or insert() this is unchanged

The allocator_ should look like this:

    struct allocator {
        // must have:
        template<typename type_> type_* allocate(size_t count);
        template<typename type_> void   free (void* pointer, size_t count) noexcept;
        
        // optional:
        bool operator==(allocator const&) const noexcept;
    };

swap(allocator&, allocator&) and move constructor must work and never ever throw or fail.
If allocator == allocator works, copy assignment can avoid allocation when the vectors allocators
compare equal.

The allocator can fail by throwing or returning 0. To make it possible to see return-0-failures
functions in this also returns 0. For example push_back() returns an iterator (a pointer) that can
be 0.

The constructors and copy-assignment operator cannot return 0, if the allocator returned 0, empty() == true

A vector holds an array of values. when the array is full it must allocate a larger array to fit
more values and copy all values from the old to the new. To avoid this a vector will allocate a
larger array than it currently needs.

The sizer_ template argument is used to decide how much memory to allocate:

    struct sizer {
        size_t operator()(size_t current_capacity, size_t need_capacity);
    };

The sizer function is called when the vector needs to grow
- current_capacity is the vector.capacity(), it can be 0
- need_capacity is the minimum capacity the vector needs, it is always greater than current_capacity
- must return something >= need_size

swap(sizer&, sizer&) and move constructor must work and never ever throw or fail.

*/



#ifdef WATER_VECTOR_SIZER
    
typedef WATER_VECTOR_SIZER vector_sizer;

#else

struct vector_sizer
{
    size_t operator()(size_t now, size_t need) const {
        ___water_assert(now < need);
        need += need / 2;
        if(need > 2047)
            return need;
        now = 8;
        while(now < need)
            now *= 2;
        return now;
    }
};

#endif



template<typename allocator_, typename = void>
struct vector_can_compare_allocators {
    static bool constexpr result = false;
};

template<typename allocator_>
struct vector_can_compare_allocators<allocator_, to_void<decltype(make_type<allocator_ const&>() == make_type<allocator_ const&>())>> {
    static bool constexpr result = true;
};

template<typename allocator_>
auto vector_allocators_are_equal(allocator_ const& a, allocator_ const& b) -> ifel<vector_can_compare_allocators<allocator_>::result, bool> {
    return a == b;
}

template<typename allocator_>
auto vector_allocators_are_equal(allocator_ const&, allocator_ const&) -> ifel<!vector_can_compare_allocators<allocator_>::result, bool> {
    return false;
}



template<typename value_, typename allocator_ = void, typename sizer_ = void>
class vector
{
public:
    typedef value_
        value_type,
        *pointer,
        *iterator,
        &reference;
    typedef value_ const
        *const_pointer,
        *const_iterator,
        &const_reference;
    using reverse_iterator = water::reverse_iterator<iterator>;
    using const_reverse_iterator = water::reverse_iterator<const_iterator>;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using allocator_type = if_not_void<allocator_, water::allocator>;
    using sizer_type = if_not_void<sizer_, vector_sizer>;

private:
    typedef vectool<value_type> tool;

private:
    value_type
        *mybegin = 0,
        *myend = 0,
        *mystop = 0; // end of memory (if myend == mystop it is full)
    allocator_type myallocator{};
    sizer_type mysizer{};

public:

    constexpr vector() = default;

    constexpr explicit vector(allocator_type const& a, sizer_type const& s = sizer_type{}) :
        myallocator(a),
        mysizer(s)
    {}

    constexpr explicit vector(sizer_type const& s, allocator_type const& a = allocator_type{}) :
        myallocator(a),
        mysizer(s)
    {}

    vector(vector const& a) :
        myallocator(a.myallocator),
        mysizer(a.mysizer)
    {
        if(size_type s = a.size())
            if(allocate(s))
                tool::construct(myend, a.begin(), s);
    }

    vector(vector&& a) noexcept :
        mybegin(a.mybegin),
        myend(a.myend),
        mystop(a.mystop),
        myallocator(static_cast<allocator_type&&>(a.myallocator)),
        mysizer(static_cast<sizer_type&&>(a.mysizer))
    {
        a.mystop = a.myend = a.mybegin = 0;
    }
    
    vector(initializer_list<value_type> l, allocator_type const& a = allocator_type{}) :
        myallocator(a)
    {
        assign(l.begin(), l.end());
    }

    explicit vector(size_type count, allocator_type const& a = allocator_type{}) :
        myallocator(a)
    {
        resize(count);
    }
    
    vector(size_type count, value_type const& value, allocator_type const& a = allocator_type{}) :
        myallocator(a)
    {
        assign(count, value);
    }

    template<
        typename input_iterator_,
        typename = decltype(value_type{*++make_type<input_iterator_&>()})
    >
    vector(input_iterator_ begin, input_iterator_ end, allocator_type const& a = allocator_type{}) :
        myallocator(a)
    {
        assign(begin, end);
    }

    template<
        typename input_iterator_,
        typename = decltype(value_type(*++make_type<input_iterator_&>()))
    >
    vector(input_iterator_ begin, size_type count, allocator_type const& a = allocator_type{}) :
        myallocator(a)
    {
        assign(begin, count);
    }
    
    template<
        typename range_,
        typename = decltype(make_type<range_&&>().begin() != make_type<range_&&>().end()),
        typename = decltype(value_type(*make_type<range_&&>().begin())),
        typename = ifel<!equal<vector, no_const_or_reference<range_>>, void>
    >
    vector(range_&& r, allocator_type const& a = allocator_type{}) :
        myallocator(a)
    {
        assign(r.begin(), r.end());
    }
    
    template<
        typename type_,
        size_t size_,
        typename = decltype(value_type(make_type<type_ const&>()))
    >
    vector(type_ const (&array)[size_], allocator_type const& a = allocator_type{}) :
        myallocator(a)
    {
        assign(array, size_);
    }

    ~vector() noexcept {
        if(mybegin) {
            tool::destruct(mybegin, myend);
            myallocator.template free<value_type>(mybegin, static_cast<size_type>(mystop - mybegin));
        }
    }

    vector& operator=(vector const& a) {
        // if the allocators compare equal, assign. otherwise copy
        if(&a != this) {
            if(vector_allocators_are_equal(myallocator, a.myallocator)) {
                mysizer = a.mysizer;
                assign(a.begin(), a.end());
            }
            else {
                vector s{a};
                swap(s);
            }
        }
        return *this;
    }

    vector& operator=(vector&& a) noexcept {
        swap(a);
        return *this;
    }
    
    vector& operator=(initializer_list_std_or_not_empty<value_type> a) {
        assign(a.begin(), a.end());
        return *this;
    }
    
    template<
        typename range_,
        typename = decltype(make_type<range_&&>().begin() != make_type<range_&&>().end()),
        typename = decltype(value_type(*make_type<range_&&>().begin())),
        typename = ifel<!equal<vector, no_const_or_reference<range_>>, void>
    >
    vector& operator=(range_&& a) {
        assign(a.begin(), a.end());
        return *this;
    }
    
    template<
        typename type_,
        size_t size_,
        typename = decltype(value_type(make_type<type_ const&>()))
    >
    vector& operator=(type_ const (&a)[size_]) {
        assign(a, size_);
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

    allocator_type& allocator() noexcept {
        return myallocator;
    }

    allocator_type const& allocator() const noexcept {
        return myallocator;
    }

    vector& allocator(allocator_type const& a) {
        myallocator = a;
        return *this;
    }

    iterator assign(size_type count, value_type const& value) {
        // see the other assign
        if(count <= capacity())
            tool::assign_value(mybegin, myend, value, count);
        else {
            vector v(myallocator, mysizer);
            if(!v.allocate(v.mysizer(capacity(), count)))
                return 0;
            tool::construct_value(v.myend, value, count);
            swap(v);
        }
        return mybegin;
    }

    template<
        typename input_iterator_,
        typename = decltype(value_type(*++make_type<input_iterator_&>()))
    >
    iterator assign(input_iterator_ begin, input_iterator_ end) {
        // see the other assign
        using select_ = at_pack<
            is_random_access_iterator<input_iterator_> ? 0 :
            is_forward_iterator<input_iterator_> ? 1 :
            2,
            random_access_iterator_tag*,
            forward_iterator_tag*,
            input_iterator_tag*
        >;
        return assign_do(begin, end, static_cast<select_>(0));
    }

    template<
        typename input_iterator_,
        typename = decltype(value_type(*++make_type<input_iterator_&>()))
    >
    iterator assign(input_iterator_ begin, size_type count) {
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
        // - no iterators are valid if capacity() was less than count
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
        if(count <= capacity())
            tool::assign(mybegin, myend, begin, count);
        else {
            vector v(myallocator, mysizer);
            if(!v.allocate(v.mysizer(capacity(), count)))
                return 0;
            tool::construct(v.myend, begin, count);
            swap(v);
        }
        return mybegin;
    }
    
    iterator assign(initializer_list<value_type> a) {
        // see the other assign
        return assign(a.begin(), a.end());
    }
    
    template<
        typename range_,
        typename = decltype(make_type<range_&&>().begin() != make_type<range_&&>().end()),
        typename = decltype(value_type(*make_type<range_&&>().begin()))
    >
    iterator assign(range_&& a) {
        // see the other assign
        return assign(a.begin(), a.end());
    }
    
    template<
        typename type_,
        size_t size_,
        typename = decltype(value_type(make_type<type_ const&>()))
    >
    iterator assign(type_ const (&a)[size_]) {
        // see the other assign
        return assign(a, size_);
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
        tool::destruct(mybegin, myend);
        myend = mybegin;
    }

    pointer data() noexcept {
        return mybegin;
    }

    const_pointer data() const noexcept {
        return mybegin;
    }

    template<typename ...arguments_>
    iterator emplace(const_iterator at, arguments_&& ...arguments) {
        // this will emplace_back if at == end otherwise insert(at, value_type&&)
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
        // if end != this->end() this can throw if the value_type copy assignment operator can. if it throws this can be in a half
        // finished state, but as long as the value whose copy assign threw is valid, all values are valid
        //
        // returns begin
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
        if(size() < capacity())
            return tool::insert_move(a, myend, static_cast<value_type&&>(value));
        vector v(myallocator, mysizer);
        if(!v.allocate(v.mysizer(capacity(), size() + 1)))
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
            if(size() + count <= capacity())
                return tool::insert_value(a, myend, value, count);
            vector v(myallocator, mysizer);
            if(!v.allocate(v.mysizer(capacity(), size() + count)))
                return 0;
            a = tool::insert_value_copy(v.myend, mybegin, a, myend, value, count);
            swap(v);
        }
        return a;
    }

    template<
        typename input_iterator_,
        typename = decltype(value_type(*++make_type<input_iterator_&>()))
    >
    iterator insert(const_iterator at, input_iterator_ begin, input_iterator_ end) {
        // see the other insert
        using select_ = at_pack<
            is_random_access_iterator<input_iterator_> ? 0 :
            is_forward_iterator<input_iterator_> ? 1 :
            2,
            random_access_iterator_tag*,
            forward_iterator_tag*,
            input_iterator_tag*
        >;
        return insert_do(const_cast<iterator>(at), begin, end, static_cast<select_>(0));
    }

    template<
        typename input_iterator_,
        typename = decltype(value_type(*++make_type<input_iterator_&>()))
    >
    iterator insert(const_iterator at, input_iterator_ begin, size_type count) {
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
        // - no iterators are valid if capacity() was less than size() + count
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
        // - in the half finished state this is still usable as long as the value_type is still usable after it's assignemnt operator threw
        //
        ___water_assert(this->begin() <= at && at <= this->end());
        auto a = const_cast<iterator>(at);
        if(count) {
            if(size() + count <= capacity())
                return tool::insert(a, myend, begin, count);
            vector v(myallocator, mysizer);
            if(!v.allocate(v.mysizer(capacity(), size() + count)))
                return 0;
            a = tool::insert_copy(v.myend, mybegin, a, myend, begin, count);
            swap(v);
        }
        return a;
    }
    
    iterator insert(const_iterator at, initializer_list<value_type> a) {
        // see the other insert
        return insert(at, a.begin(), a.end());
    }
    
    template<
        typename range_,
        typename = decltype(make_type<range_&&>().begin() != make_type<range_&&>().end()),
        typename = decltype(value_type(*make_type<range_&&>().begin()))
    >
    iterator insert(const_iterator at, range_&& a) {
        // see the other insert
        return insert(at, a.begin(), a.end());
    }
    
    template<
        typename type_,
        size_t size_,
        typename = decltype(value_type(make_type<type_ const&>()))
    >
    iterator insert(const_iterator at, type_ const (&a)[size_]) {
        // see the other insert
        return insert(at, a, size_);
    }

    size_type max_size() const noexcept {
        return static_cast<size_type>(-1) / sizeof(value_type);
    }

    void pop_back() {
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
        // - 0 if allocator returned 0
        //
        // post
        // - no iterators are valid if capacity() == size() before
        //
        // exceptions
        // - if allocator throws or returns 0 this is unchanged
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
        // if count > capacity() this will reallocate it's memory to make capacity() == count
        //
        // returns
        // - capacity()
        // - 0 if allocator returned 0
        //
        // post
        // - iterators are not valid if count > capacity()
        //
        // exceptions
        // - if allocator throws or returns 0 this is unchanged
        //
        size_type c = capacity();
        if(c >= count)
            return c;
        vector v(myallocator, mysizer);
        if(!v.allocate(count)) return 0;
        tool::construct_move(v.myend, mybegin, myend);
        swap(v);
        return capacity();
    }

    iterator resize(size_type count, value_type const& value) {
        // same effect as
        //
        //   if(count > size())
        //     insert(end(), count - size(), value);
        //   if(count < size())
        //     erase(begin() + count, end());
        //
        // returns
        // - end()
        // - 0 if insert returned 0
        //
        // exceptions
        // - if anything throws or allocator returns 0 this is unchanged
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
        else if(count < s) {
            tool::destruct(mybegin + count, myend);
            myend = mybegin + count;
        }
        return myend;
    }

    bool shrink_to_fit() {
        // returns true if capacity() == size()
        if(capacity() == size())
            return true;
        vector v(myallocator, mysizer);
        if(size()) {
            if(!v.allocate(size()))
                return false;
            tool::construct_move(v.myend, mybegin, myend);
        }
        swap(v);
        return true;
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
        swap_from_swap(mybegin, a.mybegin);
        swap_from_swap(myend, a.myend);
        swap_from_swap(mystop, a.mystop);
        swap_from_swap(myallocator, a.myallocator);
        swap_from_swap(mysizer, a.mysizer);
    }

private:

    iterator push_back_do() {
        // grow by 1, returns end or 0 if it failed
        if(myend != mystop)
            return myend;
        vector v(myallocator, mysizer);
        if(!v.allocate(v.mysizer(capacity(), capacity() + 1)))
            return 0;
        tool::construct_move(v.myend, mybegin, myend);
        swap(v);
        return myend;
    }

    bool allocate(size_type count) {
        myend = mybegin = myallocator.template allocate<value_type>(count);
        if(mybegin) {
            mystop = mybegin + count;
            return true;
        }
        return false;
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



}
#endif
