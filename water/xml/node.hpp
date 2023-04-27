// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_XML_NODE_HPP
#define WATER_XML_NODE_HPP
#include <water/xml/decode.hpp>
namespace water { namespace xml {

template<typename memory_, typename other_>
struct node_if_no_memory {};

template<typename other_>
struct node_if_no_memory<void, other_> { struct result{}; };

template<>
struct node_if_no_memory<void, void> {};

//
// DOM tree node
//
// This is just a pointer into an underlying structure owned by xml::memory. It is very small and cheap
// to copy. All nodes are owned by an xml::memory and freed when that memory is destroyed or clear()-ed.
//
// template
// - char_ is the caracter type
// - memory_ is the xml::memory type or void. If its void, this is a constant node that cannot be modified
//
// This is a doubly linked list, and the next() previous() last() first() functions are used to move around.
//
// in() is the node that this is inside of, or false if its not inside another node
//
// nodes() is the nodes this element contains
// attributes() is the attributes this element has
//
// name() is the tag-name or attribute-type, text content has no name
// value() is the text content or attribute-value, tags have no value
// first_value() is the first text content found in nodes(), short way to read content from the tag
//
// Use create() or the xml::memory create() function to create new empty nodes.
//
// All functions return something that converts to true or false. And all functions are safe to call even
// if this does not point to a node. This makes it possible to chain many function calls that reutrn node
// without checking the return value of each one
//
//   auto node = document_node.nodes().find("root").nodes("hello").attributes("world");
//
// If any function call in a chain fails, this will return something that converts to false. And when a
// a function fails no other function in that chain will have any effect.
//
// This makes it possible to create a DOM tree with a non-throwing allocator without much error checking,
// just use xml::memory allocate_has_failed() after to see if it failed at any time before.
//
// nodes_or_add(name) and attributes_or_add(name) can be useful to add or overwrite something. It will add
// if it does not exists, otherwise return the first existing with that name.
//

template<typename char_ = char, typename memory_ = void>
class node
{
    struct node_is_const;
    friend class node<char_>;
    using memory_node = xml::memory_node<char_>;

public:
    using char_type = char_;
    using memory_type = memory_;
    using text_type = text<char_type const*>;
    using node_if_mutable = ifel<water::equal<memory_, void>, node_is_const, node<char_, memory_>>;

private:
    memory_type *mym = 0; // can never be 0 if memory_ != void and my != 0
    memory_node *my = 0;

private:
    node(memory_type *m, memory_node *n) :
        mym{m},
        my{n}
    {}

public:
    constexpr node() = default;

    node(if_not_void<memory_type, node_is_const>& m, memory_node *n) :
        mym{&m},
        my{n}
    {}

    template<typename other_>
    node(node<char_, other_> a, typename node_if_no_memory<memory_, other_>::result = {}) :
        my{a.my}
    {}

    bool operator==(node const& a) const {
        return my == a.my;
    }

    bool operator!=(node const& a) const {
        return my != a.my;
    }

    explicit operator bool() const {
        return my != 0;
    }

    memory_type* memory_pointer() const {
        return mym;
    }

    node first() const {
        return { mym, my ? first(my) : 0 };
    }

    node last() const {
        return { mym, my ? last(my) : 0 };
    }

    node next() const {
        return { mym, my ? my->next : 0 };
    }

    node previous() const {
        return { mym, my ? my->previous : 0 };
    }

    node in() const {
        return { mym, my ? my->in : 0 };
    }

    node attributes() const {
        return { mym, my ? my->attributes : 0 };
    }

    template<typename iterator_>
    node attributes(iterator_ begin, iterator_ end) const {
        return attributes().find(begin, end);
    }

    template<typename iterator_>
    node attributes(iterator_ begin, size_t size) const {
        return attributes().find(begin, size);
    }

    template<typename char2_, size_t size_>
    node attributes(char2_ const (&name)[size_]) const {
        return attributes().find(name);
    }

    template<typename range_, typename = void_if_range<range_>>
    node attributes(range_ const& name) const {
        return attributes().find(name.begin(), size_from(name));
    }

    node nodes() const {
        return { mym, my ? my->nodes : 0 };
    }

    template<typename iterator_>
    node nodes(iterator_ begin, iterator_ end) const {
        return nodes().find(begin, end);
    }

    template<typename iterator_>
    node nodes(iterator_ begin, size_t size) const {
        return nodes().find(begin, size);
    }

    template<typename char2_, size_t size_>
    node nodes(char2_ const (&name)[size_]) const {
        return nodes().find(name);
    }

    template<typename range_, typename = void_if_range<range_>>
    node nodes(range_ const& name) const {
        return nodes().find(name.begin(), size_from(name));
    }

    text_type name() const {
        if(!my)
            return {};
        return {
            static_cast<char_type const*>(static_cast<void const*>(my->name_begin)),
            static_cast<char_type const*>(static_cast<void const*>(my->name_end))
        };
    }

    text_type value() const {
        if(!my)
            return {};
        if(my->value_begin > my->value_end) { // decode this
            auto
                *b = my->value_end,
                *e = my->value_begin;
            if(my->in && my->in->attributes && my->in->attributes == first(my))
                e = decode_attribute(b, e);
            else
                e = decode(b, e);
            my->value_begin = b;
            my->value_end = e;
        }
        return {
            static_cast<char_type const*>(static_cast<void const*>(my->value_begin)),
            static_cast<char_type const*>(static_cast<void const*>(my->value_end))
        };
    }

    text_type first_value() const {
        // value of first content node in nodes()
        if(my)
            if(memory_node *n = my->nodes)
                do {
                    if(n->name_begin == n->name_end)
                        return node{ mym, n }.value();
                    n = n->next;
                } while(n);
        return {};
    }

    template<typename iterator_>
    node find(iterator_ begin, size_t size) const {
        // find the first with name == begin,size inclduing this one
        // use the next() to find the next not including this one
        if(!size)
            return {};
        memory_node *n = my;
        while(n && !name_equal(n, begin, size))
            n = n->next;
        return { mym, n };
    }

    template<typename iterator_>
    node find(iterator_ begin, iterator_ end) const {
        return find(begin, size_from(begin, end));
    }

    template<typename char2_, size_t size_>
    node find(char2_ const (&name)[size_]) const {
        return find(name, size_ - (name[size_ - 1] ? 0 : 1));
    }

    template<typename range_>
    node find(range_ const& name) const {
        return find(name.begin(), size_from(name));
    }

    template<typename iterator_>
    node next(iterator_ begin, iterator_ end) const {
        return next().find(begin, end);
    }

    template<typename iterator_>
    node next(iterator_ begin, size_t size) const {
        return next().find(begin, size);
    }

    template<typename char2_, size_t size_>
    node next(char2_ const (&name)[size_]) const {
        return next().find(name);
    }

    template<typename range_, typename = void_if_range<range_>>
    node next(range_ const& name) const {
        return next().find(name.begin(), size_from(name));
    }

    template<typename iterator_>
    node previous(iterator_ begin, size_t size) const {
        // find the closest previous node with name == begin,size
        if(!size)
            return {};
        memory_node *n = my;
        if(n) while((n = n->previous) != 0 && !name_equal(n, begin, size));
        return { mym, n };
    }

    template<typename iterator_>
    node previous(iterator_ begin, iterator_ end) const {
        return previous(begin, size_from(begin, end));
    }

    template<typename char2_, size_t size_>
    node previous(char2_ const (&name)[size_]) const {
        return previous(name, size_ - (name[size_ - 1] ? 0 : 1));
    }

    template<typename range_, typename = void_if_range<range_>>
    node previous(range_ const& name) const {
        return previous(name.begin(), size_from(name));
    }

public:
    node_if_mutable create() {
        if(!mym)
            return {};
        return mym->template create<char_type>();
    }

    node_if_mutable remove_all_nodes() {
        if(!my)
            return {};
        memory_node *n = my->nodes;
        while(n) {
            n->in = 0;
            n = n->next;
        }
        n = my->nodes;
        my->nodes = 0;
        return { mym, n };
    }

    node_if_mutable remove_all_attributes() {
        if(!my)
            return {};
        memory_node *n = my->attributes;
        while(n) {
            n->in = 0;
            n = n->next;
        }
        n = my->attributes;
        my->attributes = 0;
        return { mym, n };
    }

    node_if_mutable nodes(node_if_mutable a) {
        // insert the node-list a at the end of nodes()
        if(!my || !a.my)
            return {};
        memory_node *n = first(a.my);
        if(!my->nodes)
            my->nodes = n;
        else {
            memory_node *l = last(my->nodes);
            (l->next = n)->previous = l;
        }
        while(n) {
            n->in = my;
            n = n->next;
        }
        return *this;
    }

    template<typename iterator_>
    node_if_mutable nodes_or_add(iterator_ begin, size_t size) {
        // find the first with name == begin,size in nodes()
        // if it does not exist, this->nodes(create().name(begin, size))
        if(!size || !my)
            return {};
        node r = nodes(begin, size);
        if(!r) {
            r = create().name(begin, size);
            nodes(r);
        }
        return r;
    }

    template<typename iterator_>
    node_if_mutable nodes_or_add(iterator_ begin, iterator_ end) {
        return nodes_or_add(begin, size_from(begin, end));
    }

    template<typename char2_, size_t size_>
    node_if_mutable nodes_or_add(char2_ const (&name)[size_]) {
        return nodes_or_add(name, size_ - (name[size_ - 1] ? 0 : 1));
    }

    template<typename range_, typename = void_if_range<range_>>
    node_if_mutable nodes_or_add(range_ const& name) {
        return nodes_or_add(name.begin(), size_from(name));
    }

    node_if_mutable attributes(node_if_mutable a) {
        // insert the node-list a at the end of attributes()
        if(!my || !a.my)
            return {};
        memory_node *n = first(a.my);
        if(!my->attributes)
            my->attributes = n;
        else {
            memory_node *l = last(my->attributes);
            (l->next = n)->previous = l;
        }
        while(n) {
            n->in = my;
            n = n->next;
        }
        return *this;
    }

    template<typename iterator_>
    node_if_mutable attributes_or_add(iterator_ begin, size_t size) {
        // find the first with name == begin,size in attributes()
        // if it does not exist, this->attributes(create().name(begin, size))
        if(!size || !my)
            return {};
        node r = attributes(begin, size);
        if(!r) {
            r = create().name(begin, size);
            attributes(r);
        }
        return r;
    }

    template<typename iterator_>
    node_if_mutable attributes_or_add(iterator_ begin, iterator_ end) {
        return attributes_or_add(begin, size_from(begin, end));
    }

    template<typename char2_, size_t size_>
    node_if_mutable attributes_or_add(char2_ const (&name)[size_]) {
        return attributes_or_add(name, size_ - (name[size_ - 1] ? 0 : 1));
    }

    template<typename range_, typename = void_if_range<range_>>
    node_if_mutable attributes_or_add(range_ const& name) {
        return attributes_or_add(name.begin(), size_from(name));
    }

    node_if_mutable push_back(node a) {
        // insert a at the back of this->nodes()
        // return a
        if(!my || !a.my)
            return {mym, 0};
        a.remove();
        if(!my->nodes)
            my->nodes = a.my;
        else {
            memory_node *l = last(my->nodes);
            (l->next = a.my)->previous = l;
        }
        a.my->in = my;
        return a;
    }

    node_if_mutable push_back() {
        return push_back(create());
    }

    node_if_mutable push_front(node a) {
        // insert a at the front of this->nodes()
        // return a
        if(!my || !a.my)
            return {mym, 0};
        a.remove();
        if(my->nodes)
            (my->nodes->previous = a.my)->next = my->nodes;
        my->nodes = a.my;
        a.my->in = my;
        return a;
    }

    node_if_mutable push_front() {
        return push_front(create());
    }

    node_if_mutable& remove() {
        // remove this, return this
        if(my) {
            if(my->previous)
                my->previous->next = my->next;
            else if(my->in) {
                if(my->in->nodes == my)
                    my->in->nodes = my->next;
                else
                    my->in->attributes = my->next;
            }
            if(my->next)
                my->next->previous = my->previous;
            my->next = my->previous = my->in = 0;
        }
        return *this;
    }

    node_if_mutable insert_before(node_if_mutable before) {
        // insert this before before, return this
        // will this->remove() first
        if(!my || !before.my)
            return {};
        remove();
        my->in = before.my->in;
        if(before.my->previous)
            (my->previous = before.my->previous)->next = my;
        else if(before.my->in) {
            if(before.my->in->nodes == before.my)
                before.my->in->nodes = my;
            else
                before.my->in->attributes = my;
        }
        (my->next = before.my)->previous = my;
        return *this;
    }

    node_if_mutable insert_after(node_if_mutable after) {
        // insert this after after, returns this
        // will this->reomve() first
        if(!my || !after.my)
            return {};
        remove();
        my->in = after.my->in;
        if(after.my->next)
            (my->next = after.my->next)->previous = my;
        (after.my->next = my)->previous = after.my;
        return *this;
    }

    template<typename iterator_>
    node_if_mutable name(iterator_ begin, size_t size) {
        if(!my)
            return *this;
        using uchar_ = typename memory_node::char_type;
        size_t s = static_cast<size_t>(my->name_end_memory - my->name_begin);
        if(s < size) {
            // if this has name and value, maybe it will fit after the value?
            // do not move the value if decoded because someone might have a pointer to it.
            if(my->value_end_memory) {
                bool decoded = my->value_begin <= my->value_end;
                value(); // decode
                if(!decoded && my->name_end_memory == my->value_begin && s + static_cast<size_t>(my->value_end_memory - my->value_end) >= size) {
                    // can make space by moving value if its after name, was not decoded and enough space after
                    auto
                        *f = my->value_end,
                        *t = f + size - s;
                    while(f != my->value_begin)
                        *--f = *--t;
                    my->name_end_memory = my->value_begin += size - s;
                    my->value_end += size - s;
                    s = size;
                }
                else if(my->value_end_memory == my->name_begin && s + static_cast<size_t>(my->value_end_memory - my->value_end) >= size) {
                    // if name after value, can make space by taking from value
                    my->name_begin = my->name_end = my->value_end_memory -= size - s;
                    s = size;
                }
                else if(static_cast<size_t>(my->value_end_memory - my->value_end) >= size) {
                    // else maybe it will fit after value anyway
                    my->name_end_memory = my->value_end_memory;
                    my->name_begin = my->name_end = my->value_end_memory -= size;
                    s = size;
                }
            }
            if(s < size)  {
                // allocate :(
                uchar_ *a = static_cast<uchar_*>(mym->allocate(size * sizeof(uchar_), alignof(uchar_)));
                if(!a)
                    return {};
                // see if value can get memory. it can if it has no memory or if it has the memory before this (never move value)
                // could move value if decoded = false, maybe a good idea because this is an attribute with probably not much text?
                if(my->name_end_memory) {
                    if(!my->value_end_memory) {
                        my->value_begin = my->value_end = my->name_begin;
                        my->value_end_memory = my->name_end_memory;
                    }
                    else if(my->value_end_memory == my->name_begin)
                        my->value_end_memory = my->name_end_memory;
                }
                my->name_begin = my->name_end = a;
                my->name_end_memory = a + size;
            }
        }
        // copy
        uchar_ *b = my->name_begin;
        my->name_end = b;
        while(size) {
            *b++ = static_cast<uchar_>(*begin);
            ++begin;
            --size;
        }
        my->name_end = b; // set this last, if iterator_ throws this will be empty and never contain half utf-8
        return *this;
    }

    template<typename iterator_>
    node_if_mutable name(iterator_ begin, iterator_ end) {
        return name(begin, size_from(begin, end));
    }

    template<typename char2_, size_t size_>
    node_if_mutable name(char2_ const (&name)[size_]) {
        return this->name(name, size_ - (name[size_ - 1] ? 0 : 1));
    }

    template<typename range_, typename = void_if_range<range_>>
    node_if_mutable name(range_ const& name) {
        return this->name(name.begin(), size_from(name));
    }

    template<typename iterator_>
    node_if_mutable value(iterator_ begin, size_t size) {
        if(!my)
            return *this;
        using uchar_ = typename memory_node::char_type;
        auto *b = my->value_begin < my->value_end ? my->value_begin : my->value_end;
        size_t s = static_cast<size_t>(my->value_end_memory - b);
        if(s < size) {
            // if this has name and value, maybe it will fit if some space is stolen from the name?
            if(my->name_end_memory) {
                if(b == my->name_end_memory && s + static_cast<size_t>(my->name_end_memory - my->name_end) >= size) {
                    // name before value, can take from name
                    b = my->value_begin = my->value_end = my->name_end_memory -= size - s;
                    s = size;
                }
                else if(static_cast<size_t>(my->name_end_memory - my->name_end) >= size) {
                    // value fits in end of name
                    my->value_end_memory = my->name_end_memory;
                    b = my->value_begin = my->value_end = my->name_end_memory -= size;
                    s = size;
                }
            }
            if(s < size)  {
                // allocate :(
                uchar_ *a = static_cast<uchar_*>(mym->allocate(size * sizeof(uchar_), alignof(uchar_)));
                if(!a)
                    return {};
                // see if name can get memory. it can if it has no memory or if it has the memory before this (never move name)
                if(my->value_end_memory) {
                    if(!my->name_end_memory) {
                        my->name_begin = my->name_end = b;
                        my->name_end_memory = my->value_end_memory;
                    }
                    else if(my->name_end_memory == b)
                        my->name_end_memory = my->value_end_memory;
                }
                b = my->value_begin = my->value_end = a;
                my->value_end_memory = a + size;
            }
        }
        // copy
        my->value_begin = b; // because maybe value_end < value_begin
        my->value_end = b;
        while(size) {
            *b++ = static_cast<uchar_>(*begin);
            ++begin;
            --size;
        }
        my->value_end = b; // set this last, if iterator_ throws this will be empty and never contain half utf-8
        return *this;
    }

    template<typename iterator_>
    node_if_mutable value(iterator_ begin, iterator_ end) {
        return value(begin, size_from(begin, end));
    }

    template<typename char2_, size_t size_>
    node_if_mutable value(char2_ const (&text)[size_]) {
        return value(text, size_ - (text[size_ - 1] ? 0 : 1));
    }

    template<typename range_, typename = void_if_range<range_>>
    node_if_mutable value(range_ const& text) {
        return value(text.begin(), size_from(text));
    }

    template<typename iterator_>
    node_if_mutable first_value(iterator_ begin, size_t size) {
        // set the value of the first content node in nodes(), or create a node if there is none and size != 0. the created node will be first in nodes()
        if(!my)
            return *this;
        memory_node *n = my->nodes;
        while(n && n->name_begin != n->name_end)
            n = n->next;
        if(!n) {
            if(!size)
                return *this; // all is good
            auto a = mym->template create<char_type>(); // leak this if set value fails
            if(!a)
                return {};
            n = a.my;
        }
        if(!node{mym, n}.value(begin, size))
            return {};
        if(!n->in) {
            n->in = my;
            n->next = my->nodes;
            if(n->next)
                n->next->previous = n;
            my->nodes = n;
        }
        return *this;
    }

    template<typename iterator_>
    node_if_mutable first_value(iterator_ begin, iterator_ end) {
        return first_value(begin, size_from(begin, end));
    }

    template<typename char2_, size_t size_>
    node_if_mutable first_value(char2_ const (&text)[size_]) {
        return first_value(text, size_ - (text[size_ - 1] ? 0 : 1));
    }

    template<typename range_, typename = void_if_range<range_>>
    node_if_mutable first_value(range_ const& text) {
        return first_value(text.begin(), size_from(text));
    }

private:
    template<typename iterator_>
    static bool name_equal(memory_node *n, iterator_ begin, size_t size) {
        if(static_cast<size_t>(n->name_end - n->name_begin) == size) {
            auto ni = n->name_begin;
            iterator_ i = begin;
            while(*i == *ni && ++ni != n->name_end) ++i;
            if(ni == n->name_end)
                return true;
        }
        return false;
    }

    static memory_node* first(memory_node *a) {
        while(a->previous) a = a->previous;
        return a;
    }

    static memory_node* last(memory_node *a) {
        while(a->next) a = a->next;
        return a;
    }
};

}}
#endif
