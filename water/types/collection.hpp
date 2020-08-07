// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_TYPES_COLLECTION_HPP
#define WATER_TYPES_COLLECTION_HPP
#include <water/types/result.hpp>
namespace water { namespace types {

/*

A "collection" is a collection of results. There are two types of collections in water::types:
types::list
types::link

The operations on collections defined in water::types are:

at
insert
next
offset
pull
remove
set
size

They are in headers with the same name. The tests/link.hpp and tests/list.hpp might help exaplain a
little how this works, but more documentation is needs to be written.

*/

template<typename kind_>
struct collection_tag :
    type_plain<kind_>
{};

namespace _ {

    template<typename a_>
    struct collection_tag_to_void
    {};
    
    template<typename a_>
    struct collection_tag_to_void<collection_tag<a_> > :
        type_plain<void>
    {};
    
    template<typename a_, typename = void>
    struct do_collection_tag_of :
        type_plain<void>
    {};
    
    template<typename a_>
    struct do_collection_tag_of<a_, typename collection_tag_to_void<typename a_::collection_tag>::result> :
        type_plain<typename a_::collection_tag>
    {};
    
    template<typename a_, typename = void>
    struct do_collection_kind
    {};
    
    template<typename a_>
    struct do_collection_kind<a_, typename collection_tag_to_void<typename a_::collection_tag>::result> :
        a_::collection_tag::result
    {};
    
    template<typename a_, typename = void>
    struct do_collection_kind_of :
        type_plain<void>
    {};
    
    template<typename a_>
    struct do_collection_kind_of<a_, typename collection_tag_to_void<typename a_::collection_tag>::result> :
        a_::collection_tag
    {};
    
    template<typename a_, typename = void>
    struct do_is_collection :
        false_result
    {};
    
    template<typename a_>
    struct do_is_collection<a_, typename collection_tag_to_void<typename a_::collection_tag>::result> :
        true_result
    {};
    
}

// result a_::collection_tag if it exists and is a typedef of types::collecion_tag, else void
template<typename a_>
struct collection_tag_of :
    _::do_collection_tag_of<a_>
{};

// inherits the collection kind if a_ is a collection
template<typename a_>
struct collection_kind :
    _::do_collection_kind<a_>
{};

// result the collection kind, or void if not collection
template<typename a_>
struct collection_kind_of :
    _::do_collection_kind_of<a_>
{};

// result true if a_ is a collection
template<typename a_>
struct is_collection :
    _::do_is_collection<a_>
{};

}}
#endif
